#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

namespace zonetool::iw7::converter::iw8
{
	namespace
	{
		std::vector<std::string> csv_rows;
		std::filesystem::path target_folder;
	}

	const std::filesystem::path& iw8_folder()
	{
		if (target_folder.empty())
		{
			throw std::runtime_error("IW8 game folder is not set");
		}

		return target_folder;
	}

	void set_iw8_folder(const std::filesystem::path& folder)
	{
		target_folder = folder;
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

		std::ranges::stable_sort(csv_rows, {}, [](const std::string& row)
		{
			return row.starts_with("require,") ? 0 : row.contains(",,") ? 1 : 2;
		});

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

namespace zonetool::convert::iw7_iw8
{
	namespace
	{
		std::vector<zonetool::iw7::Material*> zone_materials;

		template <typename S>
		S* header_as(void* header)
		{
			return static_cast<S*>(header);
		}

		void dump_asset(const std::int32_t type, void* header)
		{
			namespace source = zonetool::iw7;
			namespace target = zonetool::iw7::converter::iw8;

			switch (type)
			{
			case source::ASSET_TYPE_XMODEL:
				target::xmodel::dump(header_as<source::XModel>(header));
				break;
			case source::ASSET_TYPE_MATERIAL:
				zone_materials.push_back(header_as<source::Material>(header));
				break;
			case source::ASSET_TYPE_TRACER:
				target::tracer::dump(header_as<source::TracerDef>(header));
				break;
			case source::ASSET_TYPE_VFX:
				target::vfx::dump(header_as<source::ParticleSystemDef>(header));
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
			case source::ASSET_TYPE_COMWORLD:
				target::comworld::dump(header_as<source::ComWorld>(header));
				break;
			case source::ASSET_TYPE_MAP_ENTS:
				target::mapents::dump(header_as<source::MapEnts>(header));
				break;
			case source::ASSET_TYPE_CLIPMAP:
				target::clipmap::dump(header_as<source::clipMap_t>(header));
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
			case source::ASSET_TYPE_FXWORLD:
				target::fxworld::dump(header_as<source::FxWorld>(header));
				break;
			case source::ASSET_TYPE_IMPACT_FX:
				target::impactfx::dump(header_as<source::FxImpactTable>(header));
				break;
			case source::ASSET_TYPE_SCRIPTABLE:
				target::scriptable::dump(header_as<source::ScriptableDef>(header));
				break;
			case source::ASSET_TYPE_SOUND_BANK:
				target::map_sound::dump(header_as<source::SndBank>(header));
				break;
			case source::ASSET_TYPE_RAWFILE:
				target::rawfile::dump(header_as<source::RawFile>(header));
				break;
			case source::ASSET_TYPE_PATHDATA:
				target::pathdata::dump(header_as<source::PathData>(header));
				break;
			case source::ASSET_TYPE_NAVMESH:
				target::navmesh::dump(header_as<source::NavMeshData>(header));
				break;
			case source::ASSET_TYPE_NET_CONST_STRINGS:
				target::netconststrings::dump(header_as<source::NetConstStrings>(header));
				break;
			default:
				break;
			}
		}

		void setup(const std::filesystem::path& target_game_path)
		{
			zonetool::iw7::converter::iw8::set_iw8_folder(target_game_path);
		}

		void finish(const std::string& zone_name)
		{
			for (const auto material : zone_materials)
			{
				zonetool::iw7::converter::iw8::material::dump(material);
			}
			zone_materials.clear();

			zonetool::iw7::converter::iw8::netconststrings::dump_missing_level_strings();
			zonetool::iw7::converter::iw8::material::dump_donor_techsets();
			zonetool::iw7::converter::iw8::sound::dump_weapon_bank(zone_name);
			zonetool::iw7::converter::iw8::write_csv(zone_name);
		}

		std::vector<std::string> required_zones(const std::string& zone_name)
		{
			std::vector<std::string> zones{"code_post_gfx", "global_core_mp", "techsets_global_core_mp", "techsets_common", "common", "common_core_mp", "techsets_common_core_mp", "common_mp"};
			if (!zone_name.starts_with("techsets_"))
			{
				zones.emplace_back("techsets_" + zone_name);
			}

			if (zone_name.starts_with("mp_") && !zone_name.ends_with("_load"))
			{
				zones.emplace_back(zone_name + "_load");
			}

			return zones;
		}
	}

	const converter& definition()
	{
		static const converter iw7_iw8
		{
			.source = "iw7",
			.target = "iw8",
			.dump_asset = dump_asset,
			.setup = setup,
			.finish = finish,
			.required_zones = required_zones,
		};

		return iw7_iw8;
	}
}
