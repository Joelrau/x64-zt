#include <std_include.hpp>
#include "t9.hpp"
#include "loader.hpp"

#include <game.hpp>

namespace zonetool::t9
{
	const game& definition();

	namespace
	{
		const zone_layout layout
		{
			.offset_tag = 0,
			.following_markers = {0xFFFFFFFFFFFFFFFF},
			.insert_marker = 0xFFFFFFFFFFFFFFFE,
			.runtime_block = XFILE_BLOCK_RUNTIME_VIRTUAL,
			.virtual_block = XFILE_BLOCK_VIRTUAL,
			.pop_aligned_block = XFILE_BLOCK_TEMP_PRELOAD,
			.temp_block = XFILE_BLOCK_TEMP,
			.unread_blocks = {XFILE_BLOCK_RUNTIME_PHYSICAL, XFILE_BLOCK_TEMP_PRELOAD_SAVED_PTR},
			.reserved_blocks = {XFILE_BLOCK_STREAMER, XFILE_BLOCK_STREAMER_CPU},
			.inherited_blocks = {XFILE_BLOCK_MEMMAPPED},
			.asset_alignment = 7,
			.pop_alignment = 15,
			.offset_format = zone_offset_format::treyarch,
		};

		template <typename S>
		std::string asset_name_field(const void* asset)
		{
			if constexpr (requires(const S* s) { { s->name } -> std::convertible_to<XHash>; })
			{
				return xhash_name(static_cast<const S*>(asset)->name);
			}
			else if constexpr (requires(const S* s) { std::string_view(s->name); })
			{
				const auto name = static_cast<const S*>(asset)->name;
				return name ? name : "";
			}
			else
			{
				return {};
			}
		}

		template <typename Asset, typename S>
		constexpr asset_loader make_loader()
		{
			asset_loader loader{.size = sizeof(S), .alignment = 7, .name = asset_name_field<S>};
			if constexpr (requires { Asset::alignment; })
			{
				loader.alignment = Asset::alignment;
			}

			if constexpr (requires(zone_reader& reader, S* asset) { Asset::read(reader, asset); })
			{
				loader.read = [](zone_reader& reader, void* asset)
				{
					Asset::read(reader, static_cast<S*>(asset));
				};

				loader.write = [](zone_writer& writer, void* asset)
				{
					Asset::read(writer, static_cast<S*>(asset));
				};
			}

			if constexpr (requires(S* asset) { Asset::dump(asset); })
			{
				loader.dump = [](void* asset)
				{
					Asset::dump(static_cast<S*>(asset));
				};
			}

			if constexpr (requires(const S* asset) { Asset::is_placeholder(asset); })
			{
				loader.is_placeholder = [](const void* asset)
				{
					return Asset::is_placeholder(static_cast<const S*>(asset));
				};
			}

			if constexpr (requires(S* s) { { s->name } -> std::convertible_to<XHash>; })
			{
				loader.copy_name = [](void* destination, const void* source)
				{
					static_cast<S*>(destination)->name.value = static_cast<const S*>(source)->name.value | 0x8000000000000000ull; // default asset
				};
			}
			else if constexpr (requires(S* s) { std::string_view(s->name); })
			{
				loader.copy_name = [](void* destination, const void* source)
				{
					static_cast<S*>(destination)->name = static_cast<const S*>(source)->name;
				};
			}

			return loader;
		}

		const std::array<asset_loader, ASSET_TYPE_COUNT>& asset_loaders()
		{
			static const auto loaders = []
			{
				std::array<asset_loader, ASSET_TYPE_COUNT> list{};
#define ADD_LOADER(type, asset, struct_name) list[type] = make_loader<asset, struct_name>();
				T9_READ_ONLY_ASSETS(ADD_LOADER)
#undef ADD_LOADER
				return list;
			}();

			return loaders;
		}

		std::uint16_t next_zone_index()
		{
			static std::atomic_uint16_t index{};
			return ++index;
		}

		constexpr std::string_view asset_file_magic = "T9ZA";
		constexpr std::uint32_t asset_file_version = 1;

		template <typename T>
		void append_value(std::string& data, const T& value)
		{
			data.append(reinterpret_cast<const char*>(&value), sizeof(T));
		}

		template <typename T>
		T read_value(const std::string& data, std::size_t& position)
		{
			if (position + sizeof(T) > data.size())
			{
				throw std::runtime_error("the asset file is truncated");
			}

			T value{};
			std::memcpy(&value, data.data() + position, sizeof(T));
			position += sizeof(T);
			return value;
		}

		class asset_list_reader
		{
		public:
			asset_list_reader(loaded_zone& zone)
				: zone_(zone)
				, reader_(*zone.reader)
			{
			}

			void read_inline_asset(const std::int32_t type, void** field, void** insert_slot)
			{
				if (type < 0 || type >= ASSET_TYPE_COUNT || !asset_loaders()[type].read)
				{
					throw std::runtime_error(std::format("asset type {} ({}) has no reader", type,
						type >= 0 && type < ASSET_TYPE_COUNT ? type_to_string(static_cast<XAssetType>(type)) : "?"));
				}

				const auto asset_type = static_cast<XAssetType>(type);
				const auto& loader = asset_loaders()[type];
				this->reader_.push_asset();
				this->reader_.align(loader.alignment);
				const auto data = this->reader_.read_bytes(loader.size);
				this->reader_.push_stream(XFILE_BLOCK_VIRTUAL);
				try
				{
					loader.read(this->reader_, data);
				}
				catch (const std::exception& e)
				{
					throw std::runtime_error(std::format("{} \"{}\" (after {}): {}", type_to_string(asset_type),
						loader.name(data), this->previous_asset(), e.what()));
				}
				this->reader_.pop_stream();
				this->reader_.pop_asset();

				const auto header = static_cast<void*>(this->reader_.allocate<std::uint8_t>(loader.size));
				std::memcpy(header, data, loader.size);

				const auto name = loader.name(header);
				if (!loader.is_placeholder || !loader.is_placeholder(header) || !db_find_asset(type, name))
				{
					db_add_asset(type, name, header);
				}

				this->zone_.assets.emplace_back(asset_type, header);

				*field = header;
				if (insert_slot)
				{
					*insert_slot = header;
				}
			}

		private:
			loaded_zone& zone_;
			zone_reader& reader_;

			std::string previous_asset() const
			{
				if (this->zone_.assets.empty())
				{
					return "none";
				}

				const auto& asset = this->zone_.assets.back();
				return std::format("{} \"{}\"", type_to_string(asset.type), get_asset_name(asset.type, asset.header));
			}
		};

		void read_strings(zone_reader& reader, XAssetList& list)
		{
			if (const auto strings = reader.read_array(list.strings, 7, list.stringCount))
			{
				for (auto i = 0; i < list.stringCount; i++)
				{
					reader.read_string(strings[i]);
				}
			}

			reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			reader.read_array(list.unknown, 3, list.stringCount);
			reader.pop_stream();
		}
	}

	const zone_layout& get_zone_layout()
	{
		return layout;
	}

	const asset_loader& get_asset_loader(const XAssetType type)
	{
		return asset_loaders()[type];
	}

	std::string get_asset_name(const XAssetType type, const void* header)
	{
		const auto& loader = asset_loaders()[type];
		return loader.name ? loader.name(header) : std::string();
	}

	namespace
	{
		std::unique_ptr<loaded_zone> open_payload(std::string name, xfile::fastfile file)
		{
			auto zone = std::make_unique<loaded_zone>();
			zone->name = std::move(name);
			zone->file = std::move(file);
			zone->reader = std::make_unique<zone_reader>(zone->file.payload, zone->file.block_sizes, layout);

			auto& reader = *zone->reader;
			reader.mode = ::game::t9;
			reader.set_zone_index(next_zone_index());
			reader.read_raw(&zone->list, sizeof(zone->list));

			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			read_strings(reader, zone->list);
			reader.read_array(zone->list.assets, 7, zone->list.assetCount);
			reader.pop_stream();

			return zone;
		}

		void read_assets(loaded_zone& zone, const bool stop_at_error)
		{
			auto& reader = *zone.reader;

			asset_list_reader assets(zone);
			reader.set_inline_asset_loader([&assets](const std::int32_t type, void** field, void** insert_slot)
			{
				assets.read_inline_asset(type, field, insert_slot);
			});
			const auto _ = gsl::finally([&reader]
			{
				reader.set_inline_asset_loader({});
			});

			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			for (auto i = 0; i < zone.list.assetCount; i++)
			{
				try
				{
					reader.read_asset(zone.list.assets[i].type, zone.list.assets[i].header.data);
				}
				catch (const std::exception& e)
				{
					if (!stop_at_error)
					{
						throw;
					}

					ZONETOOL_WARNING("Zone \"%s\": read %d of %d assets, stopped at: %s", zone.name.data(), i, zone.list.assetCount, e.what());
					return;
				}
			}
			reader.pop_stream();

			if (reader.remaining())
			{
				throw std::runtime_error(std::format("0x{:X} bytes remain after the last asset", reader.remaining()));
			}
		}
	}

	std::unique_ptr<loaded_zone> open_zone(const std::filesystem::path& path)
	{
		return open_payload(path.stem().string(), xfile::read(path, definition().format));
	}

	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path, const bool stop_at_error)
	{
		auto zone = open_zone(path);
		read_assets(*zone, stop_at_error);
		return zone;
	}

	void write_inline_asset(zone_writer& writer, const std::int32_t type, const void* header)
	{
		const auto& loader = asset_loaders()[type];
		if (!loader.write)
		{
			throw std::runtime_error(std::format("asset type {} ({}) has no writer", type, type_to_string(static_cast<XAssetType>(type))));
		}

		writer.push_asset();
		writer.align(loader.alignment);
		const auto copy = writer.write_bytes(header, loader.size);
		writer.push_stream(XFILE_BLOCK_VIRTUAL);
		loader.write(writer, copy);
		writer.pop_stream();
		writer.pop_asset();
	}

	void write_asset_list(zone_writer& writer, const XAssetList& list)
	{
		const auto copy = static_cast<XAssetList*>(writer.write_bytes(&list, sizeof(list)));
		writer.push_stream(XFILE_BLOCK_VIRTUAL);
		if (const auto strings = writer.read_array(copy->strings, 7, copy->stringCount))
		{
			for (auto i = 0; i < copy->stringCount; i++)
			{
				writer.read_string(strings[i]);
			}
		}

		writer.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		writer.read_array(copy->unknown, 3, copy->stringCount);
		writer.pop_stream();

		if (const auto assets = writer.read_array(copy->assets, 7, copy->assetCount))
		{
			for (auto i = 0; i < copy->assetCount; i++)
			{
				writer.read_asset(assets[i].type, assets[i].header.data);
			}
		}
		writer.pop_stream();
		writer.finish();
	}

	std::string get_asset_file_path(const XAssetType type, const std::string& name)
	{
		auto file_name = name;
		std::ranges::replace_if(file_name, [](const char c) { return std::string_view("<>:\"|?*").contains(c); }, '_');
		return std::format("binary/{}/{}.asset", type_to_string(type), file_name);
	}

	namespace
	{
		std::vector<asset_file_chunk> get_source_chunks(const zone_writer& writer, const zone_reader& source)
		{
			std::vector<asset_file_chunk> chunks;
			for (const auto& chunk : writer.written_chunks())
			{
				const auto location = source.find_offset(chunk.source);
				if (location && location->block_index != XFILE_BLOCK_TEMP && chunk.block != XFILE_BLOCK_TEMP)
				{
					chunks.emplace_back(chunk.block, chunk.offset, static_cast<std::uint8_t>(location->block_index), location->offset, chunk.size);
				}
			}

			return chunks;
		}

		void append_chunks(std::string& file, const std::vector<asset_file_chunk>& chunks)
		{
			append_value(file, static_cast<std::uint32_t>(chunks.size()));
			for (const auto& chunk : chunks)
			{
				append_value(file, chunk.block);
				append_value(file, chunk.offset);
				append_value(file, chunk.source_block);
				append_value(file, chunk.source_offset);
				append_value(file, chunk.size);
			}
		}

		void append_pointers(std::string& file, const std::vector<asset_file_pointer>& pointers)
		{
			append_value(file, static_cast<std::uint32_t>(pointers.size()));
			for (const auto& pointer : pointers)
			{
				append_value(file, pointer.block);
				append_value(file, pointer.offset);
				append_value(file, pointer.target_block);
				append_value(file, pointer.target_offset);
			}
		}

		std::vector<asset_file_chunk> read_chunks(const std::string& data, std::size_t& position)
		{
			std::vector<asset_file_chunk> chunks(read_value<std::uint32_t>(data, position));
			for (auto& chunk : chunks)
			{
				chunk.block = read_value<std::uint8_t>(data, position);
				chunk.offset = read_value<std::uint64_t>(data, position);
				chunk.source_block = read_value<std::uint8_t>(data, position);
				chunk.source_offset = read_value<std::uint64_t>(data, position);
				chunk.size = read_value<std::uint64_t>(data, position);
			}

			return chunks;
		}

		std::vector<asset_file_pointer> read_pointers(const std::string& data, std::size_t& position)
		{
			std::vector<asset_file_pointer> pointers(read_value<std::uint32_t>(data, position));
			for (auto& pointer : pointers)
			{
				pointer.block = read_value<std::uint8_t>(data, position);
				pointer.offset = read_value<std::uint64_t>(data, position);
				pointer.target_block = read_value<std::uint8_t>(data, position);
				pointer.target_offset = read_value<std::uint64_t>(data, position);
			}

			return pointers;
		}
	}

	std::string write_asset_file(const loaded_zone& zone, const XAsset& asset, const std::unordered_set<const void*>& list_headers)
	{
		utils::memory::allocator allocator;
		std::unordered_map<const void*, const void*> stubs;

		zone_writer writer(layout, zone.file.block_sizes.size(), zone.file.payload.size() + 0x4000000);
		writer.mode = ::game::t9;
		writer.set_inline_asset_writer([&writer](const std::int32_t type, const void* header)
		{
			write_inline_asset(writer, type, header);
		});
		writer.set_inserted_check([&zone](const void* data)
		{
			return zone.reader->is_inserted(data);
		});
		writer.set_asset_resolver([&](const std::int32_t type, const void* header) -> const void*
		{
			const auto& loader = asset_loaders()[type];
			if (header == asset.header.data || !list_headers.contains(header) || !loader.copy_name || loader.name(header).empty())
			{
				return header;
			}

			auto& stub = stubs[header];
			if (!stub)
			{
				const auto copy = allocator.allocate_array<std::uint8_t>(loader.size);
				loader.copy_name(copy, header);
				stub = copy;
			}

			return stub;
		});

		std::vector<asset_file_pointer> pointers;
		writer.set_external_pointer([&](const void* target, const std::uint8_t block, const std::size_t offset)
		{
			const auto location = zone.reader->find_offset(target);
			if (!location || location->block_index == XFILE_BLOCK_TEMP || block == XFILE_BLOCK_TEMP)
			{
				return false;
			}

			pointers.emplace_back(block, offset, static_cast<std::uint8_t>(location->block_index), location->offset);
			return true;
		});

		auto entry = asset;
		XAssetList list{};
		list.assetCount = 1;
		list.assets = &entry;
		write_asset_list(writer, list);

		std::string file(asset_file_magic);
		append_value(file, asset_file_version);
		append_value(file, static_cast<std::uint32_t>(zone.name.size()));
		file.append(zone.name);

		const auto block_sizes = writer.block_sizes();
		append_value(file, static_cast<std::uint32_t>(block_sizes.size()));
		for (const auto size : block_sizes)
		{
			append_value(file, size);
		}

		append_chunks(file, get_source_chunks(writer, *zone.reader));
		append_pointers(file, pointers);
		file.append(writer.data());
		return file;
	}

	asset_file load_asset_file(const std::string& name, const std::string& data)
	{
		if (!data.starts_with(asset_file_magic))
		{
			throw std::runtime_error(std::format("\"{}\" is not a T9 asset file", name));
		}

		auto position = asset_file_magic.size();
		if (const auto version = read_value<std::uint32_t>(data, position); version != asset_file_version)
		{
			throw std::runtime_error(std::format("\"{}\" has version {}, expected {}", name, version, asset_file_version));
		}

		asset_file result{};
		const auto source_zone_size = read_value<std::uint32_t>(data, position);
		if (position + source_zone_size > data.size())
		{
			throw std::runtime_error("the asset file is truncated");
		}

		result.source_zone = data.substr(position, source_zone_size);
		position += source_zone_size;

		xfile::fastfile file{};
		file.block_sizes.resize(read_value<std::uint32_t>(data, position));
		if (file.block_sizes.size() != definition().format.block_count)
		{
			throw std::runtime_error(std::format("\"{}\" has {} blocks", name, file.block_sizes.size()));
		}

		for (auto& size : file.block_sizes)
		{
			size = read_value<std::uint64_t>(data, position);
		}

		result.chunks = read_chunks(data, position);
		result.pointers = read_pointers(data, position);

		file.payload = data.substr(position);

		auto zone = open_payload(name, std::move(file));
		read_assets(*zone, false);
		if (zone->list.assetCount != 1)
		{
			throw std::runtime_error(std::format("\"{}\" holds {} assets", name, zone->list.assetCount));
		}

		result.zone = std::move(zone);
		return result;
	}
}
