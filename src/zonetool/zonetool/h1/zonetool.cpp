#include <std_include.hpp>
#include "zonetool.hpp"

#include "converter/converter.hpp"

#include "../utils/gsc.hpp"
#include "../utils/csv_generator.hpp"

#include <utils/io.hpp>

namespace zonetool::h1
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

	std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

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
		if (type == "col_map_sp")
		{
			return ASSET_TYPE_CLIPMAP;
		}

		for (std::int32_t i = 0; i < ASSET_TYPE_COUNT; i++)
		{
			if (g_assetNames[i] == type)
				return i;
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
		while (!WaitForSingleObject(*reinterpret_cast<HANDLE*>(0x149811020), 0) == 0)
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
#define DUMP_ASSET(__type__,___,__struct__) \
		if (asset->type == __type__) \
		{ \
			if (IS_DEBUG) \
			{ \
				ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			} \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			___::dump(asset_ptr); \
		} \

		try
		{
			DUMP_ASSET(ASSET_TYPE_CLUT, clut, Clut);
			DUMP_ASSET(ASSET_TYPE_DOPPLER_PRESET, doppler_preset, DopplerPreset);
			DUMP_ASSET(ASSET_TYPE_FX, fx_effect_def, FxEffectDef);
			DUMP_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation);
			DUMP_ASSET(ASSET_TYPE_IMAGE, gfx_image, GfxImage);
			DUMP_ASSET(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef);
			DUMP_ASSET(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable);
			DUMP_ASSET(ASSET_TYPE_LASER, laser_def, LaserDef);
			DUMP_ASSET(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound);
			DUMP_ASSET(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry);
			DUMP_ASSET(ASSET_TYPE_LPF_CURVE, lpf_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_LUA_FILE, lua_file, LuaFile);
			DUMP_ASSET(ASSET_TYPE_MATERIAL, material, Material);
			DUMP_ASSET(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts);
			DUMP_ASSET(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings);
			DUMP_ASSET(ASSET_TYPE_RAWFILE, rawfile, RawFile);
			DUMP_ASSET(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_REVERB_PRESET, reverb_preset, ReverbPreset);
			DUMP_ASSET(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef);
			DUMP_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile);
			DUMP_ASSET(ASSET_TYPE_SKELETON_SCRIPT, skeleton_script, SkeletonScript);
			DUMP_ASSET(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext);
			DUMP_ASSET(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve);
			DUMP_ASSET(ASSET_TYPE_SNDDRIVER_GLOBALS, sound_driver_globals, SndDriverGlobals);
			DUMP_ASSET(ASSET_TYPE_SOUND_SUBMIX, sound_submix, SndSubmixList);
			DUMP_ASSET(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET(ASSET_TYPE_STRUCTURED_DATA_DEF, structured_data_def_set, StructuredDataDefSet);
			DUMP_ASSET(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable);
			DUMP_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			DUMP_ASSET(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			DUMP_ASSET(ASSET_TYPE_TTF, ttf_def, TTFDef);
			DUMP_ASSET(ASSET_TYPE_VEHICLE, vehicle_def, VehicleDef);
			DUMP_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			DUMP_ASSET(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			DUMP_ASSET(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts);
			DUMP_ASSET(ASSET_TYPE_XMODEL, xmodel, XModel);
			DUMP_ASSET(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs);

			DUMP_ASSET(ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard, VirtualLeaderboardDef);
			DUMP_ASSET(ASSET_TYPE_LEADERBOARD, leaderboard, LeaderboardDef);

			DUMP_ASSET(ASSET_TYPE_DDL, ddl, DDLRoot);
			DUMP_ASSET(ASSET_TYPE_EQUIPMENT_SND_TABLE, equip_snd_table, EquipmentSoundTable);
			DUMP_ASSET(ASSET_TYPE_VECTORFIELD, vector_field, VectorField);
			DUMP_ASSET(ASSET_TYPE_ANIMCLASS, anim_class, AnimationClass);

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

			DUMP_ASSET(ASSET_TYPE_MENU, menu_def, menuDef_t);
			DUMP_ASSET(ASSET_TYPE_MENULIST, menu_list, MenuList);

			DUMP_ASSET(ASSET_TYPE_PATHDATA, path_data, PathData);
			DUMP_ASSET(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t);
			DUMP_ASSET(ASSET_TYPE_COMWORLD, com_world, ComWorld);
			DUMP_ASSET(ASSET_TYPE_FXWORLD, fx_world, FxWorld);
			DUMP_ASSET(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld);
			DUMP_ASSET(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s", filesystem::get_fastfile().data(), ex.what());
		}

#undef DUMP_ASSET
	}

	void dump_asset_h2(XAsset* asset)
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
				ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			} \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			converter::h2::__namespace__::dump(asset_ptr); \
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
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SKELETON_SCRIPT, skeleton_script, SkeletonScript);
			DUMP_ASSET(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRUCTURED_DATA_DEF, structured_data_def_set, StructuredDataDefSet);
			DUMP_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TTF, ttf_def, TTFDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts);
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
			
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENU, menu_def, menuDef_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENULIST, menu_list, MenuList);
			
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PATHDATA, path_data, PathData);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t);
			DUMP_ASSET(ASSET_TYPE_COMWORLD, comworld, ComWorld);
			DUMP_ASSET(ASSET_TYPE_FXWORLD, fxworld, FxWorld);
			DUMP_ASSET(ASSET_TYPE_GFXWORLD, gfxworld, GfxWorld);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s", filesystem::get_fastfile().data(), ex.what());
		}

#undef DUMP_ASSET_NO_CONVERT
#undef DUMP_ASSET
	}

	void dump_asset_s1(XAsset* asset)
	{
#define DUMP_ASSET_REGULAR(__type__,___,__struct__) \
		if (asset->type == __type__) \
		{ \
			if(IS_DEBUG) ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			___::dump(asset_ptr); \
		}

#define DUMP_ASSET_NO_CONVERT(__type__,___,__struct__) \
		if (asset->type == __type__) \
		{ \
			if(IS_DEBUG) ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			auto asset_ptr = reinterpret_cast<zonetool::h1::__struct__*>(asset->header.data); \
			zonetool::h1::___::dump(asset_ptr); \
		}

#define DUMP_ASSET_CONVERT(__type__,__namespace__,__struct__) \
		if (asset->type == __type__) \
		{ \
			if(IS_DEBUG) ZONETOOL_INFO("Converting and dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			converter::s1::__namespace__::dump(asset_ptr); \
		}

		try
		{
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_CLUT, clut, Clut);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_DOPPLER_PRESET, doppler_preset, DopplerPreset);
			DUMP_ASSET_CONVERT(ASSET_TYPE_FX, fxeffectdef, FxEffectDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation);
			DUMP_ASSET_CONVERT(ASSET_TYPE_IMAGE, gfximage, GfxImage);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LASER, laser_def, LaserDef);
			DUMP_ASSET_REGULAR(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LPF_CURVE, lpf_curve, SndCurve);
			//DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LUA_FILE, lua_file, LuaFile);
			DUMP_ASSET_CONVERT(ASSET_TYPE_MATERIAL, material, Material);
			DUMP_ASSET_CONVERT(ASSET_TYPE_MAP_ENTS, mapents, MapEnts);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_RAWFILE, rawfile, RawFile);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef);
			DUMP_ASSET_CONVERT(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SKELETON_SCRIPT, skeleton_script, SkeletonScript);
			DUMP_ASSET_CONVERT(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_VIRTUAL_LEADERBOARD, structured_data_def_set, StructuredDataDefSet);
			DUMP_ASSET_CONVERT(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			//DUMP_ASSET(ASSET_TYPE_FONT, font_def, Font_s);
			//DUMP_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			//DUMP_ASSET(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XMODEL, xmodel, XModel);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs);

			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint, PhysConstraint);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset, PhysWaterPreset);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_PHYSWORLDMAP, phys_world, PhysWorld);

			DUMP_ASSET_CONVERT(ASSET_TYPE_COMPUTESHADER, techset, ComputeShader);
			DUMP_ASSET_CONVERT(ASSET_TYPE_DOMAINSHADER, techset, MaterialDomainShader);
			DUMP_ASSET_CONVERT(ASSET_TYPE_HULLSHADER, techset, MaterialHullShader);
			DUMP_ASSET_CONVERT(ASSET_TYPE_PIXELSHADER, techset, MaterialPixelShader);
			//DUMP_ASSET_CONVERT(ASSET_TYPE_VERTEXDECL, vertexdecl, MaterialVertexDeclaration);
			DUMP_ASSET_CONVERT(ASSET_TYPE_VERTEXSHADER, techset, MaterialVertexShader);

			//DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENU, menu_def, menuDef_t);
			//DUMP_ASSET_NO_CONVERT(ASSET_TYPE_MENULIST, menu_list, MenuList);

			DUMP_ASSET_CONVERT(ASSET_TYPE_PATHDATA, aipaths, PathData);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_COMWORLD, com_world, ComWorld);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_FXWORLD, fx_world, FxWorld);
			DUMP_ASSET_CONVERT(ASSET_TYPE_GFXWORLD, gfxworld, GfxWorld);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s", filesystem::get_fastfile().data(), ex.what());
		}

#undef DUMP_ASSET_NO_CONVERT
#undef DUMP_ASSET
	}

	void dump_asset_iw7(XAsset* asset)
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
				ZONETOOL_INFO("Dumping asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type)); \
			} \
			auto asset_ptr = reinterpret_cast<__struct__*>(asset->header.data); \
			converter::iw7::__namespace__::dump(asset_ptr); \
		} \

		try
		{
			//DUMP_ASSET(ASSET_TYPE_FX, fxeffectdef, FxEffectDef);
			//DUMP_ASSET(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation);
			DUMP_ASSET(ASSET_TYPE_IMAGE, gfximage, GfxImage);
			//DUMP_ASSET(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef);
			//DUMP_ASSET(ASSET_TYPE_LASER, laserdef, LaserDef);
			//DUMP_ASSET(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry);
			//DUMP_ASSET_NO_CONVERT(ASSET_TYPE_LUA_FILE, lua_file, LuaFile);
			//DUMP_ASSET(ASSET_TYPE_MATERIAL, material, Material);
			//DUMP_ASSET(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_RAWFILE, rawfile, RawFile);
			//DUMP_ASSET(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef);
			//DUMP_ASSET(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile);
			//DUMP_ASSET(ASSET_TYPE_SOUND, sound, snd_alias_list_t);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRINGTABLE, string_table, StringTable);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_STRUCTURED_DATA_DEF, structured_data_def_set, StructuredDataDefSet);
			//DUMP_ASSET(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet);
			//DUMP_ASSET(ASSET_TYPE_TRACER, tracer_def, TracerDef);
			DUMP_ASSET_NO_CONVERT(ASSET_TYPE_TTF, ttf_def, TTFDef);
			//DUMP_ASSET(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment);
			//DUMP_ASSET(ASSET_TYPE_WEAPON, weapon_def, WeaponDef);
			//DUMP_ASSET(ASSET_TYPE_XANIM, xanim_parts, XAnimParts);
			//DUMP_ASSET(ASSET_TYPE_XMODEL, xmodel, XModel);
			//DUMP_ASSET(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs);

			//DUMP_ASSET(ASSET_TYPE_COMPUTESHADER, techset, ComputeShader);
			//DUMP_ASSET(ASSET_TYPE_DOMAINSHADER, techset, MaterialDomainShader);
			//DUMP_ASSET(ASSET_TYPE_HULLSHADER, techset, MaterialHullShader);
			//DUMP_ASSET(ASSET_TYPE_PIXELSHADER, techset, MaterialPixelShader);
			////DUMP_ASSET(ASSET_TYPE_VERTEXDECL, techset, MaterialVertexDeclaration);
			//DUMP_ASSET(ASSET_TYPE_VERTEXSHADER, techset, MaterialVertexShader);

			//DUMP_ASSET_NO_CONVERT(ASSET_TYPE_AIPATHS, path_data, PathData);
			/*DUMP_ASSET(ASSET_TYPE_COL_MAP_MP, clip_map, clipMap_t);
			DUMP_ASSET(ASSET_TYPE_COMWORLD, comworld, ComWorld);
			DUMP_ASSET(ASSET_TYPE_FXWORLD, fxworld, FxWorld);
			DUMP_ASSET(ASSET_TYPE_GFXWORLD, gfxworld, GfxWorld);
			DUMP_ASSET(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld);*/
		}
		catch (std::exception& ex)
		{
			ZONETOOL_FATAL("A fatal exception occured while dumping zone \"%s\", exception was: \n%s", filesystem::get_fastfile().data(), ex.what());
		}

#undef DUMP_ASSET_NO_CONVERT
#undef DUMP_ASSET
	}

	std::unordered_map<game::game_mode, std::function<void(XAsset*)>> dump_functions =
	{
		{game::h1, dump_asset_h1},
		{game::h2, dump_asset_h2},
		{game::s1, dump_asset_s1},
		{game::iw7, dump_asset_iw7},
	};

	void dump_asset(XAsset* asset)
	{
		if (globals.verify)
		{
			ZONETOOL_INFO("Loading asset \"%s\" of type %s.", get_asset_name(asset), type_to_string(asset->type));
		}

		if (globals.dump_csv)
		{
			if (globals.csv_file.get_fp() == nullptr)
			{
				globals.csv_file = filesystem::file(filesystem::get_fastfile() + ".csv");
				globals.csv_file.open("wb");
			}

			// dump assets to disk
			if (globals.csv_file.get_fp())
			{
				std::fprintf(globals.csv_file.get_fp(), "%s,%s\n", type_to_string(asset->type), get_asset_name(asset));
			}
		}

		if (!globals.dump)
		{
			return;
		}

		if (asset_type_filter.size() > 0 && !asset_type_filter.contains(asset->type))
		{
			return;
		}

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

	void dump_refs()
	{
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

		referenced_assets.clear();
	}

	void stop_dumping()
	{
		globals.verify = false;

		if (globals.dump_csv)
		{
			globals.csv_file.close();
			globals.csv_file = {};
			globals.dump_csv = false;
		}

		if (!globals.dump)
		{
			return;
		}

		dump_refs();

		ZONETOOL_INFO("Zone \"%s\" dumped.", filesystem::get_fastfile().data());

		globals.dump = false;
	}

	utils::hook::detour db_link_x_asset_entry1_hook;
	XAssetEntry* db_link_x_asset_entry1(XAssetType type, XAssetHeader* header)
	{
		XAsset xasset = 
		{
			type,
			*header
		};

		if (type == ASSET_TYPE_IMAGE)
		{
			auto* asset = header->image;

			// patch reflection probe pixeldata, so that it will remain accessible, this is needed when converting from h1->iw7
			if (globals.dump && globals.target_game == game::game_mode::iw7 && 
				std::string(asset->name).find("*reflection_probe") != std::string::npos)
			{
				constexpr auto buffer_size = 1024 * 1024 * 32; // 32mb
				static std::uint8_t pixel_buffer[buffer_size];
				static std::uint32_t pixel_buffer_index = 0;

				if (pixel_buffer_index + asset->dataLen1 >= buffer_size)
				{
					ZONETOOL_FATAL("reflection probe pixeldata buffer is full...");
				}

				memcpy(&pixel_buffer[pixel_buffer_index], asset->pixelData, asset->dataLen1);

				auto ret = db_link_x_asset_entry1_hook.invoke<XAssetEntry*>(type, header);
				ret->asset.header.image->pixelData = &pixel_buffer[pixel_buffer_index];

				pixel_buffer_index += asset->dataLen1;

				//dump_asset(&xasset); // gfxworld generates reflection_probe array, no need to dump

				return ret;
			}
		}

		dump_asset(&xasset);
		
		return db_link_x_asset_entry1_hook.invoke<XAssetEntry*>(type, header);
	}

	utils::hook::detour db_finish_load_x_file_hook;
	void db_finish_load_x_file()
	{
		stop_dumping();
		return db_finish_load_x_file_hook.invoke<void>();
	}

	utils::hook::detour load_x_gfx_globals_hook;
	void load_x_gfx_globals(bool at_stream_start)
	{
		load_x_gfx_globals_hook.invoke<void>(at_stream_start);

		const auto var_x_gfx_globals = *reinterpret_cast<XGfxGlobals**>(0x143411DB0);
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
			for (auto i = 0u; i < *g_zoneCount; i++)
			{
				if (!_strnicmp(g_zones[i + 1].name, name.data(), 64))
				{
					if (inform)
					{
						ZONETOOL_INFO("Zone \"%s\" is already loaded...", name.data());
					}
					return false;
				}
			}
		}

		if (inform)
		{
			ZONETOOL_INFO("Loading zone \"%s\"...", name.data());
		}

		XZoneInfo zone = {name.data(), DB_ZONE_GAME | DB_ZONE_CUSTOM, 0};
		DB_LoadXAssets(&zone, 1, mode);
		return true;
	}

	void unload_zones()
	{
		static XZoneInfo zone = {0, DB_ZONE_CUSTOM, 70};
		DB_LoadXAssets(&zone, 1, DB_LOAD_ASYNC_FORCE_FREE);
		wait_for_database();

		ZONETOOL_INFO("Unloaded loaded zones...");
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
		globals.dump_csv = true;
		if (!load_zone(name, DB_LOAD_ASYNC, false))
		{
			globals.dump = false;
			globals.dump_csv = false;
			return;
		}

		while (globals.dump)
		{
			Sleep(1);
		}
	}

	void dump_csv(const std::string& name)
	{
		if (!zone_exists(name.data()))
		{
			ZONETOOL_INFO("Zone \"%s\" could not be found!", name.data());
			return;
		}

		wait_for_database();

		ZONETOOL_INFO("Dumping csv \"%s\"...", name.data());

		filesystem::set_fastfile(name);

		globals.dump_csv = true;
		if (!load_zone(name, DB_LOAD_ASYNC, true))
		{
			globals.dump_csv = false;
			return;
		}

		while (globals.dump_csv)
		{
			Sleep(1);
		}

		ZONETOOL_INFO("Csv \"%s\" dumped...", name.data());
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
			return;
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

	void parse_csv_file_ignore(const std::string& fastfile, const std::string& csv)
	{
		auto path = "zone_source\\" + csv + ".csv";
		auto parser = csv::parser(path.data(), ',');

		if (!parser.valid())
		{
			throw std::runtime_error(utils::string::va("Could not find csv file \"%s\"", csv.data()));
		}

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

			if (row->num_fields < 2 || !is_valid_asset_type(row->fields[0]))
			{
				continue;
			}

			std::string name;
			if ((!row->fields[1] || !strlen(row->fields[1]) && row->fields[2] && strlen(row->fields[2])))
			{
				continue;
			}
			else
			{
				name = row->fields[1];
			}

			const auto type = static_cast<std::uint32_t>(type_to_int(row->fields[0]));
			ignore_assets.insert(std::make_pair(type, name));
		}
	}

	void parse_csv_file(zone_base* zone, const std::string& fastfile, const std::string& csv)
	{
		auto path = "zone_source\\" + csv + ".csv";
		auto parser = csv::parser(path.data(), ',');

		if (!parser.valid())
		{
			throw std::runtime_error(utils::string::va("Could not find csv file \"%s\" to build zone!", csv.data()));
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
				parse_csv_file(zone, fastfile, row->fields[1]);
			}
			else if (row->fields[0] == "ignore"s)
			{
				parse_csv_file_ignore(fastfile, row->fields[1]);
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
					if (!is_valid_asset_type(row->fields[0]))
					{
						continue;
					}

					std::string name;
					if (row->num_fields > 1)
					{
						if (row->num_fields > 2 && (!row->fields[1] || !strlen(row->fields[1]) && row->fields[2] && strlen(row->fields[2])))
						{
							name = ","s + row->fields[2];
						}
						else
						{
							name = ((is_referencing) ? ","s : ""s) + row->fields[1];
						}
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

	dump_params get_dump_params(const ::h1::command::params& params)
	{
		dump_params dump_params{};
		dump_params.target = game::h1;

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

	void clear_asset_fields()
	{
		material::fixed_nml_images_map.clear();
		techset::vertexdecl_pointers.clear();
		xanim_parts::secondary_anims.clear();

		map_ents::clear_entity_strings();
	}

	void build_zone(const std::string& fastfile)
	{
		// make sure FS is correct.
		filesystem::set_fastfile(fastfile);

		ZONETOOL_INFO("Building fastfile \"%s\"", fastfile.data());

		ignore_assets.clear();
		clear_asset_fields();

		auto zone = alloc_zone(fastfile);
		if (zone == nullptr)
		{
			ZONETOOL_ERROR("An error occured while building fastfile \"%s\": Are you out of memory?", fastfile.data());
			return;
		}

		try
		{
			parse_csv_file(zone.get(), fastfile, fastfile);
		}
		catch (std::exception& ex)
		{
			ZONETOOL_ERROR("%s", ex.what());
			return;
		}

		// allocate zone buffer
		auto buffer = alloc_buffer();

		// add branding asset
		zone->add_asset_of_type("rawfile", fastfile);

		// compile zone
		zone->build(buffer.get());

		ignore_assets.clear();
		clear_asset_fields();
	}

	void iterate_zones()
	{
		const auto iterate_zones_internal = [](const std::string& path)
		{
			for (auto const& dir_entry : std::filesystem::directory_iterator{ path })
			{
				if (dir_entry.is_regular_file() && dir_entry.path().extension() == ".ff")
				{
					const auto zone = dir_entry.path().stem().string();

					load_zone(zone);

					wait_for_database();
					unload_zones();
				}
			}
		};

		const auto zone_path = utils::io::directory_exists("zone") ? "zone/" : "";
		iterate_zones_internal(zone_path);

		const auto lang_path = utils::io::directory_exists("zone") ? "zone/english/" : "english/";
		iterate_zones_internal(lang_path);
	}

	void register_commands()
	{
		::h1::command::add("quit", []()
		{
			std::exit(EXIT_SUCCESS);
		});

		::h1::command::add("buildzone", [](const ::h1::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: buildzone <zone>");
				return;
			}
			
			build_zone(params.get(1));
		});

		::h1::command::add("loadzone", [](const ::h1::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: loadzone <zone>");
				return;
			}
			
			load_zone(params.get(1));
		});

		::h1::command::add("unloadzones", []()
		{
			unload_zones();
		});

		::h1::command::add("verifyzone", [](const ::h1::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: verifyzone <zone>");
				return;
			}

			verify_zone(params.get(1));
		});

		::h1::command::add("dumpcsv", [](const ::h1::command::params& params)
		{
			if (params.size() != 2)
			{
				ZONETOOL_ERROR("usage: dumpcsv <zone>");
				return;
			}

			dump_csv(params.get(1));
		});

		::h1::command::add("dumpzone", [](const ::h1::command::params& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			asset_type_filter.clear();
			
			if (params.size() >= 3)
			{
				const auto mode = params.get(1);
				const auto dump_target = game::get_mode_from_string(mode);

				if (dump_target == game::none)
				{
					ZONETOOL_ERROR("Invalid dump target \"%s\"", mode);
					return;
				}

				if (!dump_functions.contains(dump_target))
				{
					ZONETOOL_ERROR("Unsupported dump target \"%s\" (%i)", mode, dump_target);
					return;
				}

				if (params.size() >= 4)
				{
					const auto asset_types_str = params.get(3);
					const auto asset_types = utils::string::split(asset_types_str, ',');

					for (const auto& type_str : asset_types)
					{
						const auto type = type_to_int(type_str);
						if (type == -1)
						{
							ZONETOOL_ERROR("Asset type \"%s\" does not exist", type_str.data());
							return;
						}

						asset_type_filter.insert(static_cast<XAssetType>(type));
					}
				}

				dump_zone(params.get(2), dump_target);
			}
			else
			{
				dump_zone(params.get(1), game::h1);
			}
		});


		::h1::command::add("dumpmatchingzones", [](const ::h1::command::params& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_ERROR("usage: dumpzone <zone>");
				return;
			}

			asset_type_filter.clear();

			auto dump_target = game::s1;
			auto zone_match = params.get(1);
			if (params.size() >= 3)
			{
				const auto mode = params.get(1);
				dump_target = game::get_mode_from_string(mode);

				if (dump_target == game::none)
				{
					ZONETOOL_ERROR("Invalid dump target \"%s\"", mode);
					return;
				}

				if (!dump_functions.contains(dump_target))
				{
					ZONETOOL_ERROR("Unsupported dump target \"%s\" (%i)", mode, dump_target);
					return;
				}

				if (params.size() >= 4)
				{
					const auto asset_types_str = params.get(3);
					const auto asset_types = utils::string::split(asset_types_str, ',');

					for (const auto& type_str : asset_types)
					{
						const auto type = type_to_int(type_str);
						if (type == -1)
						{
							ZONETOOL_ERROR("Asset type \"%s\" does not exist", type_str.data());
							return;
						}

						asset_type_filter.insert(static_cast<XAssetType>(type));
					}
				}

				zone_match = params.get(2);
			}

			const auto path = utils::io::directory_exists("zone") ? "zone" : "";
			const auto files = utils::io::list_files(path);
			for (const auto& file : files)
			{
				std::regex exp(zone_match);
				std::smatch match;
				if (file.ends_with(".ff") && std::regex_search(file, match, exp))
				{
					const auto last_slash = file.find_last_of('/');
					const auto end = (file.size() - 3) - last_slash - 1;
					const auto base_name = file.substr(last_slash + 1, end);
					dump_zone(base_name, dump_target);
					unload_zones();
				}
			}
		});

		::h1::command::add("dumpasset", [](const ::h1::command::params& params)
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
			globals.target_game = game::h1;
			dump_asset(&asset);

			ZONETOOL_INFO("Dumped to dump/assets");
		});

		::h1::command::add("dumpmap", [](const ::h1::command::params& params)
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

			const auto dump_zone_ = [&](const std::string& zone, 
				const std::optional<std::unordered_set<XAssetType>>& filter = {})
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

			auto common_filter = techset_filter;
			//common_filter.insert(ASSET_TYPE_IMAGE);

			unload_zones();

			auto zone_postfix = "";
			auto is_mp = false;
			if (dump_params.zone.starts_with("mp_"))
			{
				zone_postfix = "_mp";
				is_mp = true;
			}

			std::vector<std::pair<std::string, std::unordered_set<XAssetType>>> common_zones =
			{
				{"code_post_gfx"s + zone_postfix, techset_filter},
				{"techsets_common"s + zone_postfix, techset_filter},
				{"common"s + zone_postfix, common_filter},
				{"patch_common"s + zone_postfix, common_filter},
				{"techsets_common_core_mp", techset_filter},
			};

			for (const auto& zone : common_zones)
			{
				if (skip_common)
				{
					load_zone(zone.first);
				}
				else
				{
					dump_zone_(zone.first, zone.second);
				}
			}

			dump_zone_("techsets_" + dump_params.zone, dump_params.filter);
			dump_zone_("eng_patch_" + dump_params.zone, dump_params.filter);
			dump_zone_("patch_" + dump_params.zone, dump_params.filter);
			dump_zone_(dump_params.zone + "_path", dump_params.filter);
			dump_zone_(dump_params.zone + "_load", dump_params.filter);
			dump_zone_(dump_params.zone);

			ZONETOOL_INFO("Map \"%s\" dumped", dump_params.zone.data());
		});


		::h1::command::add("generatecsv", csv_generator::create_command
			<::h1::command::params>([](const uint32_t id)
		{
			return gsc::h1::gsc_ctx->token_name(id);
		}));

		::h1::command::add("iteratezones", []()
		{
			iterate_zones();
		});
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
		auto args = get_command_line_arguments();

		bool do_exit = false;
		if (args.size() <= 1)
		{
			return;
		}

		for (std::size_t i = 0; i < args.size(); ++i)
		{
			const auto& arg = args[i];

			// Commands that require a following parameter
			if (i + 1 < args.size())
			{
				const auto& param = args[i + 1];

				if (arg == "-loadzone")
				{
					load_zone(param);
					++i;

					do_exit = true;
				}
				else if (arg == "-buildzone")
				{
					build_zone(param);
					++i;

					do_exit = true;
				}
				else if (arg == "-buildzones")
				{
					std::string data;
					if (!utils::io::read_file(param, &data))
						return;

					auto zones = utils::string::split(data, '\n');
					for (auto& zone : zones)
					{
						if (zone.ends_with("\r"))
							zone.pop_back();

						build_zone(zone);
					}

					++i;

					do_exit = true;
				}
				else if (arg == "-verifyzone")
				{
					verify_zone(param);
					++i;

					do_exit = true;
				}
				else if (arg == "-dumpzone")
				{
					dump_zone(param, game::h1);
					++i;

					do_exit = true;
				}
				else if (arg == "-dumpcsv")
				{
					dump_csv(param);
					++i;

					do_exit = true;
				}
			}

			// Commands without parameters
			if (arg == "-unloadzones")
			{
				unload_zones();

				do_exit = true;
			}
			else if (arg == "-help")
			{
				ZONETOOL_INFO("Usage: zonetool.exe [options]");
				ZONETOOL_INFO("Options:");
				ZONETOOL_INFO("  -loadzone <zone>     Load a zone");
				ZONETOOL_INFO("  -buildzone <zone>    Build a zone");
				ZONETOOL_INFO("  -buildzones <file>   Build zones from a file");
				ZONETOOL_INFO("  -verifyzone <zone>   Verify a zone");
				ZONETOOL_INFO("  -dumpzone <zone>     Dump a zone");
				ZONETOOL_INFO("  -dumpcsv <zone>      Dump a CSV of a zone");
				ZONETOOL_INFO("  -unloadzones         Unload all zones");

				do_exit = true;
			}
		}

		if (do_exit)
		{
			wait_for_database();
			std::exit(EXIT_SUCCESS);
		}
	}

	void on_exit(void)
	{
		globals.verify = false;
		globals.dump = false;
		globals.dump_csv = false;
		globals.csv_file.close();
	}

	utils::hook::detour doexit_hook;
	void doexit(unsigned int a1, int a2, int a3)
	{
		on_exit();
		doexit_hook.invoke<void>(a1, a2, a3);
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
		reallocate_asset_pool_multiplier(ASSET_TYPE_XANIMPARTS, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_ATTACHMENT, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_TTF, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_SNDDRIVER_GLOBALS, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_EQUIPMENT_SND_TABLE, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_SOUND, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_LOADED_SOUND, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_LEADERBOARD, 2);
		reallocate_asset_pool_multiplier(ASSET_TYPE_VERTEXDECL, 6);
		reallocate_asset_pool_multiplier(ASSET_TYPE_COMPUTESHADER, 4);
		reallocate_asset_pool_multiplier(ASSET_TYPE_REVERB_PRESET, 2);

		// enable dumping
		db_link_x_asset_entry1_hook.create(0x1402BC920, &db_link_x_asset_entry1);

		// stop dumping
		db_finish_load_x_file_hook.create(0x14028DC30, &db_finish_load_x_file);

		// store xGfxGlobals pointers
		load_x_gfx_globals_hook.create(0x1402A8EA0, &load_x_gfx_globals);

		doexit_hook.create(0x1407948E0, doexit);
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