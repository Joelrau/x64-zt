#include <std_include.hpp>
#include "h1.hpp"
#include "asset_list.hpp"
#include "zone/loader.hpp"

#include <game.hpp>

namespace zonetool::h1
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
			"col_map_mp",
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

		void dump_asset(const loaded_asset& asset)
		{
			switch (asset.type)
			{
#define DUMP_ASSET(type, asset_class, struct_name) \
			case type: \
				asset_class::dump(static_cast<struct_name*>(asset.header)); \
				break;
				H1_ASSETS(DUMP_ASSET)
#undef DUMP_ASSET
			default:
				break;
			}
		}

		bool is_dumped_type(const XAssetType type)
		{
			return type != ASSET_TYPE_VERTEXDECL;
		}

		void write_asset_csv(const loaded_zone& zone)
		{
			filesystem::file csv(zone.name + ".csv");
			csv.open("wb");

			for (const auto& asset : zone.assets)
			{
				csv.write(std::format("{},{}{}\n", type_to_string(asset.type),
					asset.referenced ? "," : "", get_asset_name(asset.type, asset.header)));
			}

			csv.close();
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
		{
			filesystem::set_zone_folder(zone_folder);
			filesystem::set_fastfile(zone_path.stem().string());
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

	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const asset_dumper converter)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		ZONETOOL_INFO("Loading zone \"%s\"...", zone_path.stem().string().data());
		const auto zone = load_zone(zone_path);

		ZONETOOL_INFO("Dumping %zu assets...", zone->assets.size());
		for (const auto& asset : zone->assets)
		{
			if (asset.referenced || !is_dumped_type(asset.type))
			{
				continue;
			}

			try
			{
				if (converter)
				{
					converter(asset.type, asset.header);
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

		write_asset_csv(*zone);
		ZONETOOL_INFO("Zone \"%s\" dumped to %s", zone->name.data(), filesystem::get_dump_path().data());
	}

	void verify_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		load_zone(zone_path);
	}
}
