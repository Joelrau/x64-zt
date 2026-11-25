#include <std_include.hpp>
#include "iw3.hpp"
#include <formats/iwd.hpp>
#include "zone/loader.hpp"

#include <game.hpp>
#include <io/game_files.hpp>
#include <io/output_cache.hpp>

namespace zonetool::iw3
{
	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"xmodelpieces",
			"physpreset",
			"xanim",
			"xmodel",
			"material",
			"techset",
			"image",
			"sound",
			"sndcurve",
			"loaded_sound",
			"col_map_sp",
			"col_map_mp",
			"com_map",
			"game_map_sp",
			"game_map_mp",
			"map_ents",
			"gfx_map",
			"lightdef",
			"ui_map",
			"font",
			"menufile",
			"menu",
			"localize",
			"weapon",
			"snddriverglobals",
			"fx",
			"impactfx",
			"aitype",
			"mptype",
			"character",
			"xmodelalias",
			"rawfile",
			"stringtable",
		};

		static_assert(std::size(asset_type_names) == ASSET_TYPE_COUNT);

		std::string asset_csv(const std::vector<loaded_asset>& assets)
		{
			std::string text;
			for (const auto& asset : assets)
			{
				text += std::format("{},{}{}\n", type_to_string(asset.type),
					asset.referenced ? "," : "", get_asset_name(asset.type, asset.header));
			}

			return text;
		}

		template <typename Asset, typename S>
		void dump_if_supported(S* header)
		{
			if constexpr (requires { Asset::dump(header); })
			{
				Asset::dump(header);
			}
		}

		void dump_native(const loaded_asset& asset)
		{
			switch (asset.type)
			{
#define DUMP_ASSET(type, asset_class, struct_name) 			case type: 				dump_if_supported<asset_class>(static_cast<struct_name*>(asset.header)); 				break;
				IW3_READ_ONLY_ASSETS(DUMP_ASSET)
#undef DUMP_ASSET
			default:
				break;
			}
		}

		void dump_loaded(const loaded_asset& asset, const converter* converter)
		{
			if (converter)
			{
				converter->dump_asset(asset.type, asset.header);
			}
			else
			{
				dump_native(asset);
			}
		}

		void write_asset_csv(const loaded_zone& zone)
		{
			filesystem::file csv(zone.name + ".csv");
			csv.open("wb");
			csv.write(asset_csv(zone.assets));
			csv.close();
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::string& fastfile)
		{
			filesystem::set_zone_folder(zone_folder);
			auto game_root = zone_folder;
			while (game_root.has_parent_path() && !std::filesystem::exists(game_root / "main"))
			{
				game_root = game_root.parent_path();
			}
			iwd::set_root(game_root);
			filesystem::set_fastfile(fastfile);
			filesystem::set_language("english", "eng");
		}

		std::vector<std::unique_ptr<loaded_zone>> load_required_zones(const converter* converter, const std::filesystem::path& zone_path)
		{
			std::vector<std::unique_ptr<loaded_zone>> zones;
			if (!converter || !converter->required_zones)
			{
				return zones;
			}

			const auto zone_name = zone_path.stem().string();
			for (const auto& name : converter->required_zones(zone_name))
			{
				if (name == zone_name)
				{
					continue;
				}

				const auto path = game_files::find_zone(name);
				if (!path)
				{
					ZONETOOL_WARNING("Required zone \"%s\" not found", name.data());
					continue;
				}

				ZONETOOL_INFO("Loading required zone \"%s\"...", name.data());
				zones.emplace_back(load_zone(*path));
			}

			return zones;
		}

		void dump_required_asset(const XAssetType type, void* header, const std::unordered_set<const void*>& required_headers,
			std::unordered_set<const void*>& dumped, const converter* converter)
		{
			if (!header || !required_headers.contains(header) || !dumped.insert(header).second)
			{
				return;
			}

			try
			{
				converter->dump_asset(type, header);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to dump %s \"%s\": %s", type_to_string(type), get_asset_name(type, header), e.what());
			}

			if (type == ASSET_TYPE_MATERIAL)
			{
				const auto material = static_cast<Material*>(header);
				const auto textures = material->textureTable.get();
				for (auto i = 0; textures && i < material->textureCount; i++)
				{
					if (textures[i].semantic != TS_WATER_MAP)
					{
						dump_required_asset(ASSET_TYPE_IMAGE, textures[i].u.image.get(), required_headers, dumped, converter);
					}
				}
			}
			else if (type == ASSET_TYPE_XMODEL)
			{
				const auto model = static_cast<XModel*>(header);
				const auto materials = model->materialHandles.get();
				for (auto i = 0; materials && i < model->numsurfs; i++)
				{
					dump_required_asset(ASSET_TYPE_MATERIAL, materials[i].get(), required_headers, dumped, converter);
				}
			}
		}

		void dump_required_references(const loaded_zone& zone, const std::vector<std::unique_ptr<loaded_zone>>& required_zones,
			const converter* converter)
		{
			std::unordered_set<const void*> required_headers;
			for (const auto& required_zone : required_zones)
			{
				for (const auto& asset : required_zone->assets)
				{
					if (!asset.referenced)
					{
						required_headers.insert(asset.header);
					}
				}
			}

			std::unordered_set<const void*> dumped;
			for (const auto& asset : zone.assets)
			{
				if (asset.referenced && (asset.type == ASSET_TYPE_MATERIAL || asset.type == ASSET_TYPE_XMODEL || asset.type == ASSET_TYPE_IMAGE))
				{
					dump_required_asset(asset.type, asset.header, required_headers, dumped, converter);
				}
			}
		}

		void clear_loaded_data()
		{
			db_clear();
			x86::address_space::reset();
		}
	}

	const char* type_to_string(const XAssetType type)
	{
		return asset_type_names[type];
	}

	std::int32_t type_to_int(const std::string& type)
	{
		for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			if (asset_type_names[i] == type)
			{
				return i;
			}
		}

		return -1;
	}

	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter)
	{
		const auto _ = gsl::finally(clear_loaded_data);

		prepare_paths(zone_folder, zone_path.stem().string());
		const dump_cache_scope cache(filesystem::get_dump_path());
		const auto required_zones = load_required_zones(converter, zone_path);
		ZONETOOL_INFO("Loading zone \"%s\"...", zone_path.stem().string().data());
		const auto zone = load_zone(zone_path);

		ZONETOOL_INFO("Dumping %zu assets...", zone->assets.size());
		for (const auto& asset : zone->assets)
		{
			if (asset.referenced)
			{
				continue;
			}

			try
			{
				dump_loaded(asset, converter);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to dump %s \"%s\": %s", type_to_string(asset.type),
					get_asset_name(asset.type, asset.header), e.what());
			}
		}

		dump_required_references(*zone, required_zones, converter);
		write_asset_csv(*zone);
		ZONETOOL_INFO("Zone \"%s\" dumped to %s", zone->name.data(), filesystem::get_dump_path().data());
	}

	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type_name,
		const std::string& name, const converter* converter)
	{
		const auto type = type_to_int(type_name);
		if (type < 0)
		{
			throw std::runtime_error(std::format("unknown asset type \"{}\"", type_name));
		}

		const auto _ = gsl::finally(clear_loaded_data);

		prepare_paths(zone_folder, zone_path.stem().string());
		const dump_cache_scope cache(filesystem::get_dump_path());
		const auto zone = load_zone(zone_path);

		filesystem::set_dump_folder("assets");
		auto found = false;
		for (const auto& asset : zone->assets)
		{
			if (asset.type == type && !asset.referenced && (name == "*" || name == get_asset_name(asset.type, asset.header)))
			{
				dump_loaded(asset, converter);
				found = true;
			}
		}

		if (!found)
		{
			throw std::runtime_error(std::format("{} \"{}\" is not in zone \"{}\"", type_name, name, zone->name));
		}

		ZONETOOL_INFO("%s \"%s\" dumped to %s", type_name.data(), name.data(), filesystem::get_dump_path().data());
	}

	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(clear_loaded_data);

		prepare_paths(zone_folder, zone_path.stem().string());
		load_zone(zone_path);
	}

	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(clear_loaded_data);

		prepare_paths(zone_folder, zone_path.stem().string());
		return asset_csv(load_zone(zone_path)->assets);
	}
}
