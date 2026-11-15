#include <std_include.hpp>
#include "converter.hpp"

#include <game.hpp>

#include <utils/io.hpp>

namespace zonetool::iw8::converter::iw7
{
	namespace
	{
		std::vector<std::string> csv_rows;
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
			default:
				break;
			}
		}
	}

	namespace
	{
		void finish(const std::string& zone_name)
		{
			zonetool::iw8::converter::iw7::write_csv(zone_name);
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
			.finish = finish,
			.required_zones = required_zones,
		};

		return iw8_iw7;
	}
}
