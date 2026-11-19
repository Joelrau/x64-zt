#include <std_include.hpp>
#include "iw8.hpp"
#include "loader.hpp"

#include <game.hpp>

namespace zonetool::iw8
{
	const game& definition();

	namespace
	{
		const zone_layout layout
		{
			.offset_tag = 0,
			.following_markers = {0xFFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFFE},
			.insert_marker = 0xFFFFFFFFFFFFFFFD,
			.runtime_block = XFILE_BLOCK_RUNTIME,
			.virtual_block = XFILE_BLOCK_VIRTUAL,
			.pop_aligned_block = XFILE_BLOCK_TEMP_PRELOAD,
			.temp_block = XFILE_BLOCK_TEMP,
			.asset_alignment = 7,
			.pop_alignment = 31,
			.asset_relative_tag = 1,
		};

		template <typename S>
		const char** asset_name_field(void* asset)
		{
			if constexpr (requires(S* s) { s->bank.name; })
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->bank.name);
			}
			else if constexpr (requires(S* s) { s->szInternalName; })
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->szInternalName);
			}
			else if constexpr (requires(S* s) { s->szName; })
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->szName);
			}
			else if constexpr (requires(S* s) { s->m_szName; })
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->m_szName);
			}
			else if constexpr (requires(S* s) { s->className; })
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->className);
			}
			else
			{
				return const_cast<const char**>(&static_cast<S*>(asset)->name);
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
				IW8_ASSETS(ADD_LOADER)
				IW8_READ_ONLY_ASSETS(ADD_LOADER)
#undef ADD_LOADER
				return list;
			}();

			return loaders;
		}

		bool is_unused_type(const XAssetType type)
		{
			constexpr std::array unused_types
			{
				ASSET_TYPE_IESPROFILE,
				ASSET_TYPE_UI_MAP,
				ASSET_TYPE_AITYPE,
				ASSET_TYPE_MPTYPE,
				ASSET_TYPE_CHARACTER,
				ASSET_TYPE_XMODELALIAS,
			};

			return std::ranges::contains(unused_types, type);
		}

		std::uint16_t next_zone_index()
		{
			static std::atomic_uint16_t index{};
			return ++index;
		}

		void read_script_strings(zone_reader& reader, ScriptStringList& list)
		{
			std::vector<std::uint32_t> ids(list.count);

			reader.push_asset();
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			if (const auto strings = reader.read_array(list.strings, 7, list.count))
			{
				for (auto i = 0; i < list.count; i++)
				{
					reader.read_string(strings[i]);
					ids[i] = strings[i] ? SL_GetString(strings[i]) : 0;
				}
			}
			reader.pop_stream();
			reader.pop_asset();

			reader.set_script_strings(std::move(ids));
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

				const auto& loader = asset_loaders()[type];
				this->reader_.align(asset_alignment(static_cast<XAssetType>(type)));
				this->reader_.push_asset();
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
				this->reader_.pop_asset();

				auto header = static_cast<void*>(this->reader_.allocate<std::uint8_t>(loader.size));
				std::memcpy(header, data, loader.size);

				const auto asset_type = static_cast<XAssetType>(type);
				const auto name = loader.name(header);
				const auto referenced = *name && (*name)[0] == ',';
				if (referenced)
				{
					++*name;
				}

				const auto existing = *name ? db_find_asset(type, *name) : nullptr;
				if (referenced && existing)
				{
					header = existing;
				}
				else
				{
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
		};

		void read_asset_list(zone_reader& reader)
		{
			XAssetList list{};
			reader.read_raw(&list, sizeof(list));

			read_script_strings(reader, list.stringList);

			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			if (const auto assets = reader.read_array(list.assets, 7, list.assetCount))
			{
				for (auto i = list.assetReadPos; i < list.assetCount; i++)
				{
					if (!is_unused_type(assets[i].type))
					{
						reader.read_asset(assets[i].type, assets[i].header.data);
					}
				}
			}
			reader.pop_stream();

			if (reader.remaining())
			{
				throw std::runtime_error(std::format("0x{:X} bytes remain after the last asset", reader.remaining()));
			}
		}
	}

	std::size_t asset_alignment(const XAssetType type)
	{
		switch (type)
		{
		case ASSET_TYPE_IMAGE:
		case ASSET_TYPE_VFX:
		case ASSET_TYPE_VECTORFIELD:
			return 15;
		case ASSET_TYPE_PARTICLE_SIM_ANIMATION:
			return 31;
		default:
			return layout.asset_alignment;
		}
	}

	const asset_loader& get_asset_loader(const XAssetType type)
	{
		return asset_loaders()[type];
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
		reader.mode = ::game::iw8;
		reader.set_zone_index(next_zone_index());

		asset_list_reader assets(*zone);
		reader.set_inline_asset_loader([&assets](const std::int32_t type, void** field, void** insert_slot)
		{
			assets.read_inline_asset(type, field, insert_slot);
		});
		read_asset_list(reader);
		reader.set_inline_asset_loader({});

		return zone;
	}
}
