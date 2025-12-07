#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "tonemapping.hpp"

#include "zonetool/h1/assets/stringtable.hpp"

#include "zonetool/iw6/zonetool.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tonemapping
		{
			namespace
			{
				struct lightset_entry
				{
					std::string name;
					std::string default_value;
					std::optional<std::function<std::string(ToneMapping*)>> convert = {};
				};

				std::string convert_name(ToneMapping* asset)
				{
					std::string name = asset->name;
					const auto idx = name.find("_tone");
					const auto base_name = name.substr(0, idx);
					return base_name;
				}

				std::vector<lightset_entry> lightset_values =
				{
					{"name", "", convert_name},

					{"sunenable", "1"},
					{"sunshadowscale", "1"},
					{"sunshadowscale_time", "1"},
					{"spotlimit", "4"},
					{"qualityspotshadow", "1"},
					{"usedsuncascadecount", "3"},
					{"sunsamplesizenear", "0.25"},
					{"sunsamplesizenear_time", "0"},
					{"sunfilterradius", "0.2"},
					{"sunfilterradius_time", "0"},
					{"spotfilterradius", "1.5"},
					{"spotfilterradius_time", "0"},

					{"specularcolorscale", "2"},
					{"specularcolorscale_time", "0"},
					{"diffusecolorscale", "1"},
					{"diffusecolorscale_time", "0"},

					{"veil", "1"},
					{"veilstrength", "0.175"},
					{"veilstrength_time", "0"},
					{"veilbackgroundstrength", "0.9"},
					{"veilbackgroundstrength_time", "0"},

					{"tonemap", "2"},
					{"tonemapauto", "1"},
					{"tonemapblend", "1"},
					{"tonemaplockautoexposureadjust", "0"},
					{"tonemapautoexposureadjust", "0"},
					{"tonemapautoexposureadjust_time", "0"},
					{"tonemapexposure", "10"}, // convert?
					{"tonemapexposure_time", "0"},
					{"tonemapexposureadjust", "0"},
					{"tonemapexposureadjust_time", "0"},
					{"tonemapmaxexposure", "10"},
					{"tonemapmaxexposure_time", "0"},
					{"tonemapadaptspeed", "0.02"},
					{"tonemapadaptspeed_time", "0"},
					{"tonemapdarkev", "2.84"},
					{"tonemapdarkev_time", "0"},
					{"tonemapmidev", "7.823"},
					{"tonemapmidev_time", "0"},
					{"tonemaplightev", "12.81"},
					{"tonemaplightev_time", "0"},
					{"tonemapdarkexposureadjust", "-3.17"},
					{"tonemapdarkexposureadjust_time", "0"},
					{"tonemapmidexposureadjust", "-0.0651"},
					{"tonemapmidexposureadjust_time", "0"},
					{"tonemaplightexposureadjust", "1.47"},
					{"tonemaplightexposureadjust_time", "0"},
					{"tonemapminexposureadjust", "-3.17"},
					{"tonemapminexposureadjust_time", "0"},
					{"tonemapmaxexposureadjust", "2.3"},
					{"tonemapmaxexposureadjust_time", "0"},
					{"tonemapwhite", "512"}, // convert?
					{"tonemapwhite_time", "0"},
					{"tonemapshoulder", "0.96"}, // convert?
					{"tonemapshoulder_time", "0"},
					{"tonemapcrossover", "3"},
					{"tonemapcrossover_time", "0"},
					{"tonemaptoe", "0.3"}, // convert?
					{"tonemaptoe_time", "0"},
					{"tonemapblack", "0.01"},
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

					{"mdao", "1"},
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
				};
			}

			zonetool::h1::StringTable* convert(ToneMapping* asset, utils::memory::allocator& allocator)
			{
				const auto& fastfile = filesystem::get_fastfile();
				const auto new_asset = allocator.allocate<zonetool::h1::StringTable>();
				new_asset->name = allocator.duplicate_string(utils::string::va("maps/createart/%s_lightsets_hdr.csv", fastfile.data()));

				std::set<ToneMapping*> assets;
				assets.insert(asset);
				db_enum_assets(ASSET_TYPE_TONEMAPPING, [&](XAssetHeader header)
				{
					const std::string name = header.toneMapping->name;
					if (name.contains(fastfile))
					{
						assets.insert(header.toneMapping);
					}
				}, true);

				new_asset->columnCount = 1 + static_cast<int>(assets.size());
				new_asset->rowCount = static_cast<int>(lightset_values.size());
				new_asset->values = allocator.allocate_array<zonetool::h1::StringTableCell>(new_asset->columnCount * new_asset->rowCount);

				const auto insert_value = [&](const int column, const int row, const std::string& value)
				{
					const auto index = (row * new_asset->columnCount) + column;
					new_asset->values[index].string = allocator.duplicate_string(value.data());
					new_asset->values[index].hash = string_table_hash(new_asset->values[index].string);
				};

				for (auto i = 0u; i < lightset_values.size(); i++)
				{
					insert_value(0, i, lightset_values[i].name);
				}

				auto i = 0u;
				for (const auto& asset_ : assets)
				{
					for (auto o = 0u; o < lightset_values.size(); o++)
					{
						const auto& entry = lightset_values[o];
						if (entry.convert.has_value())
						{
							insert_value(i + 1, o, entry.convert->operator()(asset_));
						}
						else
						{
							insert_value(i + 1, o, entry.default_value);
						}
					}

					++i;
				}

				return new_asset;
			}

			void dump(ToneMapping* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::string_table::dump(converted_asset);
			}
		}
	}
}
