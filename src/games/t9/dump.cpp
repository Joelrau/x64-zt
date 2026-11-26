#include <std_include.hpp>
#include "t9.hpp"
#include "zone/loader.hpp"

#include <game.hpp>
#include <io/game_files.hpp>
#include <io/output_cache.hpp>

namespace zonetool::t9
{
	namespace
	{
		constexpr const char* asset_type_names[]
		{
			"zone",
			"assetlist",
			"physpreset",
			"physconstraints",
			"destructibledef",
			"xanim",
			"xmodel",
			"xcollision",
			"xskeleton",
			"xmodelmesh",
			"material",
			"csdef",
			"computeshaderset",
			"rtsdef",
			"raytraceshaderset",
			"techset",
			"image",
			"sound",
			"sound_bank",
			"sound_asset",
			"sound_duck",
			"sound_alias_modifier",
			"sound_acoustics",
			"col_map",
			"clip_map",
			"com_map",
			"game_map",
			"gfx_map",
			"fonticon",
			"localizeentry",
			"gesture",
			"gesturetable",
			"cinematicmotion",
			"weapon",
			"weaponfull",
			"weaponfrontend",
			"weaponblueprint",
			"weaponstylesettings",
			"weaponsecondarymovement",
			"weapontunables",
			"cgmediatable",
			"playersoundstable",
			"playerfxtable",
			"sharedweaponsounds",
			"attachment",
			"attachmentunique",
			"weaponcamo",
			"weaponcamobinding",
			"customizationtable",
			"customizationtablefrontend",
			"snddriverglobals",
			"fx",
			"tagfx",
			"klf",
			"impactsfxtable",
			"impactsoundstable",
			"aitype",
			"character",
			"xmodelalias",
			"rawfile",
			"rawfilepreproc",
			"rawtextfile",
			"animtree",
			"stringtable",
			"structuredtable",
			"leaderboarddef",
			"ddl",
			"glasses",
			"scriptparsetree",
			"scriptparsetreedbg",
			"script_using",
			"script_using_cp",
			"script_using_mp",
			"script_using_wz",
			"script_using_zm",
			"keyvaluepairs",
			"vehicle",
			"tracer",
			"surfacefxtable",
			"surfacesounddef",
			"footsteptable",
			"entityfximpacts",
			"entitysoundimpacts",
			"zbarrier",
			"vehiclefxdef",
			"vehiclesounddef",
			"typeinfo",
			"scriptbundle",
			"scriptbundlelist",
			"rumble",
			"bulletpenetration",
			"locdmgtable",
			"aimtable",
			"shoottable",
			"playerglobaltunables",
			"overheadcameratunables",
			"animselectortable",
			"animmappingtable",
			"animstatemachine",
			"behaviortree",
			"behaviorstatemachine",
			"ttf",
			"sanim",
			"shellshock",
			"statuseffect",
			"cinematic_camera",
			"cinematic_sequence",
			"spectate_camera",
			"xcam",
			"bgcache",
			"flametable",
			"bitfield",
			"maptable",
			"maptableentry",
			"maptablelist",
			"objective",
			"objectivelist",
			"navmesh",
			"navvolume",
			"laser",
			"beam",
			"streamerhint",
			"flowgraph",
			"postfxbundle",
			"luafile",
			"luafiledebug",
			"renderoverridebundle",
			"staticlevelfxlist",
			"triggerlist",
			"playerroletemplate",
			"playerroletemplatefrontend",
			"playerrolecategorytable",
			"playerrolecategory",
			"characterbodytype",
			"characterbodytypefrontend",
			"playeroutfit",
			"gametypetable",
			"gametypetableentry",
			"feature",
			"featuretable",
			"unlockableitem",
			"unlockableitemtable",
			"entitylist",
			"playlists",
			"playlistglobalsettings",
			"playlistschedule",
			"motionmatchinginput",
			"blackboard",
			"tacticalquery",
			"playermovementtunables",
			"hierarchicaltasknetwork",
			"ragdoll",
			"storagefile",
			"storagefilelist",
			"charmixer",
			"storeproduct",
			"storecategory",
			"storecategorylist",
			"rank",
			"ranktable",
			"prestige",
			"prestigetable",
			"firstpartyentitlement",
			"firstpartyentitlementlist",
			"entitlement",
			"entitlementlist",
			"sku",
			"labelstore",
			"labelstorelist",
			"cpu_occlusion_data",
			"lighting",
			"districts",
			"streamerworld",
			"talent",
			"playertalenttemplate",
			"playeranimation",
			"unused",
			"terraingfx",
			"highlightreelinfodefines",
			"highlightreelprofileweighting",
			"highlightreelstarlevels",
			"dlogevent",
			"rawstring",
			"ballisticdesc",
			"streamkey",
			"rendertargets",
			"drawnodes",
			"grouplodmodel",
			"fxlibraryvolume",
			"arenaseasons",
			"sprayorgestureitem",
			"sprayorgesturelist",
			"hwplatform",
			"attachmenttable",
			"navinput",
			"uimodeldatastruct",
			"crafticon",
			"crafticonlist",
			"craftweaponsticker",
			"craftweaponstickerlist",
			"craftbackground",
			"craftbackgroundlist",
			"craftmaterial",
			"craftmateriallist",
			"craftcategory",
			"craftcategorylist",
			"craftweaponicontransform",
			"craftweaponicontransformlist",
			"xanimcurve",
			"dynmodel",
			"vectorfield",
			"winddef",
			"vehicleassembly",
			"milestone",
			"milestonetable",
			"triggereffectdesc",
			"triggeractions",
			"playersettings",
			"compasstunables",
			"execution",
			"scenario",
		};

		static_assert(std::size(asset_type_names) == ASSET_TYPE_COUNT);

		std::string asset_csv(const std::vector<loaded_asset>& assets)
		{
			std::string text;
			for (const auto& asset : assets)
			{
				text += std::format("{},{}\n", type_to_string(asset.type), get_asset_name(asset.type, asset.header));
			}

			return text;
		}

		void dump_loaded(const loaded_asset& asset, const converter* converter)
		{
			if (converter)
			{
				converter->dump_asset(asset.type, asset.header);
			}
			else if (const auto dump = get_asset_loader(asset.type).dump)
			{
				dump(asset.header);
			}
		}

		void write_text_file(const std::string& path, const std::string& text)
		{
			filesystem::file file(path);
			file.open("wb");
			file.write(text);
			file.close();
		}

		std::unordered_set<const void*> get_list_headers(const loaded_zone& zone)
		{
			std::unordered_set<const void*> headers;
			for (auto i = 0; i < zone.list.assetCount; i++)
			{
				headers.insert(zone.list.assets[i].header.data);
			}

			return headers;
		}

		void write_binary_asset(const loaded_zone& zone, const XAsset& asset, const std::unordered_set<const void*>& list_headers,
			const std::string& name)
		{
			if (asset.type == ASSET_TYPE_KEYVALUEPAIRS)
			{
				return;
			}

			try
			{
				write_text_file(get_asset_file_path(asset.type, name), write_asset_file(zone, asset, list_headers));
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to write the asset file of %s \"%s\": %s", type_to_string(asset.type), name.data(), e.what());
			}
		}

		void write_script_strings(const loaded_zone& zone)
		{
			auto strings = json::array();
			for (auto i = 0; i < zone.list.stringCount; i++)
			{
				strings.emplace_back(zone.list.strings[i] ? json(utils::string::dump_hex(zone.list.strings[i], "")) : json(nullptr));
			}

			write_text_file(std::format("scriptstrings/{}.json", zone.name), strings.dump(1, '\t'));
		}

		void write_binary_zone(const loaded_zone& zone)
		{
			const auto list_headers = get_list_headers(zone);
			std::unordered_set<const void*> written_headers;
			std::unordered_set<std::string> written_paths;

			auto csv = std::format("scriptstrings,{}\n", zone.name);
			for (auto i = 0; i < zone.list.assetCount; i++)
			{
				const auto& asset = zone.list.assets[i];
				if (!written_headers.insert(asset.header.data).second)
				{
					continue;
				}

				auto name = get_asset_name(asset.type, asset.header.data);
				if (name.empty())
				{
					name = std::format("#{}", i);
				}

				csv += std::format("{},{}\n", type_to_string(asset.type), name);
				if (!written_paths.insert(get_asset_file_path(asset.type, name)).second)
				{
					ZONETOOL_WARNING("%s \"%s\" has the same asset file as an asset before it", type_to_string(asset.type), name.data());
					continue;
				}

				write_binary_asset(zone, asset, list_headers, name);
			}

			write_script_strings(zone);
			write_text_file(zone.name + ".csv", csv);
		}

		void prepare_paths(const std::filesystem::path& zone_folder, const std::string& fastfile)
		{
			filesystem::set_zone_folder(zone_folder);
			filesystem::set_fastfile(fastfile);
			filesystem::set_language("english", "eng");
		}

		void clear_loaded_data()
		{
			db_clear();
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
				zones.emplace_back(load_zone(*path, true));
			}

			return zones;
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
			try
			{
				dump_loaded(asset, converter);
			}
			catch (const std::exception& e)
			{
				ZONETOOL_ERROR("Failed to dump %s \"%s\": %s", type_to_string(asset.type),
					get_asset_name(asset.type, asset.header).data(), e.what());
			}
		}

		if (converter && converter->finish)
		{
			converter->finish(zone->name);
		}
		else
		{
			write_binary_zone(*zone);
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

		const auto _ = gsl::finally(clear_loaded_data);

		prepare_paths(zone_folder, zone_path.stem().string());
		const dump_cache_scope cache(filesystem::get_dump_path());
		const auto required_zones = load_required_zones(converter, zone_path);
		const auto zone = load_zone(zone_path);

		filesystem::set_dump_folder("assets");
		const auto list_headers = get_list_headers(*zone);
		auto found = false;
		for (const auto& asset : zone->assets)
		{
			if (asset.type == type && (name == "*" || name == get_asset_name(asset.type, asset.header)))
			{
				dump_loaded(asset, converter);
				if (const auto asset_name = get_asset_name(asset.type, asset.header); !converter && !asset_name.empty())
				{
					write_binary_asset(*zone, XAsset{asset.type, XAssetHeader{asset.header}}, list_headers, asset_name);
				}

				found = true;
			}
		}

		if (!found)
		{
			throw std::runtime_error(std::format("{} \"{}\" is not in zone \"{}\"", type_name, name, zone->name));
		}

		if (converter && converter->finish)
		{
			converter->finish(name == "*" ? zone->name : name);
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

	std::string inspect_zone([[maybe_unused]] const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path)
	{
		const auto zone = open_zone(zone_path);

		std::array<std::size_t, ASSET_TYPE_COUNT> counts{};
		for (auto i = 0; i < zone->list.assetCount; i++)
		{
			const auto type = zone->list.assets[i].type;
			if (type >= ASSET_TYPE_COUNT)
			{
				throw std::runtime_error(std::format("asset {} has type {}", i, static_cast<int>(type)));
			}

			counts[type]++;
		}

		auto text = std::format("strings: {}\nassets: {}\n", zone->list.stringCount, zone->list.assetCount);
		for (std::size_t type = 0; type < counts.size(); type++)
		{
			if (counts[type])
			{
				text += std::format("  {:>3} {}: {}\n", type, asset_type_names[type], counts[type]);
			}
		}

		return text;
	}
}
