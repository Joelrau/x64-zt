#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define IW8_ASSETS(x) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_KEY_VALUE_PAIRS, key_value_pairs, KeyValuePairs) \
	x(ASSET_TYPE_LOCALIZE_ENTRY, localize, LocalizeEntry) \
	x(ASSET_TYPE_STREAM_KEY, stream_key, StreamKey) \
	x(ASSET_TYPE_IMAGE, gfx_image, GfxImage) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_TECHNIQUE_SET, technique_set, MaterialTechniqueSet) \
	x(ASSET_TYPE_LIBSHADER, lib_shader, MaterialLibShader) \
	x(ASSET_TYPE_COMPUTESHADER, compute_shader, ComputeShader) \
	x(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader) \
	x(ASSET_TYPE_HULLSHADER, hull_shader, MaterialHullShader) \
	x(ASSET_TYPE_DOMAINSHADER, domain_shader, MaterialDomainShader) \
	x(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_XMODEL_SURFS, xmodel_surfs, XModelSurfs) \
	x(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts) \
	x(ASSET_TYPE_WEAPON, weapon, WeaponCompleteDef) \
	x(ASSET_TYPE_ATTACHMENT, weapon_attachment, WeaponAttachment) \
	x(ASSET_TYPE_VFX_PACKAGE, vfx_package, WeaponVFXPackage) \
	x(ASSET_TYPE_GESTURE, gesture, Gesture) \
	x(ASSET_TYPE_RETICLE, reticle_def, ReticleDef) \
	x(ASSET_TYPE_ANIM_PACKAGE, weapon_anim_package, WeaponAnimPackage) \
	x(ASSET_TYPE_SFX_PACKAGE, sfx_package, WeaponSFXPackage) \
	x(ASSET_TYPE_RUMBLE, rumble, RumbleInfo) \
	x(ASSET_TYPE_RUMBLE_GRAPH, rumble_graph, RumbleGraph) \
	x(ASSET_TYPE_TRACER, tracer_def, TracerDef) \
	x(ASSET_TYPE_LASER, laser_def, LaserDef) \
	x(ASSET_TYPE_BLEND_SPACE_2D, blend_space_2d, BlendSpace2DDef) \
	x(ASSET_TYPE_VFX, vfx, ParticleSystemDef) \
	x(ASSET_TYPE_VECTORFIELD, vector_field, VectorField) \
	x(ASSET_TYPE_FOOTSTEP_VFX, footstep_vfx, FootstepVFX) \
	x(ASSET_TYPE_LIGHT_DEF, light_def, GfxLightDef) \
	x(ASSET_TYPE_PARTICLE_SIM_ANIMATION, particle_sim_animation, FxParticleSimAnimation) \
	x(ASSET_TYPE_PHYSICS_FX_SHAPE, physics_fx_shape, PhysicsFXShape) \
	x(ASSET_TYPE_PHYSICS_FX_PIPELINE, physics_fx_pipeline, PhysicsFXPipeline) \
	x(ASSET_TYPE_PHYSICS_SFX_EVENT_ASSET, physics_sfx_event, PhysicsSFXEventAsset) \
	x(ASSET_TYPE_PHYSICS_VFX_EVENT_ASSET, physics_vfx_event, PhysicsVFXEventAsset) \
	x(ASSET_TYPE_PHYSICSASSET, physics_asset, PhysicsAsset) \
	x(ASSET_TYPE_PHYSICSLIBRARY, physics_library, PhysicsLibrary) \
	x(ASSET_TYPE_SOUND_GLOBALS, sound_globals, SndGlobals) \
	x(ASSET_TYPE_SOUND_BANK, sound_bank, SndBankResident) \
	x(ASSET_TYPE_SOUND_BANK_TRANSIENT, sound_bank_transient, SndBankTransient) \
	x(ASSET_TYPE_XMODEL_DETAIL_COLLISION, xmodel_detail_collision, XModelDetailCollision) \
	x(ASSET_TYPE_DECAL_VOLUME_MATERIAL, decal_volume_material, GfxDecalVolumeMaterial) \
	x(ASSET_TYPE_DECAL_VOLUME_MASK, decal_volume_mask, GfxDecalVolumeMask) \
	x(ASSET_TYPE_CLOTH_ASSET, cloth_asset, ClothAsset) \
	x(ASSET_TYPE_XANIM_PROCEDURALBONES, procedural_bones, XAnimProceduralBones) \
	x(ASSET_TYPE_SCRIPTABLE, scriptable_def, ScriptableDef) \
	x(ASSET_TYPE_XCOMPOSITEMODEL, composite_model, XCompositeModelDef) \
	x(ASSET_TYPE_CAMERA, camera_def, CameraDef) \
	x(ASSET_TYPE_HUDOUTLINE, hud_outline, HudOutlineDef) \
	x(ASSET_TYPE_FOG_SPLINE, fog_spline, GfxFogSpline) \
	x(ASSET_TYPE_GRADING_CLUT, grading_clut, GfxGradingClut) \
	x(ASSET_TYPE_EDGE_LIST, map_edge_list, MapEdgeList) \
	x(ASSET_TYPE_XANIM_DYNAMICBONES, dynamic_bones, XAnimDynamicBones) \
	x(ASSET_TYPE_XANIMCURVE, anim_curve, XAnimCurve) \
	x(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable) \
	x(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable) \
	x(ASSET_TYPE_NET_CONST_STRINGS, net_const_strings, NetConstStrings) \
	x(ASSET_TYPE_COMWORLD, com_world, ComWorld) \
	x(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld) \
	x(ASSET_TYPE_TACGRAPH, tactical_graph, TacticalGraphData) \
	x(ASSET_TYPE_DYN_ENTITY_LIST, dyn_entity_list, DynEntityList) \
	x(ASSET_TYPE_FXWORLD, fx_world, FxWorld) \
	x(ASSET_TYPE_FXWORLD_TRANSIENT_ZONE, fx_world_transient_zone, FxWorldTransientZone) \
	x(ASSET_TYPE_PATHDATA, path_data, PathData) \
	x(ASSET_TYPE_NAVMESH, nav_mesh, NavMeshData) \
	x(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts) \
	x(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t) \
	x(ASSET_TYPE_ST_TERRAIN, st_terrain, StTerrain) \
	x(ASSET_TYPE_STREAMING_INFO, streaming_info, StreamingInfo) \
	x(ASSET_TYPE_VEHICLE, vehicle, VehicleDef) \
	x(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld) \
	x(ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE, gfx_world_transient_zone, GfxWorldTransientZone)

#define IW8_READ_ONLY_ASSETS(x) \
	x(ASSET_TYPE_DLOG_SCHEMA, dlog_schema, DLogSchema) \
	x(ASSET_TYPE_LEADERBOARD, leaderboard_def, LeaderboardDef) \
	x(ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard_def, VirtualLeaderboardDef) \
	x(ASSET_TYPE_DDL, ddl_file, DDLFile) \
	x(ASSET_TYPE_LUA_FILE, lua_file, LuaFile) \
	x(ASSET_TYPE_SUITANIMPACKAGE, suit_anim_package, SuitAnimPackage) \
	x(ASSET_TYPE_SUIT, suit_def, SuitDef) \
	x(ASSET_TYPE_COVER_SELECTOR, cover_selector, CoverSelector) \
	x(ASSET_TYPE_ENEMY_SELECTOR, enemy_selector, EnemySelector) \
	x(ASSET_TYPE_CLIENT_CHARACTER, client_character, ClientCharacter) \
	x(ASSET_TYPE_CINEMATIC_MOTION, cinematic_motion, CinematicMotionDef) \
	x(ASSET_TYPE_ACCESSORY, accessory_def, AccessoryDef) \
	x(ASSET_TYPE_LOC_DMG_TABLE, loc_dmg_table, LocDmgTable) \
	x(ASSET_TYPE_BULLET_PENETRATION, bullet_penetration, BulletPenetration) \
	x(ASSET_TYPE_XCAM, xcam, XCam) \
	x(ASSET_TYPE_ANIMCLASS, anim_class, AnimationClass) \
	x(ASSET_TYPE_MAYHEM, mayhem, MayhemData) \
	x(ASSET_TYPE_CARRY_OBJECT, carry_object, CarryObjectDef) \
	x(ASSET_TYPE_SOUND_BANK_LIST, sound_bank_list, SoundBankListDef) \
	x(ASSET_TYPE_EXECUTION, execution_def, ExecutionDef) \
	x(ASSET_TYPE_CAMO, camo, Camo) \
	x(ASSET_TYPE_BEHAVIOR_TREE, behavior_tree, BehaviorTree) \
	x(ASSET_TYPE_AI_ANIM_SET, animset, Animset) \
	x(ASSET_TYPE_AI_ASM, ai_asm, ASM) \
	x(ASSET_TYPE_STREAM_TREE_OVERRIDE, stream_tree_override, StreamTreeOverride) \
	x(ASSET_TYPE_COLLISION_TILE, collision_tile, CollisionTile) \
	x(ASSET_TYPE_EQUIPMENT_SND_TABLE, equipment_sound_table, EquipmentSoundTable) \
	x(ASSET_TYPE_TTF, ttf_def, TTFDef)

#include <assets/shared.hpp>
#include "assets/asset_util.hpp"

#include "assets/accessory.hpp"
#include "assets/animpackage.hpp"
#include "assets/animset.hpp"
#include "assets/behaviortree.hpp"
#include "assets/blendspace2d.hpp"
#include "assets/bulletpenetration.hpp"
#include "assets/camera.hpp"
#include "assets/camo.hpp"
#include "assets/cinematicmotion.hpp"
#include "assets/clientcharacter.hpp"
#include "assets/clipmap.hpp"
#include "assets/clothasset.hpp"
#include "assets/collisiontile.hpp"
#include "assets/comworld.hpp"
#include "assets/coverselector.hpp"
#include "assets/ddl.hpp"
#include "assets/decalvolumemask.hpp"
#include "assets/decalvolumematerial.hpp"
#include "assets/detailcollision.hpp"
#include "assets/dlogschema.hpp"
#include "assets/dynamicbones.hpp"
#include "assets/dynentitylist.hpp"
#include "assets/edgelist.hpp"
#include "assets/enemyselector.hpp"
#include "assets/equipsndtable.hpp"
#include "assets/fogspline.hpp"
#include "assets/fxworld.hpp"
#include "assets/gesture.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxworld.hpp"
#include "assets/gfxworldtransientzone.hpp"
#include "assets/glassworld.hpp"
#include "assets/gradingclut.hpp"
#include "assets/hudoutline.hpp"
#include "assets/impactfx.hpp"
#include "assets/keyvaluepairs.hpp"
#include "assets/laser.hpp"
#include "assets/leaderboard.hpp"
#include "assets/lightdef.hpp"
#include "assets/localize.hpp"
#include "assets/locdmgtable.hpp"
#include "assets/luafile.hpp"
#include "assets/mapents.hpp"
#include "assets/material.hpp"
#include "assets/mayhem.hpp"
#include "assets/animclass.hpp"
#include "assets/carryobject.hpp"
#include "assets/navmesh.hpp"
#include "assets/netconststrings.hpp"
#include "assets/particlesimanimation.hpp"
#include "assets/pathdata.hpp"
#include "assets/physics.hpp"
#include "assets/physicslibrary.hpp"
#include "assets/proceduralbones.hpp"
#include "assets/rawfile.hpp"
#include "assets/reticle.hpp"
#include "assets/rumble.hpp"
#include "assets/scriptable.hpp"
#include "assets/scriptfile.hpp"
#include "assets/sound.hpp"
#include "assets/streaminginfo.hpp"
#include "assets/streamkey.hpp"
#include "assets/streamtreeoverride.hpp"
#include "assets/stringtable.hpp"
#include "assets/stterrain.hpp"
#include "assets/suit.hpp"
#include "assets/suitanimpackage.hpp"
#include "assets/tacgraph.hpp"
#include "assets/tracer.hpp"
#include "assets/ttfdef.hpp"
#include "assets/vehicle.hpp"
#include "assets/vfx.hpp"
#include "assets/weapon.hpp"
#include "assets/xanim.hpp"
#include "assets/xcam.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

namespace zonetool::iw8
{
	const char* type_to_string(XAssetType type);
	std::int32_t type_to_int(const std::string& type);
	bool is_valid_asset_type(const std::string& type);
}

#include "zone/zone.hpp"

namespace zonetool::iw8
{
	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;
	extern std::unordered_map<std::string, std::string> renamed_assets;

	std::string renamed_asset_name(const std::string& name);
}
