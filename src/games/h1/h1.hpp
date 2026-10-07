#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define H1_ASSETS(x) \
	x(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset) \
	x(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap) \
	x(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset, PhysWaterPreset) \
	x(ASSET_TYPE_PHYSWORLDMAP, phys_world, PhysWorld) \
	x(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint, PhysConstraint) \
	x(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts) \
	x(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_COMPUTESHADER, compute_shader, ComputeShader) \
	x(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader) \
	x(ASSET_TYPE_HULLSHADER, hull_shader, MaterialHullShader) \
	x(ASSET_TYPE_DOMAINSHADER, domain_shader, MaterialDomainShader) \
	x(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader) \
	x(ASSET_TYPE_VERTEXDECL, vertex_decl, MaterialVertexDeclaration) \
	x(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet) \
	x(ASSET_TYPE_IMAGE, gfx_image, GfxImage) \
	x(ASSET_TYPE_SOUND, sound, snd_alias_list_t) \
	x(ASSET_TYPE_SOUND_SUBMIX, sound_submix, SndSubmixList) \
	x(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve) \
	x(ASSET_TYPE_LPF_CURVE, lpf_curve, SndCurve) \
	x(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve) \
	x(ASSET_TYPE_SOUND_CONTEXT, sound_context, SndContext) \
	x(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound) \
	x(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t) \
	x(ASSET_TYPE_COMWORLD, com_world, ComWorld) \
	x(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld) \
	x(ASSET_TYPE_PATHDATA, path_data, PathData) \
	x(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts) \
	x(ASSET_TYPE_FXWORLD, fx_world, FxWorld) \
	x(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld) \
	x(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef) \
	x(ASSET_TYPE_MENULIST, menu_list, MenuList) \
	x(ASSET_TYPE_MENU, menu_def, menuDef_t) \
	x(ASSET_TYPE_ANIMCLASS, anim_class, AnimationClass) \
	x(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry) \
	x(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment) \
	x(ASSET_TYPE_WEAPON, weapon_def, WeaponDef) \
	x(ASSET_TYPE_SNDDRIVER_GLOBALS, sound_driver_globals, SndDriverGlobals) \
	x(ASSET_TYPE_FX, fx_effect_def, FxEffectDef) \
	x(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable) \
	x(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_LEADERBOARD, leaderboard, LeaderboardDef) \
	x(ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard, VirtualLeaderboardDef) \
	x(ASSET_TYPE_STRUCTURED_DATA_DEF, structured_data_def_set, StructuredDataDefSet) \
	x(ASSET_TYPE_DDL, ddl, DDLRoot) \
	x(ASSET_TYPE_TRACER, tracer_def, TracerDef) \
	x(ASSET_TYPE_VEHICLE, vehicle_def, VehicleDef) \
	x(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings) \
	x(ASSET_TYPE_REVERB_PRESET, reverb_preset, ReverbPreset) \
	x(ASSET_TYPE_LUA_FILE, lua_file, LuaFile) \
	x(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef) \
	x(ASSET_TYPE_EQUIPMENT_SND_TABLE, equip_snd_table, EquipmentSoundTable) \
	x(ASSET_TYPE_VECTORFIELD, vector_field, VectorField) \
	x(ASSET_TYPE_DOPPLER_PRESET, doppler_preset, DopplerPreset) \
	x(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation) \
	x(ASSET_TYPE_LASER, laser_def, LaserDef) \
	x(ASSET_TYPE_SKELETON_SCRIPT, skeleton_script, SkeletonScript) \
	x(ASSET_TYPE_CLUT, clut, Clut) \
	x(ASSET_TYPE_TTF, ttf_def, TTFDef)

#define H1_READ_ONLY_ASSETS(x) \
	x(ASSET_TYPE_VEHICLE_TRACK, vehicle_track, VehicleTrack) \
	x(ASSET_TYPE_PROTO, proto, Proto) \
	x(ASSET_TYPE_ADDON_MAP_ENTS, addon_map_ents, AddonMapEnts)

#include <assets/shared.hpp>

#include "assets/clut.hpp"
#include "assets/dopplerpreset.hpp"
#include "assets/fxeffectdef.hpp"
#include "assets/fxparticlesimanimation.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxlightdef.hpp"
#include "assets/impactfx.hpp"
#include "assets/laserdef.hpp"
#include "assets/loadedsound.hpp"
#include "assets/localize.hpp"
#include "assets/lpfcurve.hpp"
#include "assets/luafile.hpp"
#include "assets/mapents.hpp"
#include "assets/material.hpp"
#include "assets/netconststrings.hpp"
#include "assets/physcollmap.hpp"
#include "assets/physconstraint.hpp"
#include "assets/physpreset.hpp"
#include "assets/physwaterpreset.hpp"
#include "assets/physworld.hpp"
#include "assets/proto.hpp"
#include "assets/rawfile.hpp"
#include "assets/reverbcurve.hpp"
#include "assets/reverbpreset.hpp"
#include "assets/scriptabledef.hpp"
#include "assets/scriptfile.hpp"
#include "assets/skeletonscript.hpp"
#include "assets/sound.hpp"
#include "assets/soundcontext.hpp"
#include "assets/soundcurve.hpp"
#include "assets/sounddriverglobals.hpp"
#include "assets/soundsubmix.hpp"
#include "assets/stringtable.hpp"
#include "assets/structureddatadefset.hpp"
#include "assets/surfacefx.hpp"
#include "assets/techset.hpp"
#include "assets/tracerdef.hpp"
#include "assets/ttfdef.hpp"
#include "assets/vehicledef.hpp"
#include "assets/vehicletrack.hpp"
#include "assets/weaponattachment.hpp"
#include "assets/weapondef.hpp"
#include "assets/xanim.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

#include "assets/virtualleaderboard.hpp"
#include "assets/leaderboard.hpp"
#include "assets/ddl.hpp"
#include "assets/equipsndtable.hpp"
#include "assets/vectorfield.hpp"
#include "assets/animclass.hpp"

#include "assets/computeshader.hpp"
#include "assets/domainshader.hpp"
#include "assets/hullshader.hpp"
#include "assets/pixelshader.hpp"
#include "assets/vertexdecl.hpp"
#include "assets/vertexshader.hpp"

#include "assets/menudef.hpp"
#include "assets/menulist.hpp"

#include "assets/addonmapents.hpp"
#include "assets/aipaths.hpp"
#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfxworld.hpp"
#include "assets/glassworld.hpp"

#include "zone/zone.hpp"

namespace zonetool::h1
{
	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	const char* type_to_string(XAssetType type);
	std::int32_t type_to_int(const std::string& type);
	bool is_valid_asset_type(const std::string& type);

	XAssetHeader db_find_x_asset_header(XAssetType type, const char* name, int create_default);
	XAssetHeader db_find_x_asset_header_safe(XAssetType type, const std::string& name);

	template <typename T>
	XAssetHeader db_find_x_asset_header_copy(XAssetType type, const std::string& name, zone_memory* mem)
	{
		auto header = db_find_x_asset_header_safe(type, name);
		if (header.data)
		{
			T* newData = mem->allocate<T>();
			memcpy(newData, header.data, sizeof(T));
			header.data = reinterpret_cast<void*>(newData);
		}
		return header;
	}
}
