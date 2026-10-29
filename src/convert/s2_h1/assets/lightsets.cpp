#include <std_include.hpp>

#include "../include.hpp"
#include "lightsets.hpp"

namespace zonetool::s2::converter::h1::lightsets
{
	namespace
	{
		struct h1_row
		{
			std::string_view name;
			std::string_view value;
		};

		constexpr std::array<h1_row, 103> h1_rows
		{{
			{"sunenable", "1"},
			{"sunshadowscale", "1"},
			{"sunshadowscale_time", "0"},
			{"spotlimit", "4"},
			{"qualityspotshadow", "0"},
			{"usedsuncascadecount", "2"},
			{"sunsamplesizenear", "0.45"},
			{"sunsamplesizenear_time", "0"},
			{"sunfilterradius", "2"},
			{"sunfilterradius_time", "0"},
			{"spotfilterradius", "2"},
			{"spotfilterradius_time", "0"},
			{"specularcolorscale", "1"},
			{"specularcolorscale_time", "0"},
			{"diffusecolorscale", "1"},
			{"diffusecolorscale_time", "0"},
			{"veil", "1"},
			{"veilstrength", "0.25"},
			{"veilstrength_time", "0.5"},
			{"veilbackgroundstrength", "0.913"},
			{"veilbackgroundstrength_time", "0"},
			{"tonemap", "2"},
			{"tonemapauto", "1"},
			{"tonemapblend", "1"},
			{"tonemaplockautoexposureadjust", "0"},
			{"tonemapautoexposureadjust", "0"},
			{"tonemapautoexposureadjust_time", "0"},
			{"tonemapexposure", "-10"},
			{"tonemapexposure_time", "0"},
			{"tonemapexposureadjust", "0"},
			{"tonemapexposureadjust_time", "0"},
			{"tonemapmaxexposure", "-10"},
			{"tonemapmaxexposure_time", "0"},
			{"tonemapadaptspeed", "0.02"},
			{"tonemapadaptspeed_time", "0"},
			{"tonemapdarkev", "7.99214"},
			{"tonemapdarkev_time", "0.5"},
			{"tonemapmidev", "11.6215"},
			{"tonemapmidev_time", "0.5"},
			{"tonemaplightev", "15.0852"},
			{"tonemaplightev_time", "0.5"},
			{"tonemapdarkexposureadjust", "-1.78751"},
			{"tonemapdarkexposureadjust_time", "0.5"},
			{"tonemapmidexposureadjust", "-0.214723"},
			{"tonemapmidexposureadjust_time", "0.5"},
			{"tonemaplightexposureadjust", "1.47003"},
			{"tonemaplightexposureadjust_time", "0.5"},
			{"tonemapminexposureadjust", "-3.1699"},
			{"tonemapminexposureadjust_time", "0.5"},
			{"tonemapmaxexposureadjust", "2.3219"},
			{"tonemapmaxexposureadjust_time", "0.5"},
			{"tonemapwhite", "512"},
			{"tonemapwhite_time", "0"},
			{"tonemapshoulder", "0.96"},
			{"tonemapshoulder_time", "0.5"},
			{"tonemapcrossover", "3"},
			{"tonemapcrossover_time", "0"},
			{"tonemaptoe", "0.25"},
			{"tonemaptoe_time", "0"},
			{"tonemapblack", "0"},
			{"tonemapblack_time", "0"},
			{"aodiminish", "1"},
			{"aodiminish_time", "0"},
			{"ssaostrength", "0.45"},
			{"ssaostrength_time", "0"},
			{"ssaopower", "12"},
			{"ssaopower_time", "0"},
			{"ssaominstrengthdepth", "25"},
			{"ssaominstrengthdepth_time", "0"},
			{"ssaomaxstrengthdepth", "40"},
			{"ssaomaxstrengthdepth_time", "0"},
			{"hemiaostrength", "0.5"},
			{"hemiaostrength_time", "0"},
			{"hemiaopower", "1"},
			{"hemiaopower_time", "0"},
			{"hemiaorejectionfalloff", "0.5"},
			{"hemiaorejectionfalloff_time", "0"},
			{"mdao", "0"},
			{"mdaooccluderculldistance", "1000"},
			{"mdaooccluderculldistance_time", "0"},
			{"mdaooccluderfadeoutstartdistance", "900"},
			{"mdaooccluderfadeoutstartdistance_time", "0"},
			{"mdaocapsulestrength", "1"},
			{"mdaocapsulestrength_time", "0"},
			{"mdaovolumestrength", "1"},
			{"mdaovolumestrength_time", "0"},
			{"ssaowidth", "36"},
			{"ssaowidth_time", "0"},
			{"ssaogapfalloff", "0.25"},
			{"ssaogapfalloff_time", "0"},
			{"ssaogradientfalloff", "0.05"},
			{"ssaogradientfalloff_time", "0"},
			{"tessellationfactor", "60"},
			{"tessellationfactor_time", "0"},
			{"tessellationcutoffdistance", "960"},
			{"tessellationcutoffdistance_time", "0"},
			{"tessellationcutofffalloff", "320"},
			{"tessellationcutofffalloff_time", "0"},
			{"ssaomode", "1"},
			{"ssaofadedepth", "2500"},
			{"ssaofadedepth_time", "0"},
			{"ssaorejectdepth", "2500"},
			{"ssaorejectdepth_time", "0"},
		}};

		bool uses_h1_tonemap_curve(const std::string_view row)
		{
			return row == "tonemap" || row == "tonemapcrossover";
		}

		const char* cell(const zonetool::s2::StringTable* table, const int row, const int column)
		{
			const auto value = table->values[row * table->columnCount + column].string;
			return value ? value : "";
		}

		int find_row(const zonetool::s2::StringTable* table, const std::string_view name)
		{
			for (auto row = 1; row < table->rowCount; row++)
			{
				if (name == cell(table, row, 0))
				{
					return row;
				}
			}

			return -1;
		}
	}

	void dump(zonetool::s2::StringTable* asset)
	{
		constexpr auto map_column = 1;
		if (!std::string_view(asset->name).ends_with("_lightsets_hdr.csv") || asset->rowCount < 1 || asset->columnCount <= map_column)
		{
			return;
		}

		auto buffer = std::format("{},{}\n", cell(asset, 0, 0), cell(asset, 0, map_column));
		for (const auto& row : h1_rows)
		{
			const auto source_row = uses_h1_tonemap_curve(row.name) ? -1 : find_row(asset, row.name);
			buffer.append(std::format("{},{}\n", row.name, source_row < 0 ? row.value : cell(asset, source_row, map_column)));
		}

		filesystem::file file(asset->name);
		file.open("wb");
		file.write(buffer);
		file.close();

		add_csv_row("stringtable", asset->name);
	}
}
