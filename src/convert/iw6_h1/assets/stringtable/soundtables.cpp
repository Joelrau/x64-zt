#include <std_include.hpp>
#include "../../include.hpp"
#include "../sound.hpp"
#include "soundtables.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace stringtable::soundtables
		{
			namespace
			{
				constexpr std::array<std::string_view, 11> iw6_sections
				{
					"zone_name", "filter_name", "reverb_name", "occlusion_name", "mix_name", "ambient_name", "ambient_event",
					"ambient_element", "adsr_zone_player", "adsr_zone_npc", "adsr_name",
				};

				using row_t = std::vector<std::string>;
				using sections_t = std::map<std::string, std::vector<row_t>>;

				const char* table_cell(const StringTable* table, const int row, const int column)
				{
					const auto value = table->values[row * table->columnCount + column].string;
					return value ? value : "";
				}

				std::string cell(const row_t& row, const std::size_t column)
				{
					return column < row.size() ? row[column] : std::string{};
				}

				float cell_float(const row_t& row, const std::size_t column)
				{
					return static_cast<float>(std::atof(cell(row, column).data()));
				}

				const row_t* find_row(const std::vector<row_t>& rows, const std::string& name)
				{
					const auto row = std::ranges::find_if(rows, [&](const row_t& entry) { return cell(entry, 0) == name; });
					return row != rows.end() ? &*row : nullptr;
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
					if (!asset)
					{
						return sections;
					}

					std::string section;
					for (auto row = 0; row < asset->rowCount; row++)
					{
						const std::string name = table_cell(asset, row, 0);
						if (std::ranges::find(iw6_sections, name) != iw6_sections.end())
						{
							section = name;
							continue;
						}

						if (name.empty() || section.empty() || name == "END_OF_FILE")
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

				void add_global_rows(std::vector<row_t>& rows, const std::vector<row_t>& global_rows, const std::string& name)
				{
					if (name.empty() || find_row(rows, name))
					{
						return;
					}

					for (const auto& row : global_rows)
					{
						if (cell(row, 0) == name)
						{
							rows.push_back(row);
						}
					}
				}

				void dump_reverb_preset(const std::string& name)
				{
					if (auto* preset = db_find_x_asset_header_safe(ASSET_TYPE_REVERB_PRESET, name).reverbPreset)
					{
						zonetool::h1::reverb_preset::dump(reinterpret_cast<zonetool::h1::ReverbPreset*>(preset));
						add_csv_row("reverbpreset", name);
					}
				}

				std::vector<row_t> convert_reverbs(const std::vector<row_t>& rows)
				{
					std::vector<row_t> reverbs;
					for (const auto& row : rows)
					{
						if (!find_row(reverbs, cell(row, 0)))
						{
							reverbs.push_back(row);
							dump_reverb_preset(cell(row, 1));
						}
					}

					return reverbs;
				}

				std::vector<row_t> convert_filters(const std::vector<row_t>& rows)
				{
					struct bus_filter
					{
						row_t values;
						std::set<std::string> channels;
						bool conflict = false;
					};

					std::vector<std::string> names;
					std::map<std::string, std::map<zonetool::h1::SoundDspBus, bus_filter>> filters;
					for (const auto& row : rows)
					{
						const auto bus = sound::get_dsp_bus(cell(row, 1).data());
						if (!bus || std::atoi(cell(row, 2).data()) != 0)
						{
							continue;
						}

						if (!filters.contains(cell(row, 0)))
						{
							names.push_back(cell(row, 0));
						}

						auto& filter = filters[cell(row, 0)][*bus];
						const row_t values{cell(row, 3), cell(row, 4), cell(row, 5), cell(row, 6)};
						filter.conflict |= !filter.channels.empty() && filter.values != values;
						filter.values = values;
						filter.channels.insert(cell(row, 1));
					}

					std::vector<row_t> h1_rows;
					for (const auto& name : names)
					{
						for (const auto& [bus, filter] : filters[name])
						{
							if (!filter.conflict && filter.channels.size() == sound::get_dsp_bus_channel_count(bus))
							{
								h1_rows.push_back({name, zonetool::h1::sound::get_dsp_bus_name(static_cast<char>(bus)),
									filter.values[0], filter.values[1], filter.values[2], filter.values[3]});
							}
						}
					}

					return h1_rows;
				}

				std::vector<row_t> convert_mixes(const std::vector<row_t>& rows)
				{
					std::vector<row_t> mixes;
					for (const auto& row : rows)
					{
						const auto vol_mod = sound::get_vol_mod(cell(row, 1).data());
						if (!vol_mod)
						{
							continue;
						}

						const std::string vol_mod_name = zonetool::h1::sound::get_vol_mod_name(static_cast<short>(*vol_mod));
						const auto exists = std::ranges::any_of(mixes, [&](const row_t& mix)
						{
							return cell(mix, 0) == cell(row, 0) && cell(mix, 1) == vol_mod_name;
						});

						if (!exists)
						{
							mixes.push_back({cell(row, 0), vol_mod_name, cell(row, 2), cell(row, 3), cell(row, 3)});
						}
					}

					return mixes;
				}

				void convert_ambients(sections_t& sections, std::vector<row_t>& components, std::vector<row_t>& ambiences)
				{
					const auto& elements = sections["ambient_element"];
					for (const auto& ambient : sections["ambient_name"])
					{
						std::vector<std::pair<const row_t*, float>> picks;
						auto total_weight = 0.0f;
						for (const auto& event : sections["ambient_event"])
						{
							const auto* element = find_row(elements, cell(event, 1));
							const auto weight = cell_float(event, 2);
							if (cell(event, 0) == cell(ambient, 0) && element && weight > 0.0f)
							{
								picks.emplace_back(element, weight);
								total_weight += weight;
							}
						}

						const auto time = [&](const row_t& element, const std::size_t ambient_column, const std::size_t element_column)
						{
							return cell(ambient, ambient_column).empty() ? cell_float(element, element_column) : cell_float(ambient, ambient_column);
						};

						auto cycle_min = 0.0f;
						auto cycle_max = 0.0f;
						for (const auto& [element, weight] : picks)
						{
							cycle_min += time(*element, 1, 6) * weight / total_weight;
							cycle_max += time(*element, 2, 7) * weight / total_weight;
						}

						std::string names;
						for (const auto& [element, weight] : picks)
						{
							auto name = cell(*element, 0);
							if (find_row(components, name))
							{
								name = cell(ambient, 0) + "_" + name;
							}

							const auto event_min = std::format("{}", cycle_min * total_weight / weight);
							const auto event_max = std::format("{}", cycle_max * total_weight / weight);
							components.push_back({name, "", cell(*element, 1), event_min, event_max, event_min, event_max,
								cell(*element, 2), cell(*element, 3), cell(*element, 4), cell(*element, 5)});
							names += (names.empty() ? "" : " + ") + name;
						}

						ambiences.push_back({cell(ambient, 0), names});
					}
				}
			}

			void dump(StringTable* asset)
			{
				auto sections = read_sections(asset);
				auto globals = read_sections(db_find_x_asset_header_safe(ASSET_TYPE_STRINGTABLE, "soundtables/common_mp.csv").stringTable);

				std::vector<row_t> zones;
				auto& reverbs = sections["reverb_name"];
				auto& filters = sections["filter_name"];
				auto& mixes = sections["mix_name"];
				for (const auto& zone : sections["zone_name"])
				{
					add_global_rows(reverbs, globals["reverb_name"], cell(zone, 4));
					add_global_rows(filters, globals["filter_name"], cell(zone, 5));
					add_global_rows(mixes, globals["mix_name"], cell(zone, 3));

					const auto occlusion = cell(zone, 6).empty() ? "" : "mp_tuned_default";
					zones.push_back({cell(zone, 0), "", cell(zone, 1), cell(zone, 2), cell(zone, 3), cell(zone, 4), cell(zone, 5), occlusion, cell(zone, 7)});
				}

				std::vector<row_t> components;
				std::vector<row_t> ambiences;
				convert_ambients(sections, components, ambiences);

				const auto h1_reverbs = convert_reverbs(reverbs);
				const auto h1_filters = convert_filters(filters);
				const auto h1_mixes = convert_mixes(mixes);

				const std::array<std::pair<std::string_view, const std::vector<row_t>*>, 9> h1_sections
				{{
					{"zone_names,comments,streamed_ambience,ambient_name,mix,reverb,filter,occlusion,player_adsr", &zones},
					{"filter_names,dsp_bus_name,type,freq,gain,q", &h1_filters},
					{"reverb_names,roomtype,drylevel,wetlevel,fade", &h1_reverbs},
					{"dynamic_component,comment,alias,first_event_time_min,first_event_time_max,event_time_min,event_time_max,range_min,range_max,cone_min,cone_max", &components},
					{"dynamic_ambience_names,components", &ambiences},
					{"adsr_zone_player,weapon,adsr", &sections["adsr_zone_player"]},
					{"adsr_zone_npc,weapon,adsr", &sections["adsr_zone_npc"]},
					{"adsr_name,att_len,dec_len,sus_len,sus_level,rel_len", &sections["adsr_name"]},
					{"mix_names,volmod,value,fadein,fadeout", &h1_mixes},
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

				filesystem::file file(asset->name);
				file.open("wb");
				file.write(buffer.data(), buffer.size(), 1);
				file.close();
			}
		}
	}
}
