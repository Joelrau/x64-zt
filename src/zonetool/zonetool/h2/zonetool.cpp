#include <std_include.hpp>
#include "zonetool.hpp"

#include "zonetool/h1/zonetool.hpp"

#include "converter/converter.hpp"

#include "../utils/mapents.hpp"
#include "../utils/gsc.hpp"
#include "../utils/csv_generator.hpp"

#include <utils/io.hpp>

namespace zonetool::h2
{
	struct dump_params
	{
		game::game_mode target;
		std::string zone;
		bool valid;
		std::unordered_set<XAssetType> filter;
	};

	zonetool_globals_t globals{};
	std::vector<std::pair<XAssetType, std::string>> referenced_assets;
	std::unordered_set<XAssetType> asset_type_filter;

	const char* get_asset_name(XAssetType type, void* pointer)
	{
		XAssetHeader header{.data = pointer};
		XAsset xasset = {XAssetType(type), header};
		return DB_GetXAssetName(&xasset);
	}

	const char* get_asset_name(XAsset* asset)
	{
		return DB_GetXAssetName(asset);
	}

	void set_asset_name(XAsset* asset, const char* name)
	{
		DB_SetXAssetName(asset, name);
	}

	const char* type_to_string(XAssetType type)
	{
		return g_assetNames[type];
	}

	std::int32_t type_to_int(std::string type)
	{
		if (type == "col_map_mp")
		{
			return ASSET_TYPE_COL_MAP_SP;
		}

		for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			if (g_assetNames[i] == type)
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

	bool zone_exists(const std::string& zone)
	{
		return DB_FileExists(zone.data(), 0);
	}

	bool is_referenced_asset(XAsset* asset)
	{
		if (get_asset_name(asset)[0] == ',')
		{
			return true;
		}
		return false;
	}

	void wait_for_database()
	{
		// wait for database to be ready
		while (!WaitForSingleObject(*reinterpret_cast<HANDLE*>(0x14B11DEB8), 0) == 0)
		{
			Sleep(5);
		}
	}

	XAssetHeader db_find_x_asset_header(XAssetType type, const char* name, int create_default)
	{
		return zonetool::db_find_x_asset_header<XAssetHeader>(type, name, create_default);
	}

	XAssetHeader db_find_x_asset_header_safe(XAssetType type, const std::string& name)
	{
		return zonetool::db_find_x_asset_header_safe<XAssetHeader, XAssetEntry>(type, name);
	}

	void DB_EnumXAssets(const XAssetType type,
		const std::function<void(XAssetHeader)>& callback, const bool includeOverride)
	{
		DB_EnumXAssets_Internal(type, static_cast<void(*)(XAssetHeader, void*)>([](XAssetHeader header, void* data)
		{
			const auto& cb = *static_cast<const std::function<void(XAssetHeader)>*>(data);
			cb(header);
		}), &callback, includeOverride);
	}

	void dump_asset_h1(XAsset* asset)
	{
#define DUMP_ASSET_NO_CONVERT(__type__,___,__struct__) \
		if (asset->type == __type__) \
		{ \
			if (IS_DEBUG) \
			{ \
				ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			} \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			___::dump(asset_ptr); \
		} \

#define DUMP_ASSET(__type__, __namespace__,__struct__) \
		if (asset->type == __type__) \
		{ \
			if (IS_DEBUG) \
			{ \
				ZONETOOL_INFO("Dumping and converting asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			} \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			converter::h1::__namespace__::dump(asset_ptr); \
		} \

		try
		{
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_CLUT, clut, Clut);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_DOPPLER_PRESET, doppler_preset, DopplerPreset);
			DUMP_ASSET(ASSET_TYPE_FX, fxeffectdef, FxEffectDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation);
			DUMP_ASSET(ASSET_TYPE_IMAGE, gfximage, GfxImage);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef);
			DUMP_ASSET(ASSET_TYPE_LASER, laserdef, LaserDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LPF_CURVE, lpf_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LUA_FILE, lua_file, LuaFile);
			DUMP_ASSET(ASSET_TYPE_MATERIAL, material, Material);
			DUMP_ASSET(ASSET_TYPE_MAP_ENTS, mapents, MapEnts);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_RAWFILE, rawfile, RawFile);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef);
			DUMP_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SKELETONSCRIPT, skeleton_script, SkeletonScript);
			DUMP_ASSET(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRUCTUREDDATADEF, structured_data_def_set, StructuredDataDefSet);
			DUMP_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TTF, ttf_def, TTFDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XANIM, xanim_parts, XAnimParts);
			DUMP_ASSET(ASSET_TYPE_XMODEL, xmodel, XModel);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs);

			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint, PhysConstraint);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset, PhysWaterPreset);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSWORLDMAP, phys_world, PhysWorld);

			DUMP_ASSET(ASSET_TYPE_COMPUTESHADER, techset, ComputeShader);
			DUMP_ASSET(ASSET_TYPE_DOMAINSHADER, techset, MaterialDomainShader);
			DUMP_ASSET(ASSET_TYPE_HULLSHADER, techset, MaterialHullShader);
			DUMP_ASSET(ASSET_TYPE_PIXELSHADER, techset, MaterialPixelShader);
			//DUMP_ASSET(ASSET_TYPE_VERTEXDECL, techset, MaterialVertexDeclaration);
			DUMP_ASSET(ASSET_TYPE_VERTEXSHADER, techset, MaterialVertexShader);

			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENU, zonetool::h1::menu_def, zonetool::h1::menuDef_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENULIST, zonetool::h1::menu_list, zonetool::h1::MenuList);

			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_AIPATHS, path_data, PathData);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_COL_MAP_SP, clip_map, clipMap_t);
			DUMP_ASSET(ASSET_TYPE_COM_MAP, comworld, ComWorld);
			DUMP_ASSET(ASSET_TYPE_FX_MAP, fxworld, FxWorld);
			DUMP_ASSET(ASSET_TYPE_GFX_MAP, gfxworld, GfxWorld);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_GLASS_MAP, glass_world, GlassWorld);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s", filesystem::get_fastfile().data(), ex.what());
		}

#undef DUMP_ASSET_NO_CONVERT
#undef DUMP_ASSET
	}

	void dump_asset_h2(XAsset* asset)
	{
#define DUMP_ASSET(__type__,___,__struct__) \
		if (asset->type == __type__) \
		{ \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			___::dump(asset_ptr); \
		}

		try
		{
			DUMP_ASSET(ASSET_TYPE_CLUT, clut, Clut);
			DUMP_ASSET(ASSET_TYPE_DOPPLER_PRESET, doppler_preset, DopplerPreset);
			DUMP_ASSET(ASSET_TYPE_FX, fx_effect_def, FxEffectDef);
			DUMP_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation);
			DUMP_ASSET(ASSET_TYPE_IMAGE, gfx_image, GfxImage);
			DUMP_ASSET(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef);
			DUMP_ASSET(ASSET_TYPE_LASER, laser_def, LaserDef);
			DUMP_ASSET(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound);
			DUMP_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry);
			DUMP_ASSET(ASSET_TYPE_LPF_CURVE, lpf_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_LUA_FILE, lua_file, LuaFile);
			DUMP_ASSET(ASSET_TYPE_MATERIAL, material, Material);
			DUMP_ASSET(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings);
			DUMP_ASSET(ASSET_TYPE_RAWFILE, rawfile, RawFile);
			DUMP_ASSET(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef);
			DUMP_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile);
			DUMP_ASSET(ASSET_TYPE_SKELETONSCRIPT, skeleton_script, SkeletonScript);
			DUMP_ASSET(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext);
			DUMP_ASSET(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET(ASSET_TYPE_STRUCTUREDDATADEF, structured_data_def_set, StructuredDataDefSet);
			DUMP_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			DUMP_ASSET(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			DUMP_ASSET(ASSET_TYPE_TTF, ttf_def, TTFDef);
			DUMP_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			DUMP_ASSET(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			DUMP_ASSET(ASSET_TYPE_VEHICLE, vehicle_def, VehicleDef);
			DUMP_ASSET(ASSET_TYPE_XANIM, xanim_parts, XAnimParts);
			DUMP_ASSET(ASSET_TYPE_XMODEL, xmodel, XModel);
			DUMP_ASSET(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs);

			DUMP_ASSET(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap);
			DUMP_ASSET(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint, PhysConstraint);
			DUMP_ASSET(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset);
			DUMP_ASSET(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset, PhysWaterPreset);
			DUMP_ASSET(ASSET_TYPE_PHYSWORLDMAP, phys_world, PhysWorld);

			DUMP_ASSET(ASSET_TYPE_COMPUTESHADER, compute_shader, ComputeShader);
			DUMP_ASSET(ASSET_TYPE_DOMAINSHADER, domain_shader, MaterialDomainShader);
			DUMP_ASSET(ASSET_TYPE_HULLSHADER, hull_shader, MaterialHullShader);
			DUMP_ASSET(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader);
			//DUMP_ASSET(ASSET_TYPE_VERTEXDECL, vertex_decl, MaterialVertexDeclaration);
			DUMP_ASSET(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader);

			DUMP_ASSET(ASSET_TYPE_MENU, zonetool::h1::menu_def, zonetool::h1::menuDef_t);
			DUMP_ASSET(ASSET_TYPE_MENULIST, zonetool::h1::menu_list, zonetool::h1::MenuList);
			
			DUMP_ASSET(ASSET_TYPE_AIPATHS, path_data, PathData);
			DUMP_ASSET(ASSET_TYPE_ADDON_MAP_ENTS, addon_map_ents, AddonMapEnts);
			DUMP_ASSET(ASSET_TYPE_COL_MAP_SP, clip_map, clipMap_t);
			DUMP_ASSET(ASSET_TYPE_COM_MAP, com_world, ComWorld);
			DUMP_ASSET(ASSET_TYPE_FX_MAP, fx_world, FxWorld);
			DUMP_ASSET(ASSET_TYPE_GFX_MAP, gfx_world, GfxWorld);
			DUMP_ASSET(ASSET_TYPE_GLASS_MAP, glass_world, GlassWorld);
			DUMP_ASSET(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts);
		}
		catch (const std::exception& e)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s",
				filesystem::get_fastfile().data(), e.what());
		}
	}

	std::unordered_map<game::game_mode, std::function<void(XAsset*)>> dump_functions =
	{
		{game::h1, dump_asset_h1},
		{game::h2, dump_asset_h2},
	};

	void dump_asset(XAsset* asset)
	{
		if (globals.verify)
		{
			ZONETOOL_INFO("Loading asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type));
		}

		if (!globals.dump)
		{
			return;
		}

		if (asset_type_filter.size() > 0 && !asset_type_filter.contains(asset->type))
		{
			return;
		}

		if (globals.csv_file.get_fp() == nullptr)
		{
			globals.csv_file = filesystem::file(filesystem::get_fastfile() + ".csv");
			globals.csv_file.open("wb");
		}

		// dump assets to disk
		if (globals.csv_file.get_fp()/* && !is_referenced_asset(xasset)*/)
		{
			std::fprintf(globals.csv_file.get_fp(), "%s,%s\n", type_to_string(asset->type), get_asset_name(asset));
		}

		// dump referenced later
		if (is_referenced_asset(asset))
		{
			//referenced_assets.emplace_back(asset->type, get_asset_name(asset));
			return;
		}

		const auto dump_func = dump_functions.find(globals.target_game);
		if (dump_func == dump_functions.end())
		{
			const auto name = game::get_mode_as_string(globals.target_game);
			ZONETOOL_ERROR("Dump mode \"%s\" is not supported", name.data());
			return;
		}

		dump_func->second(asset);
	}

	void stop_dumping()
	{
		if (!globals.dump)
		{
			return;
		}

		// remove duplicates
		std::sort(referenced_assets.begin(), referenced_assets.end());
		referenced_assets.erase(std::unique(referenced_assets.begin(),
			referenced_assets.end()), referenced_assets.end());

		for (auto& asset : referenced_assets)
		{
			if (asset.second.length() <= 1)
			{
				continue;
			}

			const auto asset_name = &asset.second[1];

			if (asset.first == ASSET_TYPE_IMAGE)
			{
				ZONETOOL_WARNING("Not dumping referenced asset \"%s\" of type \"%s\"", asset_name, type_to_string(asset.first));
				continue;
			}

			const auto& asset_header = db_find_x_asset_header_safe(asset.first, asset_name);

			if (!asset_header.data || DB_IsXAssetDefault(asset.first, asset_name))
			{
				ZONETOOL_ERROR("Could not find referenced asset \"%s\" of type \"%s\"", asset_name, type_to_string(asset.first));
				continue;
			}

			//ZONETOOL_INFO("Dumping additional asset \"%s\" of type \"%s\"", asset_name, type_to_string(asset.first));

			XAsset referenced_asset = 
			{
				asset.first,
				asset_header
			};

			dump_asset(&referenced_asset);
		}

		ZONETOOL_INFO("Zone \"%s\" dumped.", filesystem::get_fastfile().data());

		referenced_assets.clear();

		globals.csv_file.close();
		globals.csv_file = {};

		globals.dump = false;
	}

	utils::hook::detour db_link_x_asset_entry1_hook;
	XAssetEntry* db_link_x_asset_entry1_stub(XAssetType type, XAssetHeader* header)
	{
		XAsset xasset = 
		{
			type,
			*header
		};

		dump_asset(&xasset);
		return db_link_x_asset_entry1_hook.invoke<XAssetEntry*>(type, header);
	}

	utils::hook::detour db_finish_load_x_file_hook;
	void db_finish_load_x_file_stub()
	{
		globals.verify = false;
		stop_dumping();
		return db_finish_load_x_file_hook.invoke<void>();
	}

	utils::hook::detour load_x_gfx_globals_hook;
	void load_x_gfx_globals_stub(bool atStreamStart)
	{
		load_x_gfx_globals_hook.invoke<void>(atStreamStart);

		const auto var_x_gfx_globals = *reinterpret_cast<XGfxGlobals**>(0x14224EE18);
		insert_x_gfx_globals_for_zone(*g_zoneIndex, var_x_gfx_globals);
	}

	void reallocate_asset_pool(const XAssetType type, const unsigned int new_size)
	{
		const size_t element_size = DB_GetXAssetTypeSize(type);

		auto* new_pool = utils::memory::get_allocator()->allocate(new_size * element_size);
		std::memmove(new_pool, g_assetPool[type], g_poolSize[type] * element_size);

		g_assetPool[type] = new_pool;
		g_poolSize[type] = new_size;
	}

	void reallocate_asset_pool_multiplier(const XAssetType type, unsigned int multiplier)
	{
		reallocate_asset_pool(type, multiplier * g_poolSize[type]);
	}

	bool load_zone(const std::string& name, DBSyncMode mode = DB_LOAD_SYNC, bool inform = true)
	{
		if (!zone_exists(name.data()))
		{
			ZONETOOL_INFO("Zone \"%s\" could not be found!", name.data());
			return false;
		}

		wait_for_database();

		if (!globals.dump && !globals.verify)
		{
			for (unsigned int i = 0; i < *g_zoneCount; i++)
			{
				if (!_strnicmp(::h2::game::g_zones[i + 1].name, name.data(), 64))
				{
					if (inform)
					{
						ZONETOOL_INFO("zone \"%s\" is already loaded...", name.data());
					}
					return true;
				}
			}
		}
		if (inform)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());
		}

		XZoneInfo zone = {name.data(), DB_ZONE_LOAD | DB_ZONE_CUSTOM, 0};
		DB_LoadXAssets(&zone, 1, mode);
		return true;
	}

	void unload_zones()
	{
		ZONETOOL_INFO("Unloading zones...");

		static XZoneInfo zone = {0, DB_ZONE_NONE, 70};
		DB_LoadXAssets(&zone, 1, DB_LOAD_ASYNC_FORCE_FREE);

		ZONETOOL_INFO("Unloaded zones...");
	}

	void dump_zone(const std::string& name, const game::game_mode target, const std::optional<std::string> fastfile = {})
	{
		if (!zone_exists(name.data()))
		{
			ZONETOOL_INFO("Zone \"%s\" could not be found!", name.data());
			return;
		}

		wait_for_database();

		globals.target_game = target;
		ZONETOOL_INFO("Dumping zone \"%s\"...", name.data());

		if (fastfile.has_value())
		{
			filesystem::set_fastfile(fastfile.value());
		}
		else
		{
			filesystem::set_fastfile(name);
		}

		globals.dump = true;
		if (!load_zone(name, DB_LOAD_ASYNC, false))
		{
			globals.dump = false;
		}

		while (globals.dump)
		{
			Sleep(1);
		}
	}

	void verify_zone(const std::string& name)
	{
		if (!zone_exists(name.data()))
		{
			ZONETOOL_INFO("Zone \"%s\" could not be found!", name.data());
			return;
		}

		wait_for_database();

		globals.verify = true;
		if (!load_zone(name, DB_LOAD_ASYNC, true))
		{
			globals.verify = false;
		}

		while (globals.verify)
		{
			Sleep(1);
		}
	}

	void add_assets_using_iterator(const std::string& fastfile, const std::string& type, const std::string& folder,
		const std::string& extension, bool skip_reference, zone_base* zone)
	{
		const auto path = "zonetool\\" + fastfile + "\\" + folder;
		if (!std::filesystem::is_directory(path))
		{
			return;
		}

		const auto iter = std::filesystem::recursive_directory_iterator(path);
		for (auto& file : iter)
		{
			if (!is_regular_file(file))
			{
				continue;
			}

			const auto filename = file.path().filename().string();

			if (skip_reference && filename[0] == ',')
			{
				continue;
			}

			if (!extension.empty() && filename.ends_with(extension))
			{
				const auto base_name = filename.substr(0, filename.length() - extension.length());
				zone->add_asset_of_type(type, base_name);
			}
			else if (file.path().extension().empty())
			{
				zone->add_asset_of_type(type, filename);
			}
		}
	}

	void try_parse_csv_file(zone_base* zone, const std::string& fastfile, const std::string& csv)
	{
		auto path = "zone_source\\" + csv + ".csv";
		auto parser = csv::parser(path.data(), ',');

		if (!parser.valid())
		{
			ZONETOOL_ERROR("Could not find csv file \"%s\" to build zone!", csv.data());
			return;
		}

		auto is_referencing = false;
		auto rows = parser.get_rows();
		if (rows == nullptr)
		{
			return;
		}

		for (auto row_index = 0; row_index < parser.get_num_rows(); row_index++)
		{
			auto* row = rows[row_index];
			if (row == nullptr)
			{
				continue;
			}

			if (!row->fields)
			{
				continue;
			}

			if ((strlen(row->fields[0]) >= 1 && row->fields[0][0] == '#') || (strlen(row->fields[0]) >= 2 && row->
				fields[0][0] == '/' && row->fields[0][1] == '/'))
			{
				// comment line, go to next line.
				continue;
			}
			if (!strlen(row->fields[0]))
			{
				// empty line, go to next line.
				continue;
			}
			if (row->fields[0] == "require"s)
			{
				load_zone(row->fields[1], DB_LOAD_ASYNC);
				wait_for_database();
			}
			else if (row->fields[0] == "include"s)
			{
				try_parse_csv_file(zone, fastfile, row->fields[1]);
			}
			// this allows us to reference assets instead of rewriting them
			else if (row->fields[0] == "reference"s)
			{
				if (row->num_fields >= 2)
				{
					is_referencing = row->fields[1] == "true"s;
				}
			}
			// this will use a directory iterator to automatically add assets
			else if (row->fields[0] == "iterate"s)
			{
				if (row->num_fields >= 2)
				{
					auto type = row->fields[1];
					auto iterate_all = row->fields[1] == "true"s;

					try
					{
						if (type == "fx"s || iterate_all)
						{
							add_assets_using_iterator(fastfile, type, "effects", ".fxe", true, zone);
						}
						if (type == "material"s || iterate_all)
						{
							add_assets_using_iterator(fastfile, type, "materials", "", true, zone);
						}
						if (type == "xmodel"s || iterate_all)
						{
							add_assets_using_iterator(fastfile, type, "xmodel", ".xmb", true, zone);
						}
						if (type == "xanim"s || iterate_all)
						{
							add_assets_using_iterator(fastfile, type, "xanim", ".xab", true, zone);
						}
					}
					catch (const std::exception& e)
					{
						ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: \n%s", fastfile.data(), e.what());
					}
				}
			}
			// add paths
			else if ((row->fields[0] == "addpath"s || row->fields[0] == "addpaths"s) && row->num_fields >= 2)
			{
				bool insert_at_beginning = row->num_fields >= 3 && row->fields[2] == "true"s;

				if (row->fields[0] == "addpath"s)
					filesystem::add_path(row->fields[1], insert_at_beginning);
				else
					filesystem::add_paths_from_directory(row->fields[1], insert_at_beginning);
			}
			// if entry is not an option, it should be an asset.
			else
			{
				if (row->fields[0] == "localize"s && row->num_fields >= 2 &&
					filesystem::file("localizedstrings/"s + row->fields[1] + ".str").exists())
				{
					localize::parse_localizedstrings_file(zone, row->fields[1]);
				}
				else if (row->fields[0] == "localize"s && row->num_fields >= 2 &&
					filesystem::file("localizedstrings/"s + row->fields[1] + ".json").exists())
				{
					localize::parse_localizedstrings_json(zone, row->fields[1]);
				}
				else
				{
					if (row->num_fields < 2 || !is_valid_asset_type(row->fields[0]))
					{
						continue;
					}

					std::string name;
					if ((!row->fields[1] || !strlen(row->fields[1]) && row->fields[2] && strlen(row->fields[2])))
					{
						name = ","s + row->fields[2];
					}
					else
					{
						name = ((is_referencing) ? ","s : ""s) + row->fields[1];
					}

					try
					{
						zone->add_asset_of_type(
							row->fields[0],
							name
						);
					}
					catch (std::exception& ex)
					{
						ZONETOOL_FATAL("A fatal exception occured while building zone \"%s\", exception was: \n%s", fastfile.data(), ex.what());
					}
				}
			}
		}
	}

	void parse_csv_file(zone_base* zone, const std::string& fastfile, const std::string& csv)
	{
		try
		{
			try_parse_csv_file(zone, fastfile, csv);
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("%s", e.what());
		}
	}

	std::shared_ptr<zone_base> alloc_zone(const std::string& zone)
	{
		auto ptr = std::make_shared<zone_interface>(zone);
		return ptr;
	}

	std::shared_ptr<zone_buffer> alloc_buffer()
	{
		auto ptr = std::make_shared<zone_buffer>();
		ptr->init_streams(7);

		return ptr;
	}

	void build_zone(const std::string& fastfile)
	{
		// make sure FS is correct.
		filesystem::set_fastfile(fastfile);

		ZONETOOL_INFO("Building fastfile \"%s\"", fastfile.data());

		auto zone = alloc_zone(fastfile);
		if (zone == nullptr)
		{
			ZONETOOL_ERROR("An error occured while building fastfile \"%s\": Are you out of memory?", fastfile.data());
			return;
		}

		parse_csv_file(zone.get(), fastfile, fastfile);

		// allocate zone buffer
		auto buffer = alloc_buffer();

		// add branding asset
		zone->add_asset_of_type("rawfile", fastfile);

		// compile zone
		zone->build(buffer.get());

		// clear asset shit
		material::fixed_nml_images_map.clear();
		techset::vertexdecl_pointers.clear();
		xanim_parts::secondary_anims.clear();
	}

	dump_params get_dump_params(const ::h2::command::params& params)
	{
		dump_params dump_params{};
		dump_params.target = game::h2;

		const auto parse_params = [&]()
		{
			if (params.size() < 3)
			{
				dump_params.zone = params.get(1);
				return true;
			}

			const auto mode = params.get(1);
			dump_params.zone = params.get(2);
			dump_params.target = game::get_mode_from_string(mode);

			if (dump_params.target == game::none)
			{
				ZONETOOL_ERROR("Invalid dump target \"%s\"", mode);
				return false;
			}

			if (!dump_functions.contains(dump_params.target))
			{
				ZONETOOL_ERROR("Unsupported dump target \"%s\" (%i)", mode, dump_params.target);
				return false;
			}

			if (params.size() >= 4)
			{
				const auto asset_types_str = params.get(3);
				if (asset_types_str == "_"s)
				{
					return true;
				}

				const auto asset_types = utils::string::split(asset_types_str, ',');

				for (const auto& type_str : asset_types)
				{
					const auto type = type_to_int(type_str);
					if (type == -1)
					{
						ZONETOOL_ERROR("Asset type \"%s\" does not exist", type_str.data());
						return false;
					}

					dump_params.filter.insert(static_cast<XAssetType>(type));
				}
			}

			return true;
		};

		dump_params.valid = parse_params();
		return dump_params;
	}

	void register_commands()
	{
		::h2::command::add("quit", []()
		{
			std::quick_exit(EXIT_SUCCESS);
		});

		::h2::command::add("buildzone", [](const ::h2::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}

			build_zone(params.get(1));
		});

		::h2::command::add("loadzone", [](const ::h2::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}

			load_zone(params.get(1));
		});

		::h2::command::add("unloadzones", []()
		{
			unload_zones();
		});

		::h2::command::add("dumpzone", [](const ::h2::command::params& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			const auto dump_params = get_dump_params(params);
			if (!dump_params.valid)
			{
				return;
			}

			asset_type_filter = dump_params.filter;
			dump_zone(dump_params.zone, dump_params.target);
		});

		::h2::command::add("dumpmap", [](const ::h2::command::params& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_ERROR("usage: dumpmap <zone>");
				return;
			}

			const auto dump_params = get_dump_params(params);
			if (!dump_params.valid)
			{
				return;
			}

			const auto dump_zone_ = [&](const std::string& zone, const std::optional<std::unordered_set<XAssetType>>& filter = {})
			{
				asset_type_filter.clear();
				if (filter.has_value())
				{
					asset_type_filter = filter.value();
				}

				dump_zone(zone, dump_params.target, {dump_params.zone});
			};

			auto skip_common = false;
			if (params.size() >= 5)
			{
				const std::string skip_value = params.get(4);
				skip_common = skip_value == "1" || skip_value == "true";
			}

			const std::unordered_set<XAssetType> techset_filter =
			{
				ASSET_TYPE_TECHNIQUE_SET,
				ASSET_TYPE_PIXELSHADER,
				ASSET_TYPE_VERTEXSHADER,
				ASSET_TYPE_COMPUTESHADER,
				ASSET_TYPE_DOMAINSHADER,
				ASSET_TYPE_HULLSHADER,
				ASSET_TYPE_VERTEXDECL
			};

			const std::unordered_set<XAssetType> common_filter =
			{
				ASSET_TYPE_IMAGE,
			};

			unload_zones();

			if (skip_common)
			{
				load_zone("code_post_gfx");
				load_zone("common");
				load_zone("techsets_common");
			}
			else
			{
				dump_zone_("code_post_gfx", techset_filter);
				dump_zone_("common", common_filter);
				dump_zone_("techsets_common", techset_filter);
			}

			dump_zone_("techsets_" + dump_params.zone, dump_params.filter);
			dump_zone_("eng_patch_" + dump_params.zone, dump_params.filter);
			dump_zone_("patch_" + dump_params.zone, dump_params.filter);
			dump_zone_(dump_params.zone);

			ZONETOOL_INFO("Map \"%s\" dumped", dump_params.zone.data());
		});

		::h2::command::add("dumpasset", [](const ::h2::command::params& params)
		{
			const auto type = XAssetType(type_to_int(params.get(1)));
			const auto name = params.get(2);

			XAsset asset{};
			asset.type = type;

			const auto header = db_find_x_asset_header(type, name, false);
			if (!header.data)
			{
				ZONETOOL_INFO("Asset not found\n");
				return;
			}

			globals.dump = true;
			const auto _0 = gsl::finally([]
			{
				globals.dump = false;
			});

			filesystem::set_fastfile("assets");
			asset.header = header;

			const auto dump_params = get_dump_params(params);
			if (!dump_params.valid)
			{
				return;
			}

			asset_type_filter.clear();
			globals.target_game = dump_params.target;
			dump_asset(&asset);

			ZONETOOL_INFO("Dumped to dump/assets");
		});
		
		::h2::command::add("verifyzone", [](const ::h2::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			verify_zone(params.get(1));
		});

		::h2::command::add("buildxmodel", [](const ::h2::command::params& params)
		{
			if (params.size() < 2)
			{
				return;
			}

			std::string name = params.get(1);
			auto path = "zone_source\\" + name + ".csv";
			auto parser = csv::parser(path.data(), ',');

			if (!parser.valid())
			{
				ZONETOOL_ERROR("Could not find csv file \"%s\" to build xmodel!", name.data());
				return;
			}

			auto rows = parser.get_rows();
			if (rows == nullptr)
			{
				return;
			}

			std::vector<std::string> attachments;

			for (auto row_index = 0; row_index < parser.get_num_rows(); row_index++)
			{
				auto* row = rows[row_index];
				if (row == nullptr || row->fields == nullptr || row->num_fields < 2)
				{
					continue;
				}

				if (row->fields[0] == "name"s)
				{
					name = row->fields[1];
				}

				if (row->fields[0] == "base"s)
				{
					attachments.insert(attachments.begin(), row->fields[1]);
				}

				if (row->fields[0] == "attachment"s)
				{
					attachments.push_back(row->fields[1]);
				}
			}

			filesystem::set_fastfile("custom_xmodels");
			xmodel::build_composite_model(name, attachments);

			ZONETOOL_INFO("Saved to \"dump/custom_xmodels/xmodel/%s.xmb\"", name.data());
		});

		::h2::command::add("generatecsv", csv_generator::create_command
			<::h2::command::params>([](const uint32_t id)
		{
			return gsc::h2::gsc_ctx->token_name(id);
		}));
	}

	std::vector<std::string> get_command_line_arguments()
	{
		LPWSTR* szArglist;
		int nArgs;

		szArglist = CommandLineToArgvW(GetCommandLineW(), &nArgs);

		std::vector<std::string> args;
		args.resize(nArgs);

		// convert all args to std::string
		for (int i = 0; i < nArgs; i++)
		{
			auto curArg = std::wstring(szArglist[i]);
			args[i] = std::string(curArg.begin(), curArg.end());
		}

		// return arguments
		return args;
	}

	void handle_params()
	{
		// Execute command line commands
		auto args = get_command_line_arguments();
		if (args.size() > 1)
		{
			for (std::size_t i = 0; i < args.size(); i++)
			{
				if (i < args.size() - 1 && i + 1 < args.size())
				{
					if (args[i] == "-loadzone")
					{
						load_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-buildzone")
					{
						build_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-buildzones")
					{
						const auto& filename = args[i + 1];
						std::string data{};
						if (!utils::io::read_file(filename, &data))
						{
							return;
						}

						const auto zones = utils::string::split(data, '\n');
						for (auto zone : zones)
						{
							if (zone.ends_with("\r"))
							{
								zone.pop_back();
							}

							build_zone(zone);
						}

						i++;
					}
					else if (args[i] == "-verifyzone")
					{
						verify_zone(args[i + 1]);
						i++;
					}
					else if (args[i] == "-dumpzone")
					{
						dump_zone(args[i + 1], game::h2);
						i++;
					}
				}
			}

			std::quick_exit(EXIT_SUCCESS);
		}
	}

	void on_exit(void)
	{
		globals.dump = false;
		globals.csv_file.close();
	}

	utils::hook::detour doexit_hook;
	void doexit(unsigned int a1, int a2, int a3)
	{
		on_exit();
		doexit_hook.invoke<void>(a1, a2, a3);
		//std::quick_exit(0);
	}

	void init_zonetool()
	{
		static bool initialized = false;
		if (initialized) return;
		initialized = true;

		ZONETOOL_INFO("ZoneTool is initializing...");

		// reallocs
		reallocate_asset_pool_multiplier(ASSET_TYPE_LUA_FILE, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_WEAPON, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_LOCALIZE_ENTRY, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_XANIM, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_ATTACHMENT, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_TTF, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_SNDDRIVERGLOBALS, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_EQUIPMENT_SND_TABLE, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_SOUND, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_LOADED_SOUND, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_LEADERBOARDDEF, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_VERTEXDECL, 6);
		reallocate_asset_pool_multiplier(ASSET_TYPE_COMPUTESHADER, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_REVERB_PRESET, 2);

		// enable dumping
		db_link_x_asset_entry1_hook.create(0x140414900, &db_link_x_asset_entry1_stub);

		// stop dumping
		db_finish_load_x_file_hook.create(0x1403E6010, &db_finish_load_x_file_stub);

		// store xGfxGlobals pointers
		load_x_gfx_globals_hook.create(0x140401300, &load_x_gfx_globals_stub);

		doexit_hook.create(0x1408B1A34, doexit);
		atexit(on_exit);
	}

	void finalize()
	{
		ZONETOOL_INFO("ZoneTool initialization complete!");
	}

	void initialize()
	{
		init_zonetool();
	}

	void start()
	{
		initialize();
		finalize();

		branding();
		register_commands();

		handle_params();
	}
}