#include <std_include.hpp>

#include "../include.hpp"
#include "sound.hpp"
#include "soundtables.hpp"

namespace zonetool::s2::converter::h1::soundtables
{
	namespace
	{
		using row_t = std::vector<std::string>;
		using sections_t = std::map<std::string, std::vector<row_t>>;

		constexpr std::array<std::string_view, 10> s2_sections
		{
			"zone_names", "filter_names", "reverb_names", "occlusion_names", "dynamic_component", "dynamic_ambience_names",
			"adsr_zone_player", "adsr_zone_npc", "adsr_name", "mix_names",
		};

		constexpr std::array<zonetool::h1::ReverbPreset, 1> map_reverb_presets
		{{
			{{"close_buildings"}, 0.472f, 0.694f, 1.259f, 0.741f, 1.0f, 0.9f, 0.648f, 0.7f, 0.8f, 0.491f, 0.5f, 0.0f, 10.111f, 2.3f, 0.5f, 1.0f},
		}};

		const std::unordered_map<std::string_view, std::string_view> s2_dsp_bus_renames
		{
			{"unocclu_no_timescale", "notimescale"},
		};

		const char* table_cell(const StringTable* table, const int row, const int column)
		{
			const auto value = table->values[row * table->columnCount + column].string;
			return value ? value : "";
		}

		std::string cell(const row_t& row, const std::size_t column)
		{
			return column < row.size() ? row[column] : std::string{};
		}

		std::string make_row(row_t cells)
		{
			while (cells.size() > 1 && cells.back().empty())
			{
				cells.pop_back();
			}

			cells.resize(std::max(cells.size(), static_cast<std::size_t>(11u)));
			auto line = cells.front();
			for (auto i = 1u; i < cells.size(); i++)
			{
				line += "," + cells[i];
			}

			return line + "\n";
		}

		sections_t read_sections(const StringTable* asset)
		{
			sections_t sections;
			std::string section;
			for (auto row = 0; row < asset->rowCount; row++)
			{
				const std::string name = table_cell(asset, row, 0);
				if (std::ranges::find(s2_sections, name) != s2_sections.end())
				{
					section = name;
					continue;
				}

				if (name.empty() || section.empty())
				{
					continue;
				}

				row_t cells;
				for (auto column = 0; column < asset->columnCount; column++)
				{
					cells.emplace_back(table_cell(asset, row, column));
				}

				sections[section].emplace_back(std::move(cells));
			}

			return sections;
		}

		std::optional<std::string> convert_dsp_bus(const std::string& s2_name)
		{
			std::string name = s2_name;
			if (const auto rename = s2_dsp_bus_renames.find(name); rename != s2_dsp_bus_renames.end())
			{
				name = rename->second;
			}

			if (zonetool::h1::sound::get_dsp_bus_index_from_name(name.data()) < 0)
			{
				ZONETOOL_WARNING("soundtables: dsp bus \"%s\" has no H1 bus", s2_name.data());
				return {};
			}

			return name;
		}

		void dump_reverb_preset(const std::string& name)
		{
			const auto entry = std::ranges::find_if(map_reverb_presets, [&](const zonetool::h1::ReverbPreset& preset)
			{
				return name == preset.name;
			});
			if (entry == map_reverb_presets.end())
			{
				return;
			}

			auto preset = *entry;
			zonetool::h1::reverb_preset::dump(&preset);
			add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_REVERB_PRESET), name);
		}

		std::vector<row_t> convert_zones(const std::vector<row_t>& rows)
		{
			std::vector<row_t> zones;
			for (const auto& row : rows)
			{
				sound::dump_used(cell(row, 1));
				zones.push_back({cell(row, 0), "", cell(row, 1), cell(row, 2), cell(row, 3), cell(row, 4), cell(row, 5), cell(row, 6), cell(row, 7)});
			}

			return zones;
		}

		std::vector<row_t> convert_filters(const std::vector<row_t>& rows)
		{
			std::vector<row_t> filters;
			for (const auto& row : rows)
			{
				if (const auto bus = convert_dsp_bus(cell(row, 1)))
				{
					filters.push_back({cell(row, 0), *bus, cell(row, 2), cell(row, 3), cell(row, 4), cell(row, 5)});
				}
			}

			return filters;
		}

		std::vector<row_t> convert_reverbs(const std::vector<row_t>& rows)
		{
			for (const auto& row : rows)
			{
				dump_reverb_preset(cell(row, 1));
			}

			return rows;
		}

		std::vector<row_t> convert_components(const std::vector<row_t>& rows)
		{
			std::vector<row_t> components;
			for (const auto& row : rows)
			{
				sound::dump_used(cell(row, 1));

				row_t component{cell(row, 0), ""};
				component.insert(component.end(), row.begin() + 1, row.end());
				components.push_back(std::move(component));
			}

			return components;
		}

		std::vector<row_t> convert_ambiences(const std::vector<row_t>& rows)
		{
			std::vector<row_t> ambiences;
			for (const auto& row : rows)
			{
				std::string names;
				for (const auto& name : utils::string::split(cell(row, 1), '+'))
				{
					names += (names.empty() ? "" : " + ") + name;
				}

				ambiences.push_back({cell(row, 0), names});
			}

			return ambiences;
		}

		std::vector<row_t> convert_mixes(const std::vector<row_t>& rows)
		{
			std::vector<row_t> mixes;
			for (const auto& row : rows)
			{
				if (zonetool::h1::sound::get_vol_mod_index_from_name(cell(row, 1).data()) < 0)
				{
					ZONETOOL_WARNING("soundtables: volume mod \"%s\" has no H1 group", cell(row, 1).data());
					continue;
				}

				mixes.push_back({cell(row, 0), cell(row, 1), cell(row, 2), cell(row, 3), cell(row, 4)});
			}

			return mixes;
		}
	}

	void dump(StringTable* asset)
	{
		if (!std::string_view(asset->name).starts_with("soundtables/"))
		{
			return;
		}

		auto sections = read_sections(asset);
		const auto zones = convert_zones(sections["zone_names"]);
		const auto filters = convert_filters(sections["filter_names"]);
		const auto reverbs = convert_reverbs(sections["reverb_names"]);
		const auto components = convert_components(sections["dynamic_component"]);
		const auto ambiences = convert_ambiences(sections["dynamic_ambience_names"]);
		const auto mixes = convert_mixes(sections["mix_names"]);

		const std::array<std::pair<std::string_view, const std::vector<row_t>*>, 9> h1_sections
		{{
			{"zone_names,comments,streamed_ambience,ambient_name,mix,reverb,filter,occlusion,player_adsr", &zones},
			{"filter_names,dsp_bus_name,type,freq,gain,q", &filters},
			{"reverb_names,roomtype,drylevel,wetlevel,fade", &reverbs},
			{"dynamic_component,comment,alias,first_event_time_min,first_event_time_max,event_time_min,event_time_max,range_min,range_max,cone_min,cone_max", &components},
			{"dynamic_ambience_names,components", &ambiences},
			{"adsr_zone_player,weapon,adsr", &sections["adsr_zone_player"]},
			{"adsr_zone_npc,weapon,adsr", &sections["adsr_zone_npc"]},
			{"adsr_name,att_len,dec_len,sus_len,sus_level,rel_len", &sections["adsr_name"]},
			{"mix_names,volmod,value,fadein,fadeout", &mixes},
		}};

		std::string buffer;
		for (const auto& [header, rows] : h1_sections)
		{
			buffer += make_row(utils::string::split(std::string(header), ','));
			for (const auto& row : *rows)
			{
				buffer += make_row(row);
			}
		}

		write_file(asset->name, buffer);
		add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_STRINGTABLE), asset->name);
	}
}
