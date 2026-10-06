#include <std_include.hpp>
#include "h1.hpp"
#include "asset_list.hpp"
#include "loader.hpp"

#include <game.hpp>

namespace zonetool::h1
{
	const game& definition();

	namespace
	{
		constexpr std::uint64_t pointer_mask = 0xFDFDFDF000000000;
		constexpr std::uint64_t marker_base = pointer_mask | (0xFull << 32);

		const zone_layout layout
		{
			.offset_tag = pointer_mask >> 36,
			.following_markers = {marker_base | 0xFFFFFFFF},
			.insert_marker = marker_base | 0xFFFFFFFE,
			.runtime_block = XFILE_BLOCK_RUNTIME,
			.virtual_block = XFILE_BLOCK_VIRTUAL,
		};

		constexpr std::size_t image_stream_file_count = 4;

		struct asset_loader
		{
			std::size_t size;
			void (*read)(zone_reader& reader, void* asset);
			const char** (*name)(void* asset);
		};

		template <typename S>
		const char** asset_name_field(void* asset)
		{
			const auto data = static_cast<S*>(asset);
			if constexpr (requires { data->window.name; })
			{
				return const_cast<const char**>(&data->window.name);
			}
			else
			{
				return const_cast<const char**>(&data->name);
			}
		}

		template <typename Asset, typename S>
		constexpr asset_loader make_loader()
		{
			asset_loader loader{.size = sizeof(S), .name = asset_name_field<S>};
			if constexpr (requires(zone_reader& reader, S* asset) { Asset::read(reader, asset); })
			{
				loader.read = [](zone_reader& reader, void* asset)
				{
					Asset::read(reader, static_cast<S*>(asset));
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
				H1_ASSETS(ADD_LOADER)
				H1_READ_ONLY_ASSETS(ADD_LOADER)
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

		void read_script_strings(zone_reader& reader, ScriptStringList& list)
		{
			std::vector<std::uint32_t> ids(list.count);

			const auto strings = reader.read_array(list.strings, 7, list.count);
			if (strings)
			{
				for (auto i = 0; i < list.count; i++)
				{
					reader.read_string(strings[i]);
					ids[i] = SL_GetString(strings[i]);
				}
			}

			reader.set_script_strings(std::move(ids));
		}

		void read_zone_table(zone_reader& reader, GfxZoneTableEntry*& table, const unsigned int count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);

			if (const auto entries = reader.read_array(table, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_array(entries[i].dataPtr, 0, 1);
				}
			}

			reader.pop_stream();
		}

		XGfxGlobals* read_gfx_globals(zone_reader& reader, XGlobals*& globals)
		{
			if (!reader.read_single(globals, 3))
			{
				return nullptr;
			}

			const auto gfx_globals = reader.read_single(globals->gfxGlobals, 3);
			if (!gfx_globals)
			{
				return nullptr;
			}

			reader.read_array(gfx_globals->depthStencilStateBits, 3, gfx_globals->depthStencilStateCount);
			reader.read_array(gfx_globals->blendStateBits, 3, gfx_globals->blendStateCount);
			read_zone_table(reader, gfx_globals->depthStencilStates, gfx_globals->depthStencilStateCount);
			read_zone_table(reader, gfx_globals->blendStates, gfx_globals->blendStateCount);
			reader.read_array(gfx_globals->perPrimConstantBufferSizes, 3, gfx_globals->perPrimConstantBufferCount);
			reader.read_array(gfx_globals->perObjConstantBufferSizes, 3, gfx_globals->perObjConstantBufferCount);
			reader.read_array(gfx_globals->stableConstantBufferSizes, 3, gfx_globals->stableConstantBufferCount);
			read_zone_table(reader, gfx_globals->perPrimConstantBuffers, gfx_globals->perPrimConstantBufferCount);
			read_zone_table(reader, gfx_globals->perObjConstantBuffers, gfx_globals->perObjConstantBufferCount);
			read_zone_table(reader, gfx_globals->stableConstantBuffers, gfx_globals->stableConstantBufferCount);

			return gfx_globals;
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
					throw std::runtime_error(std::format("asset type {} has no reader", type));
				}

				const auto& loader = asset_loaders()[type];
				const auto data = this->reader_.read_bytes(loader.size);
				try
				{
					loader.read(this->reader_, data);
				}
				catch (const std::exception& e)
				{
					const auto name = *loader.name(data);
					throw std::runtime_error(std::format("{} \"{}\": {}", type_to_string(static_cast<XAssetType>(type)),
						name && this->reader_.contains(name) ? name : "", e.what()));
				}

				auto header = static_cast<void*>(this->reader_.allocate<std::uint8_t>(loader.size));
				std::memcpy(header, data, loader.size);

				const auto name = loader.name(header);
				const auto referenced = *name && (*name)[0] == ',';
				if (referenced)
				{
					++*name;
				}

				const auto asset_type = static_cast<XAssetType>(type);
				const auto existing = *name ? db_find_asset(type, *name) : nullptr;
				if (referenced && existing)
				{
					header = existing;
				}
				else
				{
					this->add_stream_files(asset_type, header);
					db_add_asset(type, *name ? *name : "", header);
				}

				this->zone_.assets.emplace_back(asset_type, header, referenced);

				*field = header;
				if (insert_slot)
				{
					*insert_slot = header;
				}
			}

		private:
			loaded_zone& zone_;
			zone_reader& reader_;
			std::size_t stream_file_index_{};

			void add_stream_files(const XAssetType type, const void* header)
			{
				if (type != ASSET_TYPE_IMAGE || !static_cast<const GfxImage*>(header)->streamed)
				{
					return;
				}

				const std::span stream_files = this->zone_.file.stream_files;
				if (this->stream_file_index_ + image_stream_file_count > stream_files.size())
				{
					throw std::runtime_error("streamed image has no stream files");
				}

				set_image_stream_files(header, stream_files.subspan(this->stream_file_index_, image_stream_file_count));
				this->stream_file_index_ += image_stream_file_count;
			}
		};
	}

	const char* get_asset_name(const XAssetType type, const void* header)
	{
		const auto& loader = asset_loaders()[type];
		return loader.name ? *loader.name(const_cast<void*>(header)) : "";
	}

	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path)
	{
		auto zone = std::make_unique<loaded_zone>();
		zone->name = path.stem().string();
		zone->file = xfile::read(path, definition().format);
		zone->reader = std::make_unique<zone_reader>(zone->file.payload, zone->file.block_sizes, layout);
		auto& reader = *zone->reader;
		reader.set_zone_index(next_zone_index());

		asset_list_reader assets(*zone);
		reader.set_inline_asset_loader([&](const std::int32_t type, void** field, void** insert_slot)
		{
			assets.read_inline_asset(type, field, insert_slot);
		});

		XAssetList list{};
		reader.read_raw(&list, sizeof(list));

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_script_strings(reader, list.stringList);
		if (const auto gfx_globals = read_gfx_globals(reader, list.globals))
		{
			insert_x_gfx_globals_for_zone(reader.zone_index(), gfx_globals);
		}
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		if (const auto list_assets = reader.read_array(list.assets, 3, list.assetCount))
		{
			for (auto i = 0; i < list.assetCount; i++)
			{
				reader.read_asset(list_assets[i].type, list_assets[i].header.data);
			}
		}
		reader.pop_stream();

		if (reader.remaining())
		{
			throw std::runtime_error(std::format("0x{:X} bytes remain after the last asset", reader.remaining()));
		}

		reader.set_inline_asset_loader({});
		return zone;
	}
}
