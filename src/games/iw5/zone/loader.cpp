#include <std_include.hpp>
#include "iw5.hpp"
#include "loader.hpp"

#include <game.hpp>

namespace zonetool::iw5
{
	const game& definition();

	namespace
	{
		const zone_layout32 layout
		{
			.temp_block = XFILE_BLOCK_TEMP,
			.virtual_block = XFILE_BLOCK_VIRTUAL,
			.runtime_blocks = {XFILE_BLOCK_RUNTIME},
			.asset_alignment = 3,
		};

		template <typename S>
		x86::pointer<const char>* asset_name_field(void* asset)
		{
			const auto data = static_cast<S*>(asset);
			if constexpr (requires { data->name; })
			{
				return &data->name;
			}
			else if constexpr (requires { data->info.name; })
			{
				return &data->info.name;
			}
			else if constexpr (requires { data->window.name; })
			{
				return &data->window.name;
			}
			else if constexpr (requires { data->aliasName; })
			{
				return &data->aliasName;
			}
			else if constexpr (requires { data->fontName; })
			{
				return &data->fontName;
			}
			else if constexpr (requires { data->szInternalName; })
			{
				return &data->szInternalName;
			}
			else if constexpr (requires { data->filename; })
			{
				return &data->filename;
			}
			else
			{
				static_assert(sizeof(S) == 0, "asset has no known name field");
			}
		}

		template <typename Asset, typename S>
		constexpr asset_loader make_loader()
		{
			return asset_loader
			{
				.size = sizeof(S),
				.read = [](zone_reader32& reader, void* asset)
				{
					Asset::read(reader, static_cast<S*>(asset));
				},
				.name = asset_name_field<S>,
			};
		}

		const std::array<asset_loader, ASSET_TYPE_COUNT>& asset_loaders()
		{
			static const auto loaders = []
			{
				std::array<asset_loader, ASSET_TYPE_COUNT> list{};
#define ADD_LOADER(type, asset, struct_name) list[type] = make_loader<asset, struct_name>();
				IW5_READ_ONLY_ASSETS(ADD_LOADER)
#undef ADD_LOADER
				return list;
			}();

			return loaders;
		}

		void read_script_strings(zone_reader32& reader, ScriptStringList& list)
		{
			std::vector<std::uint32_t> ids(list.count);

			if (const auto strings = reader.read_array(list.strings, 3, list.count))
			{
				for (auto i = 0; i < list.count; i++)
				{
					reader.read_string(strings[i]);
					ids[i] = SL_GetString(strings[i].get());
				}
			}

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

			void read_inline_asset(const std::int32_t type, x86::pointer<void>& field, std::uint32_t* insert_slot)
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
					const auto name = loader.name(data)->get();
					throw std::runtime_error(std::format("{} \"{}\": {}", type_to_string(static_cast<XAssetType>(type)),
						name && this->reader_.contains(name) ? name : "", e.what()));
				}

				const auto header = this->reader_.duplicate(static_cast<const std::uint8_t*>(data), loader.size);
				auto& name = *loader.name(header);
				const auto referenced = name.get() && name.get()[0] == ',';
				if (referenced)
				{
					name.value++;
				}

				const auto asset_type = static_cast<XAssetType>(type);
				const auto text = name.get() ? name.get() : "";
				const auto existing = *text ? db_find_asset(type, text) : nullptr;
				void* stored = header;
				if (referenced && existing)
				{
					stored = existing;
				}
				else
				{
					db_add_asset(type, text, header);
				}

				this->zone_.assets.emplace_back(asset_type, stored, referenced);

				field = static_cast<std::uint8_t*>(stored);
				if (insert_slot)
				{
					*insert_slot = field.value;
				}
			}

		private:
			loaded_zone& zone_;
			zone_reader32& reader_;
		};
	}

	const char* get_asset_name(const XAssetType type, const void* header)
	{
		const auto& loader = asset_loaders()[type];
		const auto name = loader.name ? loader.name(const_cast<void*>(header))->get() : nullptr;
		return name ? name : "";
	}

	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path)
	{
		auto zone = std::make_unique<loaded_zone>();
		zone->name = path.stem().string();
		zone->file = xfile::read(path, definition().format);
		zone->reader = std::make_unique<zone_reader32>(zone->file.payload, zone->file.block_sizes, layout);
		auto& reader = *zone->reader;

		asset_list_reader assets(*zone);
		reader.set_inline_asset_loader([&](const std::int32_t type, x86::pointer<void>& field, std::uint32_t* insert_slot)
		{
			assets.read_inline_asset(type, field, insert_slot);
		});

		XAssetList list{};
		reader.read_raw(&list, sizeof(list));

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		read_script_strings(reader, list.stringList);
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		if (const auto list_assets = reader.read_array(list.assets, 3, list.assetCount))
		{
			for (auto i = 0; i < list.assetCount; i++)
			{
				if (list_assets[i].type == ASSET_TYPE_SNDDRIVER_GLOBALS)
				{
					continue; // the header is an address inside the game executable, no data follows
				}

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
