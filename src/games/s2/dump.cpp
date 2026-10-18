#include <std_include.hpp>
#include "s2.hpp"
#include "zone/loader.hpp"

#include <game.hpp>
#include <io/output_cache.hpp>

namespace zonetool::s2
{
	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"physpreset",
			"sndphyspreset",
			"sndmusicset",
			"phys_collmap",
			"physwaterpreset",
			"phys_worldmap",
			"physconstraint",
			"xanim",
			"xsurfshared",
			"xmodelsurfs",
			"xmodel",
			"xmodelbase",
			"mayhem",
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
			"distcurve",
			"reverbsendcurve",
			"sndcontext",
			"aliasparametermodifier",
			"aliascombatcone",
			"loaded_sound",
			"col_map_mp",
			"com_map",
			"glass_map",
			"aipaths",
			"navmesh",
			"vehicle_track",
			"map_ents",
			"fx_map",
			"gfx_map",
			"gfx_map_trzone",
			"col_map_trzone",
			"iesprofile",
			"lightdef",
			"ui_map",
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
			"tracer",
			"vehicle",
			"addon_map_ents",
			"netconststrings",
			"reverbpreset",
			"luafile",
			"scriptable",
			"equipsndtable",
			"vectorfield",
			"particlesimanimation",
			"laser",
			"beam",
			"skeletonscript",
			"clut",
			"ttf",
			"splines",
			"physclothtuning",
			"dlogschema",
			"dlogroutes",
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
				S2_ASSETS(DUMP_ASSET)
#undef DUMP_ASSET
			default:
				break;
			}
		}

		std::string asset_csv(const loaded_zone& zone)
		{
			std::string csv;
			for (const auto& asset : zone.assets)
			{
				if (asset.type == ASSET_TYPE_XSURFACE_SHARED)
				{
					continue;
				}

				csv += std::format("{},{}{}\n", type_to_string(asset.type),
					asset.referenced ? "," : "", get_asset_name(asset.type, asset.header));
			}

			return csv;
		}

		void write_asset_csv(const loaded_zone& zone)
		{
			filesystem::file csv(zone.name + ".csv");
			csv.open("wb");
			csv.write(asset_csv(zone));
			csv.close();
		}

		bool is_transient_zone_of(const loaded_zone& transient_zone, const std::string& zone_name)
		{
			const auto prefix = zone_name + "_";
			for (const auto& asset : transient_zone.assets)
			{
				if (asset.type != ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE)
				{
					continue;
				}

				const std::string_view name = get_asset_name(asset.type, asset.header);
				if (name.starts_with(prefix) && name.size() > prefix.size()
					&& std::ranges::all_of(name.substr(prefix.size()), [](const char c) { return std::isdigit(static_cast<unsigned char>(c)) != 0; }))
				{
					return true;
				}
			}

			return false;
		}

		std::vector<std::unique_ptr<loaded_zone>> load_transient_zones(const std::filesystem::path& zone_path)
		{
			std::vector<std::unique_ptr<loaded_zone>> zones;
			for (const auto& entry : std::filesystem::directory_iterator(zone_path.parent_path()))
			{
				const auto name = entry.path().filename().string();
				if (!entry.is_regular_file() || !name.ends_with("_tr.ff"))
				{
					continue;
				}

				try
				{
					zones.emplace_back(load_zone(entry.path()));
				}
				catch (const std::exception& e)
				{
					ZONETOOL_ERROR("Failed to load transient zone \"%s\": %s", name.data(), e.what());
				}
			}

			return zones;
		}

		void dump_assets(const loaded_zone& zone, const converter* converter)
		{
			for (const auto& asset : zone.assets)
			{
				if (asset.referenced)
				{
					continue;
				}

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
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
		{
			filesystem::set_zone_folder(zone_folder);
			filesystem::set_fastfile(zone_path.stem().string());
			filesystem::set_language("english", "eng");
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

		prepare_paths(zone_folder, zone_path);
		const dump_cache_scope cache(filesystem::get_dump_path());
		ZONETOOL_INFO("Loading zone \"%s\"...", zone_path.stem().string().data());
		const auto zone = load_zone(zone_path);

		const auto transient_zones = converter ? load_transient_zones(zone_path) : decltype(load_transient_zones(zone_path)){};

		ZONETOOL_INFO("Dumping %zu assets...", zone->assets.size());
		dump_assets(*zone, converter);
		for (const auto& transient_zone : transient_zones)
		{
			if (!is_transient_zone_of(*transient_zone, zone->name))
			{
				continue;
			}

			ZONETOOL_INFO("Dumping %zu assets of transient zone \"%s\"...", transient_zone->assets.size(), transient_zone->name.data());
			dump_assets(*transient_zone, converter);
		}

		write_asset_csv(*zone);
		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}
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

		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		const auto zone = load_zone(zone_path);

		const auto asset = std::ranges::find_if(zone->assets, [&](const loaded_asset& entry)
		{
			return entry.type == type && !entry.referenced && name == get_asset_name(entry.type, entry.header);
		});

		if (asset == zone->assets.end())
		{
			throw std::runtime_error(std::format("{} \"{}\" is not in zone \"{}\"", type_name, name, zone->name));
		}

		filesystem::set_dump_folder("assets");
		if (converter)
		{
			converter->dump_asset(asset->type, asset->header);
			if (converter->finish)
			{
				converter->finish(zone->name);
			}
		}
		else
		{
			dump_asset(*asset);
		}

		ZONETOOL_INFO("%s \"%s\" dumped to %s", type_name.data(), name.data(), filesystem::get_dump_path().data());
	}

	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		load_zone(zone_path);
	}

	std::string list_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		return asset_csv(*load_zone(zone_path));
	}
}
