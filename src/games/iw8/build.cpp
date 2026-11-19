#include <std_include.hpp>
#include "iw8.hpp"
#include "zone/loader.hpp"

#include <game.hpp>
#include <formats/csv.hpp>
#include <utils/flags.hpp>

namespace zonetool::iw8
{
	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;
	std::unordered_map<std::string, std::string> renamed_assets;

	std::string renamed_asset_name(const std::string& name)
	{
		const auto is_reference = name.starts_with(',');
		const auto renamed = renamed_assets.find(is_reference ? name.substr(1) : name);
		if (renamed == renamed_assets.end())
		{
			return name;
		}

		return (is_reference ? ","s : ""s) + renamed->second;
	}

	namespace
	{
		struct build_context
		{
			std::string fastfile;
			zone_interface* zone;
			std::vector<std::unique_ptr<loaded_zone>> required_zones;
			bool is_referencing{};
		};

		std::vector<csv::row*> read_csv_rows(csv::parser& parser)
		{
			std::vector<csv::row*> rows;

			const auto raw_rows = parser.get_rows();
			if (!raw_rows)
			{
				return rows;
			}

			for (auto i = 0; i < parser.get_num_rows(); i++)
			{
				const auto row = raw_rows[i];
				if (!row || !row->fields || !row->num_fields)
				{
					continue;
				}

				const std::string_view command = row->fields[0];
				if (command.empty() || command.starts_with('#') || command.starts_with("//"))
				{
					continue;
				}

				rows.emplace_back(row);
			}

			return rows;
		}

		std::string_view field(const csv::row* row, const int index)
		{
			if (index >= row->num_fields || !row->fields[index])
			{
				return {};
			}

			return row->fields[index];
		}

		csv::parser open_csv(const std::string& csv_name)
		{
			return csv::parser("zone_source\\" + csv_name + ".csv", ',');
		}

		void parse_ignore_csv(const std::string& csv_name)
		{
			auto parser = open_csv(csv_name);
			if (!parser.valid())
			{
				throw std::runtime_error(std::format("Could not find csv file \"{}\"", csv_name));
			}

			for (const auto row : read_csv_rows(parser))
			{
				const auto type = std::string(field(row, 0));
				const auto name = field(row, 1);
				if (name.empty() || !is_valid_asset_type(type))
				{
					continue;
				}

				ignore_assets.emplace(static_cast<std::uint32_t>(type_to_int(type)), std::string(name));
			}
		}

		void rename_asset(const std::string& name, const std::string& new_name)
		{
			if (name.empty() || new_name.empty())
			{
				throw std::runtime_error(std::format("rename row needs a name and a new name (\"{}\")", name));
			}

			renamed_assets[name] = new_name;
			filesystem::add_renamed_source(new_name, name);
		}

		void require_zone(build_context& context, const std::string_view name)
		{
			const auto path = game_files::find_zone(std::string(name));
			if (!path)
			{
				throw std::runtime_error(std::format("required zone \"{}\" not found", name));
			}

			ZONETOOL_INFO("Loading required zone \"%s\"...", std::string(name).data());
			context.required_zones.emplace_back(load_zone(*path));
		}

		void add_asset(build_context& context, const csv::row* row)
		{
			const auto type = std::string(field(row, 0));
			const auto name = std::string(field(row, 1));
			const auto reference_name = field(row, 2);

			if (!is_valid_asset_type(type))
			{
				return;
			}

			const auto type_index = type_to_int(type);
			if (name.empty() && !reference_name.empty())
			{
				context.zone->add_listed_asset(type_index, "," + std::string(reference_name));
			}
			else
			{
				context.zone->add_listed_asset(type_index, (context.is_referencing ? "," : "") + name);
			}
		}

		void register_referenced_assets(const std::vector<csv::row*>& rows)
		{
			std::set<std::pair<std::uint32_t, std::string>> referenced;
			std::set<std::pair<std::uint32_t, std::string>> included;

			auto is_referencing = false;
			for (const auto row : rows)
			{
				const auto type = std::string(field(row, 0));
				const auto name = field(row, 1);

				if (type == "reference")
				{
					is_referencing = name == "true";
					continue;
				}

				if (!is_valid_asset_type(type))
				{
					continue;
				}

				const auto type_index = static_cast<std::uint32_t>(type_to_int(type));
				const auto referenced_name = name.empty() ? field(row, 2) : is_referencing ? name : std::string_view{};
				if (!referenced_name.empty())
				{
					referenced.emplace(type_index, std::string(referenced_name));
				}
				else if (!name.empty())
				{
					included.emplace(type_index, std::string(name));
				}
			}

			for (const auto& asset : referenced)
			{
				if (!included.contains(asset))
				{
					ignore_assets.insert(asset);
				}
			}
		}

		void parse_csv(build_context& context, const std::string& csv_name)
		{
			auto parser = open_csv(csv_name);
			if (!parser.valid())
			{
				throw std::runtime_error(std::format("Could not find csv file \"{}\" to build zone", csv_name));
			}

			const auto rows = read_csv_rows(parser);
			register_referenced_assets(rows);

			for (const auto row : rows)
			{
				const auto command = field(row, 0);
				const auto argument = field(row, 1);

				if (command == "require")
				{
					require_zone(context, argument);
				}
				else if (command == "include")
				{
					parse_csv(context, std::string(argument));
				}
				else if (command == "ignore")
				{
					parse_ignore_csv(std::string(argument));
				}
				else if (command == "reference")
				{
					context.is_referencing = argument == "true";
				}
				else if (command == "rename")
				{
					rename_asset(std::string(argument), std::string(field(row, 2)));
				}
				else if (command == "addpath")
				{
					filesystem::add_path(std::string(argument), field(row, 2) == "true");
				}
				else if (command == "addpaths")
				{
					filesystem::add_paths_from_directory(std::string(argument), field(row, 2) == "true");
				}
				else
				{
					add_asset(context, row);
				}
			}
		}

		void clear_build_state()
		{
			ignore_assets.clear();
			renamed_assets.clear();
			db_clear();
		}
	}

	void build_zone(const std::filesystem::path& zone_folder, const std::string& name)
	{
		clear_build_state();
		const auto _ = gsl::finally(clear_build_state);

		filesystem::set_zone_folder(zone_folder);
		filesystem::set_fastfile(name);

		ZONETOOL_INFO("Building fastfile \"%s\"", name.data());

		auto zone = std::make_unique<zone_interface>(name);
		build_context context{.fastfile = name, .zone = zone.get()};

		const auto search_paths = filesystem::get_search_paths();
		parse_csv(context, name);
		filesystem::get_search_paths() = search_paths;

		const auto buffer_size = utils::flags::has_flag("more_memory") ? max_zone_size * 2 : max_zone_size;
		zone_buffer buffer(buffer_size);
		buffer.mode = ::game::iw8;
		buffer.set_fields(XFILE_BLOCK_RUNTIME, 0xFFFFFFF000000000, static_cast<std::uint32_t>(-2),
			static_cast<std::uint32_t>(-4), static_cast<std::uint32_t>(-3), static_cast<std::uint32_t>(-1));
		zone->build(&buffer);
	}
}
