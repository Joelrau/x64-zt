#include <std_include.hpp>

#include "../include.hpp"
#include "soundtables.hpp"

namespace zonetool::h1::converter::s2::soundtables
{
	namespace
	{
		constexpr auto s2_column_count = 10;

		constexpr std::array<std::string_view, 10> s2_headers
		{
			"zone_names,ambience,ambient_name,mix,reverb,filter,occlusion,player_adsr",
			"filter_names,dsp_bus_name,type,freq,gain,q",
			"reverb_names,roomtype,drylevel,wetlevel,fade",
			"occlusion_names,dsp_bus_name,freq,type,gain,q",
			"dynamic_component,alias,first_event_time_min,first_event_time_max,event_time_min,event_time_max,range_min,range_max,cone_min,cone_max",
			"dynamic_ambience_names,components",
			"adsr_zone_player,weapon,adsr",
			"adsr_zone_npc,weapon,adsr",
			"adsr_name,att_len,dec_len,sus_len,sus_level,rel_len",
			"mix_names,volmod,value,fadein,fadeout",
		};

		std::string make_row(std::vector<std::string> cells)
		{
			while (cells.size() > 1 && cells.back().empty())
			{
				cells.pop_back();
			}

			cells.resize(std::max(cells.size(), static_cast<std::size_t>(s2_column_count)));

			auto line = cells.front();
			for (auto i = 1u; i < cells.size(); i++)
			{
				line += "," + cells[i];
			}

			return line + "\n";
		}

		std::optional<std::string_view> find_header(const std::string_view name)
		{
			for (const auto& header : s2_headers)
			{
				if (header.substr(0, header.find(',')) == name)
				{
					return header;
				}
			}

			return {};
		}

		bool is_header_column(const std::string_view name)
		{
			// h1 tables can hold broken header rows ("sus_len,sus_level" on its own)
			return std::ranges::any_of(s2_headers, [&](const std::string_view header)
			{
				const auto columns = utils::string::split(std::string(header), ',');
				return std::find(columns.begin() + 1, columns.end(), name) != columns.end();
			});
		}

		struct map_reverb_preset
		{
			const char* name;
			std::array<float, 24> values;
		};

		constexpr std::array<map_reverb_preset, 6> map_reverb_presets // the other room types are global s2 presets
		{{
			{"alleyway_buildings", {74.0f, 0.78f, 0.54f, 0.05f, 23.0f, 0.0f, 0.79f, 0.56f,
				-1.099998f, 0.07f, 0.72f, 11.4f, 0.69f, 83.0f, 0.07f, 0.55f,
				-120.0f, 0.6f, 0.72f, 11.4f, 0.69f, 23.0f, 10000.0f, 6.0f}},
			{"close_buildings", {275.0f, 0.88f, 0.04f, 0.05f, 0.0f, -21.0f, 0.88f, 0.03f,
				-24.0f, 0.09999999f, 0.72f, 15.0f, 0.69f, 470.0f, 0.88f, 0.25f,
				-24.0f, 0.26f, 0.72f, 14.0f, 0.69f, 480.0f, 800.0f, 12.0f}},
			{"large_size_vibrant", {8.0f, 0.57f, 0.9f, 0.63f, 22.0f, -14.0f, 0.62f, 0.5049505f,
				-9.0f, 0.33f, 0.39f, 5.4f, 0.48f, 62.0f, 0.61f, 0.68f,
				-9.0f, 0.17f, 0.31f, 6.0f, 0.48f, 64.0f, 800.0f, 0.0f}},
			{"med_size_vibrant", {8.0f, 0.57f, 0.9f, 0.63f, 22.0f, -49.0f, 0.62f, 0.63f,
				-11.0f, 0.03f, 0.33f, 5.1f, 0.48f, 34.0f, 0.65f, 0.64f,
				-7.0f, 0.02f, 0.61f, 5.7f, 0.5f, 37.0f, 800.0f, 4.0f}},
			{"semi_open_large", {33.0f, 0.95f, 0.69f, 0.61f, 12.0f, 1.0f, 0.88f, 0.75f,
				8.0f, 0.11f, 0.7f, 5.4f, 0.55f, 21.0f, 0.07f, 0.12f,
				6.0f, 0.38f, 0.03f, 12.75f, 0.45f, 0.0f, 800.0f, 3.800003f}},
			{"semi_open_small", {10.0f, 0.95f, 0.78f, 0.49f, 3.0f, -1.0f, 0.84f, 0.32f,
				-7.5f, 0.04f, 0.72f, 6.0f, 0.69f, 230.0f, 0.86f, 0.09f,
				-120.0f, 0.07f, 0.43f, 18.0f, 0.66f, 365.0f, 10000.0f, 5.0f}},
		}};

		std::string convert_room_type(const std::string& name)
		{
			return name == "church" ? "hall_marble" : name; // s2 has no church preset, a marble hall is the closest global one
		}

		void dump_reverb_preset(const std::string& name)
		{
			const auto entry = std::ranges::find_if(map_reverb_presets, [&](const map_reverb_preset& preset)
			{
				return name == preset.name;
			});
			if (entry == map_reverb_presets.end())
			{
				return;
			}

			zonetool::s2::ReverbPreset preset{};
			preset.name = entry->name;
			std::ranges::copy(entry->values, preset.unk_0x8);
			zonetool::s2::reverb_preset::dump(&preset);
			add_csv_row("reverbpreset", name);
		}
	}

	void dump(StringTable* asset)
	{
		if (!std::string_view(asset->name).starts_with("soundtables/"))
		{
			return;
		}

		std::string buffer;
		std::string section;
		auto comment_column = false;
		for (auto row = 0; row < asset->rowCount; row++)
		{
			const std::string name = table_cell(asset, row, 0);
			if (const auto header = find_header(name))
			{
				section = name;
				comment_column = asset->columnCount > 1 && std::string_view(table_cell(asset, row, 1)).starts_with("comment");
				buffer += make_row(utils::string::split(std::string(*header), ','));
				continue;
			}

			if (name.empty() || section.empty() || is_header_column(name))
			{
				continue;
			}

			std::vector<std::string> cells{name};
			for (auto column = comment_column ? 2 : 1; column < asset->columnCount; column++)
			{
				cells.emplace_back(table_cell(asset, row, column));
			}

			if (section == "dynamic_ambience_names" && cells.size() > 1)
			{
				std::erase(cells[1], ' '); // s2 joins the components with a bare "+"
			}
			else if (section == "reverb_names" && cells.size() > 1)
			{
				cells[1] = convert_room_type(cells[1]);
				dump_reverb_preset(cells[1]);
			}

			buffer += make_row(cells);
		}

		write_file(asset->name, buffer);

		add_csv_row("stringtable", asset->name);
	}
}
