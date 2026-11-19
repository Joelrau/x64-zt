#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

#include <iw8/zone/loader.hpp>
#include <io/game_files.hpp>

#include <utils/io.hpp>

namespace zonetool::iw8::converter::iw7
{
	namespace
	{
		std::vector<std::string> csv_rows;
		std::vector<std::unique_ptr<loaded_zone>> transient_zones;
		std::unordered_set<std::string> loaded_transient_files;

		const char* find_transient_file(const std::string_view asset_name)
		{
			const char* file{};
			db_enum_assets(ASSET_TYPE_STREAMING_INFO, [&](void* header)
			{
				const auto info = static_cast<StreamingInfo*>(header)->transientInfo;
				if (file || !info || !info->transientAssets || !info->transientFiles)
				{
					return;
				}

				for (auto i = 0u; i < info->transientAssetCount; i++)
				{
					const auto& asset = info->transientAssets[i];
					if (asset.name && asset.name == asset_name && asset.mpParentIndex < info->transientFileCount)
					{
						file = info->transientFiles[asset.mpParentIndex].name;
						return;
					}
				}
			});

			return file;
		}

		XModel* find_loaded_model(const char* name)
		{
			const auto model = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, name));
			return model && model->numBones ? model : nullptr;
		}
	}

	XModel* find_transient_model(const char* name)
	{
		if (const auto model = find_loaded_model(name))
		{
			return model;
		}

		const auto file = find_transient_file(name);
		if (!file || !loaded_transient_files.emplace(file).second)
		{
			return nullptr;
		}

		const auto path = game_files::find_zone(file);
		if (!path)
		{
			ZONETOOL_WARNING("Transient zone \"%s\" of xmodel \"%s\" not found", file, name);
			return nullptr;
		}

		ZONETOOL_INFO("Loading transient zone \"%s\"...", file);
		transient_zones.emplace_back(load_zone(*path));
		return find_loaded_model(name);
	}

	void release_transient_zones()
	{
		transient_zones.clear();
		loaded_transient_files.clear();
	}

	void add_csv_row(const std::string_view type, const std::string_view name)
	{
		auto row = std::format("{},{}", type, name);
		if (std::ranges::find(csv_rows, row) == csv_rows.end())
		{
			csv_rows.emplace_back(std::move(row));
		}
	}

	bool mark_dumped(const XAssetType type, const std::string_view name)
	{
		static std::unordered_set<std::string> dumped;
		return dumped.emplace(std::format("{},{}", static_cast<int>(type), name)).second;
	}

	void write_csv(const std::string& zone_name)
	{
		if (csv_rows.empty())
		{
			return;
		}

		if (std::ranges::any_of(csv_rows, [](const std::string& row) { return row.starts_with("gfx_map,"); }))
		{
			netconststrings::dump_level_strings();
		}

		std::ranges::stable_partition(csv_rows, [](const std::string& row) { return row.starts_with("require,"); });

		std::string text;
		for (const auto& row : csv_rows)
		{
			text += row + "\n";
		}

		filesystem::file csv(zone_name + ".csv");
		csv.open("wb");
		csv.write(text);
		csv.close();
		csv_rows.clear();
	}
}

namespace zonetool::convert::iw8_iw7
{
	namespace
	{
		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw8;
			namespace target = zonetool::iw8::converter::iw7;

			switch (type)
			{
			case source::ASSET_TYPE_RAWFILE:
				target::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_IMAGE:
				target::gfximage::dump(header_as<source::GfxImage>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				target::material::dump(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_XMODEL_SURFS:
				target::xsurface::dump(header_as<source::XModelSurfs>(header));
				break;
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clipmap::dump(header_as<source::clipMap_t>(header));
				break;
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_LIGHT_DEF:
				target::lightdef::dump(header_as<source::GfxLightDef>(header));
				break;
			case source::ASSET_TYPE_GLASSWORLD:
				target::glassworld::dump(header_as<source::GlassWorld>(header));
				break;
			case source::ASSET_TYPE_GFXWORLD:
				target::gfxworld::dump(header_as<source::GfxWorld>(header));
				break;
			case source::ASSET_TYPE_XANIMPARTS:
				target::xanim::dump(header_as<source::XAnimParts>(header));
				break;
			case source::ASSET_TYPE_ANIM_PACKAGE:
				target::animpackage::dump(header_as<source::WeaponAnimPackage>(header));
				break;
			case source::ASSET_TYPE_WEAPON:
				target::weapon::dump(header_as<source::WeaponCompleteDef>(header));
				break;
			default:
				break;
			}
		}
	}

	namespace
	{
		void setup(const std::filesystem::path& target_game_path)
		{
			zonetool::iw8::converter::iw7::sound::set_iw7_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			zonetool::iw8::converter::iw7::write_csv(zone_name);
			zonetool::iw8::converter::iw7::release_transient_zones();
		}

		std::vector<std::string> required_zones(const std::string& zone_name)
		{
			std::vector<std::string> zones{"global_mp", "global_stream_mp", "common", "common_core_mp", "common_base_mp", "common_mp", "techsets_common",
				"techsets_common_mp"};
			if (!zone_name.starts_with("techsets_"))
			{
				zones.emplace_back("techsets_" + zone_name);
			}

			return zones;
		}
	}

	const converter& definition()
	{
		static const converter iw8_iw7
		{
			.source = "iw8",
			.target = "iw7",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
			.required_zones = required_zones,
		};

		return iw8_iw7;
	}
}
