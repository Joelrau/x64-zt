#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define S2_ASSETS(x) \
	x(ASSET_TYPE_SNDPHYSPRESET, snd_phys_preset, SndPhysPreset) \
	x(ASSET_TYPE_PHYSCONSTRAINT, phys_constraint, PhysConstraint) \
	x(ASSET_TYPE_VERTEXDECL, vertex_decl, MaterialVertexDeclaration) \
	x(ASSET_TYPE_SOUND_CURVE, snd_curve, SndCurve) \
	x(ASSET_TYPE_DIST_CURVE, dist_curve, DistCurve) \
	x(ASSET_TYPE_REVERB_CURVE, reverb_curve, SndCurve) \
	x(ASSET_TYPE_SOUND_CONTEXT, snd_context, SndContext) \
	x(ASSET_TYPE_ALIAS_PARAMETER_MODIFIER, snd_alias_parameter_modifier, SndAliasParameterModifier) \
	x(ASSET_TYPE_ALIAS_COMBAT_CONE, snd_alias_combat_cone, SndAliasCombatCone) \
	x(ASSET_TYPE_SNDDRIVER_GLOBALS, snd_driver_globals, SndDriverGlobals) \
	x(ASSET_TYPE_REVERB_PRESET, reverb_preset, ReverbPreset) \
	x(ASSET_TYPE_COMPUTESHADER, compute_shader, ComputeShader) \
	x(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader) \
	x(ASSET_TYPE_HULLSHADER, hull_shader, MaterialHullShader) \
	x(ASSET_TYPE_DOMAINSHADER, domain_shader, MaterialDomainShader) \
	x(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader) \
	x(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_LUA_FILE, lua_file, LuaFile) \
	x(ASSET_TYPE_TTF, ttf_def, TTFDef) \
	x(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings) \
	x(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset) \
	x(ASSET_TYPE_PHYS_CLOTH_TUNING, phys_cloth_tuning, PhysClothTuning) \
	x(ASSET_TYPE_TRACER, tracer_def, TracerDef) \
	x(ASSET_TYPE_PHYSWATERPRESET, phys_water_preset, PhysWaterPreset) \
	x(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound) \
	x(ASSET_TYPE_SKELETON_SCRIPT, skeleton_script, SkeletonScript) \
	x(ASSET_TYPE_IMAGE, gfx_image, GfxImage) \
	x(ASSET_TYPE_CLUT, clut, Clut) \
	x(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_XSURFACE_SHARED, xsurface_shared, XSurfaceShared) \
	x(ASSET_TYPE_XMODEL_SURFS, xmodel_surfs, XModelSurfs) \
	x(ASSET_TYPE_XMODEL_BASE, xmodel_base, XModelBase) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap) \
	x(ASSET_TYPE_XANIM, xanim_parts, XAnimParts) \
	x(ASSET_TYPE_SOUND, sound, snd_alias_list_t) \
	x(ASSET_TYPE_SOUNDSUBMIX, sound_submix, SoundSubmix) \
	x(ASSET_TYPE_SNDMUSICSET, snd_music_set, SndMusicSet) \
	x(ASSET_TYPE_LIGHT_DEF, light_def, LightDef) \
	x(ASSET_TYPE_PARTICLE_SIM_ANIMATION, particle_sim_animation, FxParticleSimAnimation) \
	x(ASSET_TYPE_FX, fx_effect_def, FxEffectDef) \
	x(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable) \
	x(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable) \
	x(ASSET_TYPE_VECTORFIELD, vector_field, VectorField) \
	x(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment) \
	x(ASSET_TYPE_WEAPON, weapon_def, WeaponCompleteDef) \
	x(ASSET_TYPE_LASER, laser_def, LaserDef) \
	x(ASSET_TYPE_BEAM, beam_def, BeamDef) \
	x(ASSET_TYPE_LEADERBOARDDEF, leaderboard, LeaderboardDef) \
	x(ASSET_TYPE_DDL, ddl, DDLRoot) \
	x(ASSET_TYPE_SPLINES, splines, Spline) \
	x(ASSET_TYPE_DLOG_SCHEMA, dlog_schema, DLogSchema) \
	x(ASSET_TYPE_DLOG_ROUTES, dlog_routes, DLogRoutes) \
	x(ASSET_TYPE_ANIMCLASS, anim_class, AnimationClass) \
	x(ASSET_TYPE_VEHICLE, vehicle_def, VehicleDef) \
	x(ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard, VirtualLeaderboardDef) \
	x(ASSET_TYPE_EQUIPMENT_SND_TABLE, equipment_snd_table, EquipmentSndTable) \
	x(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef) \
	x(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, gfx_map_trzone, GfxMapTrZone) \
	x(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, clip_map_trzone, ClipMapTrZone) \
	x(ASSET_TYPE_COMWORLD, com_world, ComWorld) \
	x(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld) \
	x(ASSET_TYPE_NAVMESH, nav_mesh, NavMeshData) \
	x(ASSET_TYPE_FXWORLD, fx_world, FxWorld) \
	x(ASSET_TYPE_PATHDATA, path_data, PathData) \
	x(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts) \
	x(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t) \
	x(ASSET_TYPE_ADDON_MAP_ENTS, addon_map_ents, AddonMapEnts) \
	x(ASSET_TYPE_PHYSWORLDMAP, phys_worldmap, PhysWorld) \
	x(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld)

#include <assets/shared.hpp>

namespace zonetool::s2
{
	const char* type_to_string(XAssetType type);
	std::int32_t type_to_int(const std::string& type);
	bool is_valid_asset_type(const std::string& type);
	void set_zone_key_folder(const std::filesystem::path& folder);
}

#include "assets/asset_base.hpp"
#include "assets/name_only.hpp"
#include "assets/netconststrings.hpp"
#include "assets/techset.hpp"
#include "assets/xsurfaceshared.hpp"
#include "assets/gfximage.hpp"
#include "assets/physpreset.hpp"
#include "assets/loadedsound.hpp"
#include "assets/sound.hpp"
#include "assets/aipaths.hpp"
#include "assets/material.hpp"
#include "assets/physcollmap.hpp"
#include "assets/xmodelsurfs.hpp"
#include "assets/skeletonscript.hpp"
#include "assets/xmodel.hpp"
#include "assets/fx.hpp"
#include "assets/xanim.hpp"
#include "assets/tracerdef.hpp"
#include "assets/leaderboard.hpp"
#include "assets/ddl.hpp"
#include "assets/gfxworld_trzone.hpp"
#include "assets/scriptable.hpp"
#include "assets/splines.hpp"
#include "assets/dlogschema.hpp"
#include "assets/clipmap_trzone.hpp"
#include "assets/mapents.hpp"
#include "assets/attachment.hpp"
#include "assets/weapon.hpp"
#include "assets/animclass.hpp"
#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/navmesh.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfxworld.hpp"
#include "assets/vehicle.hpp"

#include "zone/zone.hpp"

namespace zonetool::s2
{
	REGISTER_TEMPLATED_ASSET_CLASS(compute_shader, shader, ComputeShader, ASSET_TYPE_COMPUTESHADER, shader_type::computeshader);
	REGISTER_TEMPLATED_ASSET_CLASS(vertex_shader, shader, MaterialVertexShader, ASSET_TYPE_VERTEXSHADER, shader_type::vertexshader);
	REGISTER_TEMPLATED_ASSET_CLASS(hull_shader, shader, MaterialHullShader, ASSET_TYPE_HULLSHADER, shader_type::hullshader);
	REGISTER_TEMPLATED_ASSET_CLASS(domain_shader, shader, MaterialDomainShader, ASSET_TYPE_DOMAINSHADER, shader_type::domainshader);
	REGISTER_TEMPLATED_ASSET_CLASS(pixel_shader, shader, MaterialPixelShader, ASSET_TYPE_PIXELSHADER, shader_type::pixelshader);
	REGISTER_TEMPLATED_ASSET(localize, LocalizeEntry, ASSET_TYPE_LOCALIZE_ENTRY);
	REGISTER_TEMPLATED_ASSET(rawfile, RawFile, ASSET_TYPE_RAWFILE);
	REGISTER_TEMPLATED_ASSET(scriptfile, ScriptFile, ASSET_TYPE_SCRIPTFILE);
	REGISTER_TEMPLATED_ASSET(string_table, StringTable, ASSET_TYPE_STRINGTABLE);
	REGISTER_TEMPLATED_ASSET(lua_file, LuaFile, ASSET_TYPE_LUA_FILE);
	REGISTER_TEMPLATED_ASSET(ttf_def, TTFDef, ASSET_TYPE_TTF);

	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	XAssetHeader db_find_x_asset_header(XAssetType type, const char* name, int create_default);
	XAssetHeader db_find_x_asset_header_safe(XAssetType type, const std::string& name);
}
