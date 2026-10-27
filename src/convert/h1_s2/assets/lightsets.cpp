#include <std_include.hpp>

#include "../include.hpp"
#include "lightsets.hpp"

namespace zonetool::h1::converter::s2::lightsets
{
	namespace
	{
		struct s2_row
		{
			std::string_view name;
			std::string_view h1_name;
			std::string_view fallback;
		};

		constexpr std::array<s2_row, 92> s2_rows // s2 matches the rows by name, a missing row keeps the dvar value
		{{
			{"sunenable"},
			{"sunshadowscale"},
			{"sunshadowscale_time"},
			{"spotupdatelimit", "spotlimit"},
			{"spotdynamics", {}, "4"},
			{"usedsuncascadecount"},
			{"usedsuncascadecount_durango", "usedsuncascadecount"},
			{"usedsuncascadecountsceneent", "usedsuncascadecount"},
			{"sunsamplesizenear"},
			{"sunsamplesizenear_time"},
			{"sunfilterradius"},
			{"sunfilterradius_time"},
			{"spotfilterradius"},
			{"spotfilterradius_time"},
			{"veil"},
			{"veilstrength"},
			{"veilstrength_time"},
			{"veilbackgroundstrength"},
			{"veilbackgroundstrength_time"},
			{"tonemap"},
			{"tonemapauto"},
			{"tonemapblend"},
			{"tonemaplockautoexposureadjust"},
			{"tonemapautoexposureadjust"},
			{"tonemapautoexposureadjust_time"},
			{"tonemapexposure"},
			{"tonemapexposure_time"},
			{"tonemapexposureadjust"},
			{"tonemapexposureadjust_time"},
			{"tonemapmaxexposure"},
			{"tonemapmaxexposure_time"},
			{"tonemapadaptspeed"},
			{"tonemapadaptspeed_time"},
			{"tonemapdarkev"},
			{"tonemapdarkev_time"},
			{"tonemapmidev"},
			{"tonemapmidev_time"},
			{"tonemaplightev"},
			{"tonemaplightev_time"},
			{"tonemapdarkexposureadjust"},
			{"tonemapdarkexposureadjust_time"},
			{"tonemapmidexposureadjust"},
			{"tonemapmidexposureadjust_time"},
			{"tonemaplightexposureadjust"},
			{"tonemaplightexposureadjust_time"},
			{"tonemapminexposureadjust"},
			{"tonemapminexposureadjust_time"},
			{"tonemapmaxexposureadjust"},
			{"tonemapmaxexposureadjust_time"},
			{"tonemapwhite"},
			{"tonemapwhite_time"},
			{"tonemapshoulder"},
			{"tonemapshoulder_time"},
			{"tonemapcrossover"},
			{"tonemapcrossover_time"},
			{"tonemaptoe"},
			{"tonemaptoe_time"},
			{"tonemapblack"},
			{"tonemapblack_time"},
			{"aodiminish"},
			{"aodiminish_time"},
			{"ssaostrength"},
			{"ssaostrength_time"},
			{"ssaopower"},
			{"ssaopower_time"},
			{"ssaominstrengthdepth"},
			{"ssaominstrengthdepth_time"},
			{"ssaomaxstrengthdepth"},
			{"ssaomaxstrengthdepth_time"},
			{"ssaoblurradius"},
			{"ssaoblurradius_time"},
			{"hemiaostrength"},
			{"hemiaostrength_time"},
			{"hemiaopower"},
			{"hemiaopower_time"},
			{"hemiaorejectionfalloff"},
			{"hemiaorejectionfalloff_time"},
			{"mdao"},
			{"mdaooccluderculldistance"},
			{"mdaooccluderculldistance_time"},
			{"mdaooccluderfadeoutstartdistance"},
			{"mdaooccluderfadeoutstartdistance_time"},
			{"mdaocapsulestrength"},
			{"mdaocapsulestrength_time"},
			{"mdaovolumestrength"},
			{"mdaovolumestrength_time"},
			{"wbColorTempK", {}, "6500"},
			{"wbColorTempK_time"},
			{"wbMethod", {}, "0"},
			{"wbIlluminant", {}, "0"},
			{"lgtemporalsmoothingfactor", {}, "0.9"},
			{"lgtemporalsmoothingfactor_time"},
		}};

		int find_row(const StringTable* table, const std::string_view name)
		{
			const auto wanted = utils::string::to_lower(std::string(name));
			for (auto row = 1; row < table->rowCount; row++)
			{
				if (utils::string::to_lower(table_cell(table, row, 0)) == wanted)
				{
					return row;
				}
			}

			return -1;
		}

		std::string make_row(const StringTable* table, const std::string_view name, const std::function<std::string(int)>& value)
		{
			auto line = std::string(name);
			for (auto column = 1; column < table->columnCount; column++)
			{
				line += "," + value(column);
			}

			return line + "\n";
		}
	}

	void dump(StringTable* asset)
	{
		if (!std::string_view(asset->name).ends_with("_lightsets_hdr.csv") || asset->rowCount < 1 || asset->columnCount < 2)
		{
			return;
		}

		auto buffer = make_row(asset, table_cell(asset, 0, 0), [&](const int column) { return std::string(table_cell(asset, 0, column)); });
		for (const auto& row : s2_rows)
		{
			const auto source = find_row(asset, row.h1_name.empty() ? row.name : row.h1_name);
			if (source >= 0)
			{
				buffer += make_row(asset, row.name, [&](const int column) { return std::string(table_cell(asset, source, column)); });
			}
			else if (row.name.ends_with("_time") || !row.fallback.empty())
			{
				buffer += make_row(asset, row.name, [&](int) { return std::string(row.name.ends_with("_time") ? "0" : row.fallback); });
			}
		}

		write_file(asset->name, buffer);

		add_csv_row("stringtable", asset->name);
	}
}
