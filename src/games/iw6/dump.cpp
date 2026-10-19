#include <std_include.hpp>
#include "iw6.hpp"
#include "zone/loader.hpp"

#include <game.hpp>

namespace zonetool::iw6
{
	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"physpreset",
			"phys_collmap",
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
			"sndcurve",
			"lpfcurve",
			"reverbsendcurve",
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
			"font",
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
			"structureddatadef",
			"tracer",
			"vehicle",
			"addon_map_ents",
			"netconststrings",
			"reverbpreset",
			"luafile",
			"scriptable",
			"colorization",
			"colorizationset",
			"tonemapping",
			"equipsndtable",
			"vectorfield",
			"dopplerpreset",
			"particlesimanimation",
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
				IW6_ASSETS(DUMP_ASSET)
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

	void dump_zone(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, const converter* converter)
	{
		const auto _ = gsl::finally(db_clear);

		prepare_paths(zone_folder, zone_path);
		const auto required_zones = load_required_zones(converter, zone_path);
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
		const auto required_zones = load_required_zones(converter, zone_path);
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
}
