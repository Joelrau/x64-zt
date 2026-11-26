#include <std_include.hpp>
#include "t9.hpp"
#include "zone/loader.hpp"

#include <game.hpp>
#include <formats/csv.hpp>

#include <charconv>

namespace zonetool::t9
{
	const game& definition();

	namespace
	{
		struct build_context
		{
			std::vector<std::unique_ptr<loaded_zone>> source_zones;
			std::vector<asset_file> asset_files;
			std::vector<const char*> strings;
			std::string strings_zone;
			std::vector<XAsset> assets;
			std::map<std::pair<std::int32_t, std::string>, const void*> named_assets;
			std::unordered_set<const void*> references;
			std::map<std::string, std::uint64_t> source_zone_ids;
			std::map<std::uint64_t, address_range> identities;
			std::map<std::uint64_t, address_range> addresses;
			utils::memory::allocator allocator;
		};

		std::uint64_t make_identity(const std::uint64_t zone_id, const std::uint64_t block, const std::uint64_t offset)
		{
			return zone_id << 48 | block << 44 | offset;
		}

		std::optional<std::uint64_t> find_target(const std::map<std::uint64_t, address_range>& ranges, const std::uint64_t key)
		{
			auto range = ranges.upper_bound(key);
			if (range == ranges.begin())
			{
				return {};
			}

			--range;
			if (key >= range->second.end)
			{
				return {};
			}

			return range->second.target + (key - range->first);
		}

		std::vector<std::uint8_t> read_source_file(const std::string& path)
		{
			filesystem::file file(path);
			if (!file.exists())
			{
				throw std::runtime_error(std::format("\"{}\" not found", path));
			}

			file.open("rb");
			auto bytes = file.read_bytes(file.size());
			file.close();
			return bytes;
		}

		void add_source_zone(build_context& context, const std::string& name)
		{
			const auto path = game_files::find_zone(name);
			if (!path)
			{
				throw std::runtime_error(std::format("zone \"{}\" not found", name));
			}

			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());
			auto& zone = context.source_zones.emplace_back(load_zone(*path));
			for (auto i = 0; i < zone->list.stringCount; i++)
			{
				context.strings.emplace_back(zone->list.strings[i]);
			}

			for (auto i = 0; i < zone->list.assetCount; i++)
			{
				context.assets.emplace_back(zone->list.assets[i]);
			}

			for (const auto& asset : zone->assets)
			{
				if (const auto asset_name = get_asset_name(asset.type, asset.header); !asset_name.empty())
				{
					context.named_assets.emplace(std::make_pair(asset.type, asset_name), asset.header);
				}
			}
		}

		void add_script_strings(build_context& context, const std::string& name)
		{
			const auto bytes = read_source_file(std::format("scriptstrings/{}.json", name));
			const auto strings = json::parse(bytes.begin(), bytes.end());
			for (const auto& string : strings)
			{
				if (string.is_null())
				{
					context.strings.emplace_back(nullptr);
					continue;
				}

				const auto hex = string.get<std::string>();
				std::string text(hex.size() / 2, '\0');
				for (std::size_t i = 0; i < text.size(); i++)
				{
					std::uint8_t value{};
					if (std::from_chars(hex.data() + i * 2, hex.data() + i * 2 + 2, value, 16).ec != std::errc{})
					{
						throw std::runtime_error(std::format("script string \"{}\" is not hex", hex));
					}

					text[i] = static_cast<char>(value);
				}

				context.strings.emplace_back(context.allocator.duplicate_string(text));
			}

			if (context.strings_zone.empty())
			{
				context.strings_zone = name;
			}
		}

		void add_key_value_pairs(build_context& context, const std::string& name)
		{
			const auto asset = key_value_pairs::parse(name, context.allocator);
			if (!asset)
			{
				throw std::runtime_error(std::format("keyvaluepairs \"{}\" not found", name));
			}

			auto& entry = context.assets.emplace_back();
			entry.type = ASSET_TYPE_KEYVALUEPAIRS;
			entry.header.data = asset;
			context.named_assets.insert_or_assign(std::make_pair(ASSET_TYPE_KEYVALUEPAIRS, name), asset);
		}

		void add_parsed_asset(build_context& context, const std::string& type_name, const std::string& name)
		{
			const auto type = type_to_int(type_name);
			if (type < 0)
			{
				throw std::runtime_error(std::format("unknown asset type \"{}\"", type_name));
			}

			if (type == ASSET_TYPE_KEYVALUEPAIRS)
			{
				add_key_value_pairs(context, name);
				return;
			}

			const auto bytes = read_source_file(get_asset_file_path(static_cast<XAssetType>(type), name));
			auto& file = context.asset_files.emplace_back(load_asset_file(name, std::string(bytes.begin(), bytes.end())));

			const auto zone_id = context.source_zone_ids.emplace(file.source_zone, context.source_zone_ids.size()).first->second;
			for (const auto& chunk : file.chunks)
			{
				const auto address = reinterpret_cast<std::uint64_t>(file.zone->reader->block_address(chunk.block, chunk.offset));
				const auto identity = make_identity(zone_id, chunk.source_block, chunk.source_offset);
				context.identities.emplace(address, address_range{address + chunk.size, identity});
				context.addresses.emplace(identity, address_range{identity + chunk.size, address});
			}

			const auto& asset = file.zone->list.assets[0];
			if (asset.type != type)
			{
				throw std::runtime_error(std::format("{} \"{}\": the asset file holds a {}", type_name, name, type_to_string(asset.type)));
			}

			context.assets.emplace_back(asset);
			if (const auto asset_name = get_asset_name(asset.type, asset.header.data); !asset_name.empty())
			{
				context.named_assets.insert_or_assign(std::make_pair(type, asset_name), asset.header.data);
			}

			for (const auto& nested : file.zone->assets)
			{
				if (nested.header != asset.header.data)
				{
					context.references.insert(nested.header);
				}
			}
		}

		void parse_csv(build_context& context, const std::string& name)
		{
			csv::parser parser("zone_source\\" + name + ".csv", ',');
			if (!parser.valid())
			{
				throw std::runtime_error(std::format("Could not find csv file \"{}\" to build zone", name));
			}

			const auto rows = parser.get_rows();
			for (auto i = 0; rows && i < parser.get_num_rows(); i++)
			{
				const auto row = rows[i];
				if (!row || row->num_fields < 2 || !row->fields[0] || !row->fields[1])
				{
					continue;
				}

				const std::string command = row->fields[0];
				const std::string argument = row->fields[1];
				if (command.empty() || command.starts_with('#') || command.starts_with("//"))
				{
					continue;
				}

				if (command == "zone")
				{
					add_source_zone(context, argument);
				}
				else if (command == "scriptstrings")
				{
					add_script_strings(context, argument);
				}
				else
				{
					add_parsed_asset(context, command, argument);
				}
			}
		}

		void link_external_pointers(build_context& context)
		{
			for (auto& file : context.asset_files)
			{
				const auto zone_id = context.source_zone_ids.at(file.source_zone);
				for (const auto& pointer : file.pointers)
				{
					const auto field = reinterpret_cast<void**>(file.zone->reader->block_address(pointer.block, pointer.offset));
					if (const auto target = find_target(context.addresses, make_identity(zone_id, pointer.target_block, pointer.target_offset)))
					{
						*field = reinterpret_cast<void*>(*target);
					}
					else
					{
						ZONETOOL_WARNING("%s: a pointer into the data of a different asset has no target in this zone", file.zone->name.data());
					}
				}
			}
		}

		const void* resolve_reference(const build_context& context, const std::int32_t type, const void* header)
		{
			if (!context.references.contains(header))
			{
				return header;
			}

			const auto asset = context.named_assets.find(std::make_pair(type, get_asset_name(static_cast<XAssetType>(type), header)));
			return asset != context.named_assets.end() ? asset->second : header;
		}

		bool is_inserted(const build_context& context, const void* data)
		{
			return std::ranges::any_of(context.source_zones, [&](const auto& zone) { return zone->reader->is_inserted(data); })
				|| std::ranges::any_of(context.asset_files, [&](const auto& file) { return file.zone->reader->is_inserted(data); });
		}

		void write_zone(zone_writer& writer, build_context& context)
		{
			XAssetList list{};
			list.stringCount = static_cast<std::int32_t>(context.strings.size());
			list.strings = context.strings.empty() ? nullptr : context.strings.data();
			list.unknown = context.strings.empty() ? nullptr : context.allocator.allocate_array<std::int32_t>(context.strings.size());
			list.assetCount = static_cast<std::int32_t>(context.assets.size());
			list.assets = context.assets.empty() ? nullptr : context.assets.data();
			write_asset_list(writer, list);
		}

		xfile::fastfile read_template(const build_context& context, const std::string& name)
		{
			if (!context.source_zones.empty())
			{
				return context.source_zones.front()->file;
			}

			auto path = game_files::find_zone(name);
			if (!path && !context.strings_zone.empty())
			{
				path = game_files::find_zone(context.strings_zone);
			}

			if (!path)
			{
				throw std::runtime_error(std::format("zone \"{}\" not found, its container header is the template", name));
			}

			return xfile::read(*path, definition().format);
		}
	}

	void build_zone(const std::filesystem::path& zone_folder, const std::string& name)
	{
		filesystem::set_zone_folder(zone_folder);
		filesystem::set_fastfile(name);
		ZONETOOL_INFO("Building fastfile \"%s\"", name.data());

		build_context context;
		parse_csv(context, name);
		link_external_pointers(context);

		const auto& format = definition().format;
		auto file = read_template(context, name);
		file.payload.clear();

		zone_writer writer(get_zone_layout(), format.block_count, max_zone_size);
		writer.mode = ::game::t9;
		writer.set_inline_asset_writer([&writer](const std::int32_t type, const void* header)
		{
			write_inline_asset(writer, type, header);
		});
		writer.set_inserted_check([&context](const void* data)
		{
			return is_inserted(context, data);
		});
		writer.set_asset_resolver([&context](const std::int32_t type, const void* header)
		{
			return resolve_reference(context, type, header);
		});
		writer.set_data_identity([&context](const void* data)
		{
			return find_target(context.identities, reinterpret_cast<std::uint64_t>(data));
		});

		write_zone(writer, context);
		file.block_sizes = writer.block_sizes();
		ZONETOOL_WARNING("Zone \"%s\": the sizes of blocks 1 and 11 have no proven rule", name.data()); // TODO: blocks 1 and 11

		const auto path = filesystem::get_output_path() + name + ".ff";
		xfile::write_t9(path, format, file, writer.data());
		ZONETOOL_INFO("Fastfile \"%s\" written to %s", name.data(), path.data());
	}
}
