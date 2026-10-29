#include <std_include.hpp>
#include "h2.hpp"
#include "zone/loader.hpp"

#include <game.hpp>

namespace zonetool::h2
{
	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"physpreset",
			"phys_collmap",
			"physwaterpreset",
			"phys_worldmap",
			"physconstraint",
			"xanim",
			"xmodelsurfs",
			"xmodel",
			"material",
			"computeshader",
			"vertexshader",
			"hullshader",
			"domainshader",
			"pixelshader",
			"vertexdecl",
			"techset",
			"image",
			"sound",
			"soundsubmix",
			"sndcurve",
			"lpfcurve",
			"reverbsendcurve",
			"sndcontext",
			"loaded_sound",
			"col_map_sp",
			"com_map",
			"glass_map",
			"aipaths",
			"vehicle_track",
			"map_ents",
			"fx_map",
			"gfx_map",
			"lightdef",
			"ui_map",
			"menufile",
			"menu",
			"animclass",
			"localize",
			"attachment",
			"weapon",
			"snddriverglobals",
			"fx",
			"impactfx",
			"surfacefx",
			"aitype",
			"mptype",
			"character",
			"xmodelalias",
			"rawfile",
			"scriptfile",
			"stringtable",
			"leaderboarddef",
			"virtualleaderboarddef",
			"structureddatadef",
			"ddl",
			"proto",
			"tracer",
			"vehicle",
			"addon_map_ents",
			"netconststrings",
			"reverbpreset",
			"luafile",
			"scriptable",
			"equipsndtable",
			"vectorfield",
			"dopplerpreset",
			"particlesimanimation",
			"laser",
			"skeletonscript",
			"clut",
			"ttf",
		};

		static_assert(std::size(asset_type_names) == ASSET_TYPE_COUNT);

		template <typename asset_class, typename struct_name>
		void dump_if_supported(struct_name* header)
		{
			if constexpr (requires { asset_class::dump(header); })
			{
				asset_class::dump(header);
			}
		}

		void dump_asset(const loaded_asset& asset)
		{
			switch (asset.type)
			{
#define DUMP_ASSET(type, asset_class, struct_name) \
			case type: \
				dump_if_supported<asset_class>(static_cast<struct_name*>(asset.header)); \
				break;
				H2_ASSETS(DUMP_ASSET)
				H2_READ_ONLY_ASSETS(DUMP_ASSET)
#undef DUMP_ASSET
			default:
				break;
			}
		}

		bool is_dumped_type(const XAssetType type)
		{
			return type != ASSET_TYPE_VERTEXDECL;
		}

		enum class map_zone_dump
		{
			none,
			referenced,
			all,
		};

		struct map_zone
		{
			std::string name;
			map_zone_dump dump;
		};

		struct map_asset_list
		{
			std::vector<loaded_asset> assets;
			std::unordered_map<std::pair<std::uint32_t, std::string>, std::size_t, pair_hash<std::uint32_t, std::string>> indices;

			void add(const loaded_asset& asset)
			{
				const auto [entry, inserted] = this->indices.try_emplace({asset.type, get_asset_name(asset.type, asset.header)},
					this->assets.size());
				if (inserted)
				{
					this->assets.emplace_back(asset);
				}
				else if (!asset.referenced)
				{
					this->assets[entry->second] = asset;
				}
			}
		};

		std::vector<map_zone> map_zones(const std::string& map)
		{
			return
			{
				{"code_post_gfx", map_zone_dump::referenced},
				{"common", map_zone_dump::referenced},
				{"techsets_common", map_zone_dump::referenced},
				{"techsets_" + map, map_zone_dump::all},
				{"eng_patch_" + map, map_zone_dump::all},
				{"patch_" + map, map_zone_dump::all},
				{map, map_zone_dump::all},
			};
		}

		// the loader points a reference at the header of the zone that holds the asset
		std::unordered_set<const void*> collect_map_references(const std::vector<std::pair<std::unique_ptr<loaded_zone>, map_zone_dump>>& zones)
		{
			std::unordered_set<const void*> references;
			for (const auto& [zone, dump] : zones)
			{
				for (const auto& asset : zone->assets)
				{
					if (dump == map_zone_dump::all && asset.referenced)
					{
						references.insert(asset.header);
					}
				}
			}

			return references;
		}

		bool is_map_zone_asset(const loaded_asset& asset, const map_zone_dump dump, const std::unordered_set<const void*>& references)
		{
			if (asset.referenced || !is_dumped_type(asset.type))
			{
				return false;
			}

			return dump == map_zone_dump::all || (dump == map_zone_dump::referenced && references.contains(asset.header));
		}

		void dump_zone_asset(const loaded_asset& asset, const converter* converter)
		{
			try
			{
				if (converter)
				{
					converter->dump_asset(asset.type, asset.header);
				}
				else
				{
					dump_asset(asset);
				}
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to dump %s \"%s\": %s", type_to_string(asset.type),
					get_asset_name(asset.type, asset.header), e.what());
			}
		}

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

		// a converter may replace <name>.csv with its build list in finish, the source listing stays in <name>.<source>.csv
		void write_asset_csv(const std::string& name, const std::vector<loaded_asset>& assets, const converter* converter)
		{
			const auto text = asset_csv(assets);

			const auto write = [&](const std::string& file_name)
			{
				filesystem::file csv(file_name);
				csv.open("wb");
				csv.write(text);
				csv.close();
			};

			write(name + ".csv");
			if (converter)
			{
				write(std::format("{}.{}.csv", name, converter->source));
			}
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::string& fastfile)
		{
			filesystem::set_zone_folder(zone_folder);
			filesystem::set_fastfile(fastfile);
			filesystem::set_language("english", "eng");
		}
	}

	const char* type_to_string(const XAssetType type)
	{
		return asset_type_names[type];
	}

	std::int32_t type_to_int(const std::string& type)
	{
		if (type == "col_map_sp")
		{
			return ASSET_TYPE_CLIPMAP;
		}

		for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			if (asset_type_names[i] == type)
			{
				return i;
			}
		}

		return -1;
	}

	bool is_valid_asset_type(const std::string& type)
	{
		return type_to_int(type) >= 0;
	}

	XAssetHeader db_find_x_asset_header(const XAssetType type, const char* name, const int create_default)
	{
		return zonetool::db_find_x_asset_header<XAssetHeader>(type, name, create_default);
	}

	XAssetHeader db_find_x_asset_header_safe(const XAssetType type, const std::string& name)
	{
		return zonetool::db_find_x_asset_header_safe<XAssetHeader, XAssetEntry>(type, name);
	}

	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		ZONETOOL_INFO("Loading zone \"%s\"...", zone_path.stem().string().data());
		const auto zone = load_zone(zone_path);

		ZONETOOL_INFO("Dumping %zu assets...", zone->assets.size());
		for (const auto& asset : zone->assets)
		{
			if (asset.referenced || !is_dumped_type(asset.type))
			{
				continue;
			}

			dump_zone_asset(asset, converter);
		}

		write_asset_csv(zone->name, zone->assets, converter);
		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}
		ZONETOOL_INFO("Zone \"%s\" dumped to %s", zone->name.data(), filesystem::get_dump_path().data());
	}

	void dump_map(const std::filesystem::path& zone_folder, const std::string& map, const converter* converter, const bool skip_common)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, map);

		std::vector<std::pair<std::unique_ptr<loaded_zone>, map_zone_dump>> zones;
		for (const auto& [name, dump] : map_zones(map))
		{
			const auto path = game_files::find_zone(name);
			if (!path)
			{
				ZONETOOL_INFO("Zone \"%s\" not found, skipping", name.data());
				continue;
			}

			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());
			zones.emplace_back(load_zone(*path), skip_common && dump != map_zone_dump::all ? map_zone_dump::none : dump);
		}

		const auto references = collect_map_references(zones);
		map_asset_list map_assets;
		for (const auto& [zone, dump] : zones)
		{
			ZONETOOL_INFO("Dumping zone \"%s\"...", zone->name.data());
			for (const auto& asset : zone->assets)
			{
				const auto dumped = is_map_zone_asset(asset, dump, references);
				if (dumped)
				{
					dump_zone_asset(asset, converter);
				}

				if (dumped || dump == map_zone_dump::all)
				{
					map_assets.add(asset);
				}
			}
		}

		write_asset_csv(map, map_assets.assets, converter);
		if (converter && converter->finish)
		{
			converter->finish(map);
		}
		ZONETOOL_INFO("Map \"%s\" dumped to %s", map.data(), filesystem::get_dump_path().data());
	}

	void dump_asset(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const std::string& type_name,
		const std::string& name, const converter* converter)
	{
		const auto type = type_to_int(type_name);
		if (type < 0)
		{
			throw std::runtime_error(std::format("unknown asset type \"{}\"", type_name));
		}

		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		const auto zone = load_zone(zone_path);

		std::vector<const loaded_asset*> assets;
		for (const auto& entry : zone->assets)
		{
			if (entry.type == type && !entry.referenced && (name == "*" || name == get_asset_name(entry.type, entry.header)))
			{
				assets.emplace_back(&entry);
			}
		}

		if (assets.empty())
		{
			throw std::runtime_error(std::format("{} \"{}\" is not in zone \"{}\"", type_name, name, zone->name));
		}

		filesystem::set_dump_folder("assets");
		for (const auto* asset : assets)
		{
			if (converter)
			{
				converter->dump_asset(asset->type, asset->header);
			}
			else
			{
				dump_asset(*asset);
			}
		}

		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}

		ZONETOOL_INFO("%s \"%s\" dumped to %s", type_name.data(), name.data(), filesystem::get_dump_path().data());
	}

	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		load_zone(zone_path);
	}

	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path.stem().string());
		return asset_csv(load_zone(zone_path)->assets);
	}
}
