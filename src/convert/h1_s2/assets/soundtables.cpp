#include <std_include.hpp>

#include "../include.hpp"
#include "soundtables.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::soundtables
{
	namespace
	{
		constexpr auto s2_column_count = 10;

		// section header rows of the stock s2 soundtables (soundtables/mp_aachen_v2.csv, soundtables/mp_defaults.csv)
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

		// h1 tables can hold broken header rows ("sus_len,sus_level" on its own), those name a column of some s2 header
		bool is_header_column(const std::string_view name)
		{
			return std::ranges::any_of(s2_headers, [&](const std::string_view header)
			{
				const auto columns = utils::string::split(std::string(header), ',');
				return std::find(columns.begin() + 1, columns.end(), name) != columns.end();
			});
		}

		// s2 maps carry the reverb presets that are not in code_post_gfx_mp, copy them from the stock donor map
		void dump_reverb_preset(const std::string& name)
		{
			for (auto* header : techset::donor_zone().find_assets(zonetool::s2::ASSET_TYPE_REVERB_PRESET))
			{
				const auto preset = static_cast<zonetool::s2::ReverbPreset*>(header);
				if (preset->name && name == preset->name)
				{
					zonetool::s2::reverb_preset::dump(preset);
					add_csv_row("reverbpreset", name);
					return;
				}
			}
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
				// h1 has a comments column after the name in some sections and calls the ambience column streamed_ambience
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
				dump_reverb_preset(cells[1]);
			}

			buffer += make_row(cells);
		}

		write_file(asset->name, buffer);

		add_csv_row("stringtable", asset->name);
	}
}
