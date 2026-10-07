#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define IW7_ASSETS(x) \
	x(ASSET_TYPE_PHYSICSLIBRARY, physics_library, PhysicsLibrary) \
	x(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, physics_sfx_event, PhysicsSFXEventAsset) \
	x(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, physics_vfx_event, PhysicsVFXEventAsset) \
	x(ASSET_TYPE_PHYSICSASSET, physics_asset, PhysicsAsset) \
	x(ASSET_TYPE_PHYSICS_FX_PIPELINE, physics_fx_pipeline, PhysicsFXPipeline) \
	x(ASSET_TYPE_PHYSICS_FX_SHAPE, physics_fx_shape, PhysicsFXShape) \
	x(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts) \
	x(ASSET_TYPE_XMODEL_SURFS, xsurface, XModelSurfs) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_MAYHEM, mayhem, MayhemData) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_COMPUTESHADER, compute_shader, ComputeShader) \
	x(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader) \
	x(ASSET_TYPE_HULLSHADER, hull_shader, MaterialHullShader) \
	x(ASSET_TYPE_DOMAINSHADER, domain_shader, MaterialDomainShader) \
	x(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader) \
	x(ASSET_TYPE_VERTEXDECL, vertex_decl, MaterialVertexDeclaration) \
	x(ASSET_TYPE_TECHNIQUE_SET, techset, MaterialTechniqueSet) \
	x(ASSET_TYPE_IMAGE, gfx_image, GfxImage) \
	x(ASSET_TYPE_SOUND_GLOBALS, sound_globals, SndGlobals) \
	x(ASSET_TYPE_SOUND_BANK, sound_bank, SndBank) \
	x(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t) \
	x(ASSET_TYPE_COMWORLD, com_world, ComWorld) \
	x(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld) \
	x(ASSET_TYPE_PATHDATA, path_data, PathData) \
	x(ASSET_TYPE_NAVMESH, nav_mesh, NavMeshData) \
	x(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts) \
	x(ASSET_TYPE_FXWORLD, fx_world, FxWorld) \
	x(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld) \
	x(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, gfx_world_tr, GfxWorldTransientZone) \
	x(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef) \
	x(ASSET_TYPE_ANIMCLASS, animation_class, AnimationClass) \
	x(ASSET_TYPE_PLAYERANIM, player_anim_script, PlayerAnimScript) \
	x(ASSET_TYPE_GESTURE, gesture, Gesture) \
	x(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry) \
	x(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment) \
	x(ASSET_TYPE_WEAPON, weapon_def, WeaponCompleteDef) \
	x(ASSET_TYPE_VFX, particle_system, ParticleSystemDef) \
	x(ASSET_TYPE_FX, fx_effect_def, FxEffectDef) \
	x(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable) \
	x(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_LEADERBOARD, leaderboard, LeaderboardDef) \
	x(ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard, VirtualLeaderboardDef) \
	x(ASSET_TYPE_DDL, ddl, DDLFile) \
	x(ASSET_TYPE_TRACER, tracer, TracerDef) \
	x(ASSET_TYPE_VEHICLE, vehicle, VehicleDef) \
	x(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings) \
	x(ASSET_TYPE_LUA_FILE, lua_file, LuaFile) \
	x(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef) \
	x(ASSET_TYPE_EQUIPMENT_SND_TABLE, equipment_sound_table, EquipmentSoundTable) \
	x(ASSET_TYPE_VECTORFIELD, vector_field, VectorField) \
	x(ASSET_TYPE_PARTICLE_SIM_ANIMATION, fx_particle_sim_animation, FxParticleSimAnimation) \
	x(ASSET_TYPE_STREAMING_INFO, streaming_info, StreamingInfo) \
	x(ASSET_TYPE_LASER, laser, LaserDef) \
	x(ASSET_TYPE_TTF, ttf_def, TTFDef) \
	x(ASSET_TYPE_SUIT, suit, SuitDef) \
	x(ASSET_TYPE_SUITANIMPACKAGE, suit_anim_package, SuitAnimPackage) \
	x(ASSET_TYPE_SPACESHIPTARGET, spaceship_target, SpaceshipTargetDef) \
	x(ASSET_TYPE_RUMBLE, rumble, RumbleInfo) \
	x(ASSET_TYPE_RUMBLE_GRAPH, rumble_graph, RumbleGraph) \
	x(ASSET_TYPE_ANIM_PACKAGE, weapon_anim_package, WeaponAnimPackage) \
	x(ASSET_TYPE_SFX_PACKAGE, weapon_sfx_package, WeaponSFXPackage) \
	x(ASSET_TYPE_VFX_PACKAGE, weapon_vfx_package, WeaponVFXPackage) \
	x(ASSET_TYPE_XANIM_PROCEDURALBONES, xanim_procedural_bones, XAnimProceduralBones) \
	x(ASSET_TYPE_BEHAVIOR_TREE, behavior_tree, BehaviorTree) \
	x(ASSET_TYPE_RETICLE, reticle, ReticleDef) \
	x(ASSET_TYPE_GFXLIGHTMAP, gfx_light_map, GfxLightMap)

#include <assets/shared.hpp>

namespace zonetool::iw7
{
	inline void read_fx_combined(zone_reader& reader, FxCombinedDef& effect)
	{
		reader.read_asset(effect.type == FX_COMBINED_VFX ? ASSET_TYPE_VFX : ASSET_TYPE_FX, effect.u.data);
	}
}

#include "assets/animation_class.hpp"
#include "assets/behavior_tree.hpp"
#include "assets/ddl.hpp"
#include "assets/equipment_sound_table.hpp"
#include "assets/fxeffectdef.hpp"
#include "assets/fxparticlesimanimation.hpp"
#include "assets/gesture.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxlightdef.hpp"
#include "assets/gfxlightmap.hpp"
#include "assets/impactfx.hpp"
#include "assets/laser.hpp"
#include "assets/leaderboard.hpp"
#include "assets/localize.hpp"
#include "assets/luafile.hpp"
#include "assets/material.hpp"
#include "assets/mayhem.hpp"
#include "assets/netconststrings.hpp"
#include "assets/particle_system.hpp"
#include "assets/player_anim_script.hpp"
#include "assets/rawfile.hpp"
#include "assets/reticle.hpp"
#include "assets/rumble.hpp"
#include "assets/rumble_graph.hpp"
#include "assets/scriptabledef.hpp"
#include "assets/scriptfile.hpp"
#include "assets/spaceship_target.hpp"
#include "assets/streaming_info.hpp"
#include "assets/stringtable.hpp"
#include "assets/suit.hpp"
#include "assets/suit_anim_package.hpp"
#include "assets/surface_fx.hpp"
#include "assets/tracer.hpp"
#include "assets/ttfdef.hpp"
#include "assets/vectorfield.hpp"
#include "assets/vehicle.hpp"
#include "assets/virtual_leaderboard.hpp"
#include "assets/weapon_anim_package.hpp"
#include "assets/weapon_sfx_package.hpp"
#include "assets/weapon_vfx_package.hpp"
#include "assets/weaponattachment.hpp"
#include "assets/weapondef.hpp"
#include "assets/xanim.hpp"
#include "assets/xanim_procedural_bones.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

#include "assets/sound_globals.hpp"
#include "assets/sound_bank.hpp"

#include "assets/physics_asset.hpp"
#include "assets/physics_fx_pipeline.hpp"
#include "assets/physics_fx_shape.hpp"
#include "assets/physics_library.hpp"
#include "assets/physics_sfx_event.hpp"
#include "assets/physics_vfx_event.hpp"

#include "assets/techset.hpp"

#include "assets/computeshader.hpp"
#include "assets/domainshader.hpp"
#include "assets/hullshader.hpp"
#include "assets/pixelshader.hpp"
#include "assets/vertexdecl.hpp"
#include "assets/vertexshader.hpp"

#include "assets/aipaths.hpp"
#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfxworld.hpp"
#include "assets/gfxworld_tr.hpp"
#include "assets/glassworld.hpp"
#include "assets/mapents.hpp"
#include "assets/navmesh.hpp"

#include "zone/zone.hpp"

namespace zonetool::iw7
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
