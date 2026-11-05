#include <std_include.hpp>
#include "s2.hpp"
#include "zone/loader.hpp"

#include <formats/csv.hpp>
#include <formats/cast/cast.hpp>
#include <utils/flags.hpp>
#include <utils/string.hpp>

namespace zonetool::s2
{
	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	namespace
	{
		struct build_context
		{
			std::filesystem::path zone_folder;
			std::string fastfile;
			zone_interface* zone;
			std::vector<std::unique_ptr<loaded_zone>> required_zones;
			bool is_referencing{};
		};

		struct asset_folder
		{
			std::string_view type;
			std::string_view folder;
			std::string_view extension;
		};

		constexpr asset_folder iterated_asset_folders[]
		{
			{"fx", "effects", ".fxe"},
			{"material", "materials", ""},
			{"xmodel", "xmodel", ".xmb"},
			{"xmodel", "xmodel", ".cast"},
			{"xanim", "xanim", ".xab"},
			{"xanim", "xanim", ".cast"},
		};

		std::optional<std::string> cast_asset_name(std::string name)
		{
			constexpr std::string_view lod_suffix = "_lod";
			const auto suffix_size = lod_suffix.size() + 1;
			if (name.size() <= suffix_size)
			{
				return name;
			}

			const auto suffix = utils::string::to_lower(name.substr(name.size() - suffix_size));
			const auto lod = suffix.back();
			if (!suffix.starts_with(lod_suffix) || lod < '0' || lod > '5')
			{
				return name;
			}

			if (lod != '0')
			{
				return {};
			}

			name.resize(name.size() - suffix_size);
			return name;
		}

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
				const auto type = field(row, 0);
				const auto name = field(row, 1);
				if (name.empty() || !is_valid_asset_type(std::string(type)))
				{
					continue;
				}

				ignore_assets.emplace(static_cast<std::uint32_t>(type_to_int(std::string(type))), std::string(name));
			}
		}

		void add_assets_from_folder(build_context& context, const asset_folder& folder)
		{
			const auto path = std::filesystem::path("zonetool") / context.fastfile / folder.folder;
			if (!std::filesystem::is_directory(path))
			{
				return;
			}

			for (const auto& file : std::filesystem::recursive_directory_iterator(path))
			{
				if (!file.is_regular_file())
				{
					continue;
				}

				const auto filename = file.path().filename().string();
				if (filename.starts_with(','))
				{
					continue;
				}

				if (!folder.extension.empty() && filename.ends_with(folder.extension))
				{
					const auto name = filename.substr(0, filename.size() - folder.extension.size());
					const auto asset_name = folder.extension == ".cast" ? cast_asset_name(name) : name;
					if (asset_name)
					{
						context.zone->add_asset_of_type(std::string(folder.type), *asset_name);
					}
				}
				else if (!file.path().has_extension())
				{
					context.zone->add_asset_of_type(std::string(folder.type), filename);
				}
			}
		}

		void iterate_assets(build_context& context, const std::string_view type)
		{
			const auto iterate_all = type == "true";
			for (const auto& folder : iterated_asset_folders)
			{
				if (iterate_all || folder.type == type)
				{
					add_assets_from_folder(context, folder);
				}
			}
		}

		void require_zone(build_context& context, const std::string_view name)
		{
			const auto path = context.zone_folder / std::filesystem::path(name).replace_extension(".ff");
			ZONETOOL_INFO("Loading required zone \"%s\"...", path.string().data());
			context.required_zones.emplace_back(load_zone(path));
		}

		void add_localize(build_context& context, const std::string& name)
		{
			if (filesystem::file("localizedstrings/" + name + ".str").exists())
			{
				localize::parse_localizedstrings_file(context.zone, name);
			}
			else if (filesystem::file("localizedstrings/" + name + ".json").exists())
			{
				localize::parse_localizedstrings_json(context.zone, name);
			}
			else
			{
				context.zone->add_asset_of_type(ASSET_TYPE_LOCALIZE_ENTRY,
					(context.is_referencing ? "," : "") + name);
			}
		}

		void add_asset(build_context& context, const csv::row* row)
		{
			const auto type = std::string(field(row, 0));
			const auto name = std::string(field(row, 1));
			const auto reference_name = field(row, 2);

			if (type == "localize" && !name.empty())
			{
				add_localize(context, name);
				return;
			}

			if (!is_valid_asset_type(type))
			{
				return;
			}

			if (name.empty() && !reference_name.empty())
			{
				context.zone->add_asset_of_type(type, "," + std::string(reference_name));
			}
			else
			{
				context.zone->add_asset_of_type(type, (context.is_referencing ? "," : "") + name);
			}
		}

		void register_referenced_assets(const std::vector<csv::row*>& rows)
		{
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

				const auto referenced_name = name.empty() ? field(row, 2) : is_referencing ? name : std::string_view{};
				if (!referenced_name.empty())
				{
					ignore_assets.emplace(static_cast<std::uint32_t>(type_to_int(type)), std::string(referenced_name));
				}
			}
		}

		void set_cast_scale(const std::string_view value)
		{
			const auto scale = cast::parse_import_scale(std::string(value));
			if (scale <= 0.0f)
			{
				ZONETOOL_ERROR("castscale \"%s\" is not a positive factor or a known unit (inch, cm, mm, m)", std::string(value).data());
				return;
			}

			cast::set_import_scale(scale);
			ZONETOOL_INFO("Cast files import at a scale of %g", scale);
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
				else if (command == "iterate")
				{
					iterate_assets(context, argument);
				}
				else if (command == "castscale")
				{
					set_cast_scale(argument);
				}
				else if (command == "castroot")
				{
					cast::set_import_root(std::string(argument));
					ZONETOOL_INFO("Stray cast root bones go under \"%s\"", std::string(argument).data());
				}
				else if (command == "castignorebone")
				{
					cast::add_ignored_bone(std::string(argument));
					ZONETOOL_INFO("Cast animations do not drive \"%s\"", std::string(argument).data());
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
			db_clear();
			cast::reset_import_settings();
			cast_xmodel::reset();
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
		build_context context{.zone_folder = zone_folder, .fastfile = name, .zone = zone.get()};

		const auto search_paths = filesystem::get_search_paths();
		parse_csv(context, name);
		filesystem::get_search_paths() = search_paths;

		zone->add_asset_of_type("rawfile", name);

		const auto buffer_size = utils::flags::has_flag("more_memory") ? max_zone_size * 2 : max_zone_size;
		zone_buffer buffer(buffer_size);
		buffer.mode = ::game::s2;
		buffer.set_fields(XFILE_BLOCK_RUNTIME, 0xFFFFFFF000000000, static_cast<std::uint32_t>(-1),
			static_cast<std::uint32_t>(-3), static_cast<std::uint32_t>(-2));
		zone->build(&buffer);
	}
}
