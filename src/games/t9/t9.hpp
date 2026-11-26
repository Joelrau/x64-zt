#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define T9_READ_ONLY_ASSETS(x) \
	x(ASSET_TYPE_TECHSET, techset, MaterialTechniqueSet) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_XSKELETON, xskeleton, XSkeleton) \
	x(ASSET_TYPE_IMAGE, image, GfxImage) \
	x(ASSET_TYPE_SOUND_BANK, sound_bank, SndBank) \
	x(ASSET_TYPE_SOUND_ASSET, sound_asset, SndAsset) \
	x(ASSET_TYPE_SOUND_DUCK, sound_duck, SndDuck) \
	x(ASSET_TYPE_SOUND_ALIAS_MODIFIER, sound_alias_modifier, SndAliasModifier) \
	x(ASSET_TYPE_SOUND_ACOUSTICS, sound_acoustics, SndAcoustics) \
	x(ASSET_TYPE_SNDDRIVERGLOBALS, snd_driver_globals, SndDriverGlobals) \
	x(ASSET_TYPE_LOCALIZEENTRY, localize, LocalizeEntry) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTPARSETREE, script_parse_tree, ScriptParseTree) \
	x(ASSET_TYPE_SCRIPT_USING, script_using, ScriptUsing) \
	x(ASSET_TYPE_SCRIPT_USING_CP, script_using, ScriptUsing) \
	x(ASSET_TYPE_SCRIPT_USING_MP, script_using, ScriptUsing) \
	x(ASSET_TYPE_SCRIPT_USING_WZ, script_using, ScriptUsing) \
	x(ASSET_TYPE_SCRIPT_USING_ZM, script_using, ScriptUsing) \
	x(ASSET_TYPE_DDL, ddl, DDLFile) \
	x(ASSET_TYPE_SURFACESOUNDDEF, surface_sound_def, SurfaceSoundDef) \
	x(ASSET_TYPE_ENTITYSOUNDIMPACTS, entity_sound_impacts, EntitySoundImpacts) \
	x(ASSET_TYPE_IMPACTSOUNDSTABLE, impact_sounds_table, ImpactSoundsTable) \
	x(ASSET_TYPE_KEYVALUEPAIRS, key_value_pairs, KeyValuePairs) \
	x(ASSET_TYPE_STREAMKEY, stream_key, StreamKey) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_XMODELMESH, xmodel_mesh, XModelMesh) \
	x(ASSET_TYPE_XCOLLISION, xcollision, XCollision) \
	x(ASSET_TYPE_TRIGGERACTIONS, trigger_actions, TriggerActions) \
	x(ASSET_TYPE_PHYSPRESET, physpreset, PhysPreset) \
	x(ASSET_TYPE_PHYSCONSTRAINTS, physconstraints, PhysConstraints) \
	x(ASSET_TYPE_RAGDOLL, ragdoll, Ragdoll) \
	x(ASSET_TYPE_XANIMCURVE, xanim_curve, XAnimCurve) \
	x(ASSET_TYPE_FX, fx, FxEffectDef) \
	x(ASSET_TYPE_KLF, klf, Klf) \
	x(ASSET_TYPE_IMPACTSFXTABLE, fx_impact_table, FxImpactTable) \
	x(ASSET_TYPE_SURFACEFXTABLE, surface_fx_table, SurfaceFxTable) \
	x(ASSET_TYPE_ENTITYFXIMPACTS, entity_fx_impacts, EntityFxImpacts) \
	x(ASSET_TYPE_FXLIBRARYVOLUME, fx_library_volume, FxLibraryVolume) \
	x(ASSET_TYPE_VECTORFIELD, vector_field, VectorField) \
	x(ASSET_TYPE_GLASSES, glasses, Glasses) \
	x(ASSET_TYPE_TRIGGERLIST, trigger_list, TriggerList) \
	x(ASSET_TYPE_XANIM, xanim, XAnim) \
	x(ASSET_TYPE_DESTRUCTIBLEDEF, destructible_def, DestructibleDef) \
	x(ASSET_TYPE_ZBARRIER, zbarrier, ZBarrier) \
	x(ASSET_TYPE_SCRIPTBUNDLE, script_bundle, ScriptBundle) \
	x(ASSET_TYPE_ENTITYLIST, entity_list, EntityList) \
	x(ASSET_TYPE_CPU_OCCLUSION_DATA, cpu_occlusion_data, CpuOcclusionData) \
	x(ASSET_TYPE_COM_MAP, com_world, ComWorld) \
	x(ASSET_TYPE_WINDDEF, wind_def, WindDef) \
	x(ASSET_TYPE_RUMBLE, rumble, RumbleInfo) \
	x(ASSET_TYPE_LIGHTING, lighting, Lighting) \
	x(ASSET_TYPE_TERRAINGFX, terrain_gfx, TerrainGfx) \
	x(ASSET_TYPE_STREAMERWORLD, streamer_world, StreamerWorld) \
	x(ASSET_TYPE_STREAMERHINT, streamer_hint, StreamerHint) \
	x(ASSET_TYPE_DYNMODEL, dyn_model, DynModel) \
	x(ASSET_TYPE_CLIP_MAP, clip_map, ClipMap) \
	x(ASSET_TYPE_NAVMESH, nav_mesh, NavMesh) \
	x(ASSET_TYPE_NAVVOLUME, nav_volume, NavVolume) \
	x(ASSET_TYPE_BLACKBOARD, blackboard, Blackboard) \
	x(ASSET_TYPE_GAME_MAP, game_map, GameMap) \
	x(ASSET_TYPE_SANIM, sanim, SAnim) \
	x(ASSET_TYPE_GROUPLODMODEL, group_lod_model, GroupLodModel) \
	x(ASSET_TYPE_GFX_MAP, gfx_world, GfxWorld) \
	x(ASSET_TYPE_STATICLEVELFXLIST, static_level_fx_list, StaticLevelFxList) \
	x(ASSET_TYPE_DISTRICTS, districts, Districts) \
	x(ASSET_TYPE_XCAM, xcam, XCam) \
	x(ASSET_TYPE_BGCACHE, bg_cache, BGCache) \
	x(ASSET_TYPE_POSTFXBUNDLE, post_fx_bundle, PostFxBundle) \
	x(ASSET_TYPE_RENDEROVERRIDEBUNDLE, render_override_bundle, RenderOverrideBundle) \
	x(ASSET_TYPE_GESTURE, gesture, Gesture) \
	x(ASSET_TYPE_GESTURETABLE, gesture_table, GestureTable) \
	x(ASSET_TYPE_WEAPONSTYLESETTINGS, weapon_style_settings, WeaponStyleSettings) \
	x(ASSET_TYPE_WEAPONTUNABLES, weapon_tunables, WeaponTunables) \
	x(ASSET_TYPE_WEAPONSECONDARYMOVEMENT, weapon_secondary_movement, WeaponSecondaryMovement) \
	x(ASSET_TYPE_CINEMATICMOTION, cinematic_motion, CinematicMotion) \
	x(ASSET_TYPE_WEAPONCAMO, weapon_camo, WeaponCamo) \
	x(ASSET_TYPE_WEAPONCAMOBINDING, weapon_camo_binding, WeaponCamoBinding) \
	x(ASSET_TYPE_SHAREDWEAPONSOUNDS, shared_weapon_sounds, SharedWeaponSounds) \
	x(ASSET_TYPE_WEAPONBLUEPRINT, weapon_blueprint, WeaponBlueprint) \
	x(ASSET_TYPE_ATTACHMENT, attachment, Attachment) \
	x(ASSET_TYPE_ATTACHMENTUNIQUE, attachment_unique, AttachmentUnique) \
	x(ASSET_TYPE_WEAPON, weapon, Weapon) \
	x(ASSET_TYPE_WEAPONFRONTEND, weapon_frontend, WeaponFrontend) \
	x(ASSET_TYPE_TAGFX, tag_fx, TagFx) \
	x(ASSET_TYPE_BEAM, beam, Beam) \
	x(ASSET_TYPE_TRACER, tracer, Tracer) \
	x(ASSET_TYPE_LASER, laser, Laser) \
	x(ASSET_TYPE_FLAMETABLE, flame_table, FlameTable) \
	x(ASSET_TYPE_BALLISTICDESC, ballistic_desc, BallisticDesc) \
	x(ASSET_TYPE_TRIGGEREFFECTDESC, trigger_effect_desc, TriggerEffectDesc) \
	x(ASSET_TYPE_SHELLSHOCK, shell_shock, ShellShock) \
	x(ASSET_TYPE_STATUSEFFECT, status_effect, StatusEffect) \
	x(ASSET_TYPE_FONTICON, font_icon, FontIcon) \
	x(ASSET_TYPE_OBJECTIVE, objective, Objective) \
	x(ASSET_TYPE_RAWTEXTFILE, raw_text_file, RawTextFile) \
	x(ASSET_TYPE_ANIMTREE, anim_tree, AnimTree) \
	x(ASSET_TYPE_FOOTSTEPTABLE, footstep_table, FootstepTable) \
	x(ASSET_TYPE_AIMTABLE, aim_table, AimTable) \
	x(ASSET_TYPE_SHOOTTABLE, shoot_table, ShootTable) \
	x(ASSET_TYPE_ANIMSELECTORTABLE, anim_selector_table, AnimSelectorTable) \
	x(ASSET_TYPE_ANIMMAPPINGTABLE, anim_mapping_table, AnimMappingTable) \
	x(ASSET_TYPE_ANIMSTATEMACHINE, anim_state_machine, AnimStateMachine) \
	x(ASSET_TYPE_BEHAVIORTREE, behavior_tree, BehaviorTree) \
	x(ASSET_TYPE_BEHAVIORSTATEMACHINE, behavior_state_machine, BehaviorStateMachine) \
	x(ASSET_TYPE_XMODELALIAS, xmodel_alias, XModelAlias) \
	x(ASSET_TYPE_CHARACTER, character, Character) \
	x(ASSET_TYPE_AITYPE, ai_type, AIType) \
	x(ASSET_TYPE_RAWFILEPREPROC, raw_text_file, RawTextFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_STRUCTUREDTABLE, structured_table, StructuredTable) \
	x(ASSET_TYPE_FEATURETABLE, feature_table, FeatureTable) \
	x(ASSET_TYPE_PLAYLISTS, playlists, Playlists) \
	x(ASSET_TYPE_PLAYLISTSCHEDULE, playlist_schedule, PlaylistSchedule) \
	x(ASSET_TYPE_ARENASEASONS, arena_seasons, ArenaSeasons) \
	x(ASSET_TYPE_SCENARIO, scenario, Scenario) \
	x(ASSET_TYPE_SCRIPTBUNDLELIST, script_bundle_list, ScriptBundleList) \
	x(ASSET_TYPE_VEHICLEFXDEF, vehicle_fx_def, VehicleFxDef) \
	x(ASSET_TYPE_OVERHEADCAMERATUNABLES, overhead_camera_tunables, OverheadCameraTunables) \
	x(ASSET_TYPE_TTF, ttf, TTFDef) \
	x(ASSET_TYPE_COMPASSTUNABLES, compass_tunables, CompassTunables) \
	x(ASSET_TYPE_OBJECTIVELIST, objective_list, ObjectiveList) \
	x(ASSET_TYPE_MAPTABLEENTRY, map_table_entry, MapTableEntry) \
	x(ASSET_TYPE_MAPTABLE, map_table, MapTable) \
	x(ASSET_TYPE_MAPTABLELIST, map_table_list, MapTableList) \
	x(ASSET_TYPE_PLAYERGLOBALTUNABLES, player_global_tunables, PlayerGlobalTunables) \
	x(ASSET_TYPE_VEHICLESOUNDDEF, vehicle_sound_def, VehicleSoundDef) \
	x(ASSET_TYPE_VEHICLEASSEMBLY, vehicle_assembly, VehicleAssembly) \
	x(ASSET_TYPE_VEHICLE, vehicle, Vehicle) \
	x(ASSET_TYPE_BITFIELD, bitfield, BitField) \
	x(ASSET_TYPE_TACTICALQUERY, tactical_query, TacticalQuery) \
	x(ASSET_TYPE_RAWSTRING, raw_string, RawString) \
	x(ASSET_TYPE_PLAYERSETTINGS, player_settings, PlayerSettings) \
	x(ASSET_TYPE_MOTIONMATCHINGINPUT, motion_matching_input, MotionMatchingInput) \
	x(ASSET_TYPE_PLAYERSOUNDSTABLE, player_sounds_table, PlayerSoundsTable) \
	x(ASSET_TYPE_PLAYERFXTABLE, player_fx_table, PlayerFxTable) \
	x(ASSET_TYPE_PLAYERROLECATEGORY, player_role_category, PlayerRoleCategory) \
	x(ASSET_TYPE_PLAYEROUTFIT, player_outfit, PlayerOutfit) \
	x(ASSET_TYPE_CHARACTERBODYTYPE, character_body_type, CharacterBodyType) \
	x(ASSET_TYPE_PLAYERMOVEMENTTUNABLES, player_movement_tunables, PlayerMovementTunables) \
	x(ASSET_TYPE_TALENT, talent, Talent) \
	x(ASSET_TYPE_PLAYERTALENTTEMPLATE, player_talent_template, PlayerTalentTemplate) \
	x(ASSET_TYPE_PLAYERROLETEMPLATE, player_role_template, PlayerRoleTemplate) \
	x(ASSET_TYPE_CUSTOMIZATIONTABLE, customization_table, CustomizationTable) \
	x(ASSET_TYPE_CUSTOMIZATIONTABLEFRONTEND, customization_table_frontend, CustomizationTableFrontend) \
	x(ASSET_TYPE_PLAYERROLETEMPLATEFRONTEND, player_role_template_frontend, PlayerRoleTemplateFrontend) \
	x(ASSET_TYPE_CHARACTERBODYTYPEFRONTEND, character_body_type_frontend, CharacterBodyTypeFrontend) \
	x(ASSET_TYPE_CHARMIXER, char_mixer, CharMixer) \
	x(ASSET_TYPE_EXECUTION, execution, Execution) \
	x(ASSET_TYPE_PLAYERANIMATION, player_animation, PlayerAnimation) \
	x(ASSET_TYPE_SPRAYORGESTUREITEM, spray_or_gesture_item, SprayOrGestureItem) \
	x(ASSET_TYPE_SPRAYORGESTURELIST, spray_or_gesture_list, SprayOrGestureList) \
	x(ASSET_TYPE_UIMODELDATASTRUCT, ui_model_data_struct, UIModelDataStruct) \
	x(ASSET_TYPE_STORAGEFILE, storage_file, StorageFile) \
	x(ASSET_TYPE_STORAGEFILELIST, storage_file_list, StorageFileList) \
	x(ASSET_TYPE_STOREPRODUCT, store_product, StoreProduct) \
	x(ASSET_TYPE_STORECATEGORY, store_category, StoreCategory) \
	x(ASSET_TYPE_STORECATEGORYLIST, store_category_list, StoreCategoryList) \
	x(ASSET_TYPE_RANK, rank, Rank) \
	x(ASSET_TYPE_RANKTABLE, rank_table, RankTable) \
	x(ASSET_TYPE_PRESTIGE, prestige, Prestige) \
	x(ASSET_TYPE_PRESTIGETABLE, prestige_table, PrestigeTable) \
	x(ASSET_TYPE_FIRSTPARTYENTITLEMENT, first_party_entitlement, FirstPartyEntitlement) \
	x(ASSET_TYPE_FIRSTPARTYENTITLEMENTLIST, first_party_entitlement_list, FirstPartyEntitlementList) \
	x(ASSET_TYPE_ENTITLEMENT, entitlement, Entitlement) \
	x(ASSET_TYPE_ENTITLEMENTLIST, entitlement_list, EntitlementList) \
	x(ASSET_TYPE_SKU, sku, Sku) \
	x(ASSET_TYPE_LABELSTORE, label_store, LabelStore) \
	x(ASSET_TYPE_LABELSTORELIST, label_store_list, LabelStoreList) \
	x(ASSET_TYPE_MILESTONE, milestone, Milestone) \
	x(ASSET_TYPE_MILESTONETABLE, milestone_table, MilestoneTable) \
	x(ASSET_TYPE_UNLOCKABLEITEM, unlockable_item, UnlockableItem) \
	x(ASSET_TYPE_UNLOCKABLEITEMTABLE, unlockable_item_table, UnlockableItemTable) \
	x(ASSET_TYPE_GAMETYPETABLEENTRY, game_type_table_entry, GameTypeTableEntry) \
	x(ASSET_TYPE_GAMETYPETABLE, game_type_table, GameTypeTable) \
	x(ASSET_TYPE_CRAFTICON, craft_icon, CraftIcon) \
	x(ASSET_TYPE_CRAFTICONLIST, craft_icon_list, CraftIconList) \
	x(ASSET_TYPE_CRAFTWEAPONSTICKER, craft_weapon_sticker, CraftWeaponSticker) \
	x(ASSET_TYPE_CRAFTWEAPONSTICKERLIST, craft_weapon_sticker_list, CraftWeaponStickerList) \
	x(ASSET_TYPE_CRAFTBACKGROUND, craft_background, CraftBackground) \
	x(ASSET_TYPE_CRAFTBACKGROUNDLIST, craft_background_list, CraftBackgroundList) \
	x(ASSET_TYPE_CRAFTMATERIAL, craft_material, CraftMaterial) \
	x(ASSET_TYPE_CRAFTMATERIALLIST, craft_material_list, CraftMaterialList) \
	x(ASSET_TYPE_CRAFTCATEGORY, craft_category, CraftCategory) \
	x(ASSET_TYPE_CRAFTCATEGORYLIST, craft_category_list, CraftCategoryList) \
	x(ASSET_TYPE_CRAFTWEAPONICONTRANSFORM, craft_weapon_icon_transform, CraftWeaponIconTransform) \
	x(ASSET_TYPE_CRAFTWEAPONICONTRANSFORMLIST, craft_weapon_icon_transform_list, CraftWeaponIconTransformList) \
	x(ASSET_TYPE_LUAFILE, raw_text_file, RawTextFile) \
	x(ASSET_TYPE_LEADERBOARDDEF, leaderboard_def, LeaderboardDef) \
	x(ASSET_TYPE_CGMEDIATABLE, cg_media_table, CgMediaTable) \
	x(ASSET_TYPE_BULLETPENETRATION, bullet_penetration, BulletPenetration) \
	x(ASSET_TYPE_LOCDMGTABLE, loc_dmg_table, LocDmgTable) \
	x(ASSET_TYPE_CINEMATIC_CAMERA, cinematic_camera, CinematicCamera) \
	x(ASSET_TYPE_CINEMATIC_SEQUENCE, cinematic_sequence, CinematicSequence) \
	x(ASSET_TYPE_SPECTATE_CAMERA, spectate_camera, SpectateCamera) \
	x(ASSET_TYPE_PLAYERROLECATEGORYTABLE, player_role_category_table, PlayerRoleCategoryTable) \
	x(ASSET_TYPE_HIGHLIGHTREELINFODEFINES, highlight_reel_info_defines, HighlightReelInfoDefines) \
	x(ASSET_TYPE_HIGHLIGHTREELPROFILEWEIGHTING, highlight_reel_value_table, HighlightReelValueTable) \
	x(ASSET_TYPE_HIGHLIGHTREELSTARLEVELS, highlight_reel_value_table, HighlightReelValueTable) \
	x(ASSET_TYPE_DLOGEVENT, dlog_event, DLogEvent) \
	x(ASSET_TYPE_COMPUTESHADERSET, compute_shader_set, ComputeShaderSet) \
	x(ASSET_TYPE_RENDERTARGETS, render_targets, RenderTargets) \
	x(ASSET_TYPE_HWPLATFORM, hw_platform, HwPlatform) \
	x(ASSET_TYPE_RAYTRACESHADERSET, raytrace_shader_set, RaytraceShaderSet)

namespace zonetool::t9
{
	const char* type_to_string(XAssetType type);
	std::int32_t type_to_int(const std::string& type);

	XHash xhash(std::string_view text);
	XHash xhash_from_name(std::string_view name);
	std::string xhash_name(XHash hash);
}

#include "assets/ai.hpp"
#include "assets/blackboard.hpp"
#include "assets/cinematic.hpp"
#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/craft.hpp"
#include "assets/ddl.hpp"
#include "assets/destructible.hpp"
#include "assets/dynmodel.hpp"
#include "assets/effects.hpp"
#include "assets/fx.hpp"
#include "assets/gamemap.hpp"
#include "assets/gfxworld.hpp"
#include "assets/glasses.hpp"
#include "assets/image.hpp"
#include "assets/impacts.hpp"
#include "assets/keyvaluepairs.hpp"
#include "assets/lighting.hpp"
#include "assets/localize.hpp"
#include "assets/material.hpp"
#include "assets/navmesh.hpp"
#include "assets/occlusion.hpp"
#include "assets/physics.hpp"
#include "assets/player.hpp"
#include "assets/postfx.hpp"
#include "assets/rawfile.hpp"
#include "assets/rumble.hpp"
#include "assets/scriptbundle.hpp"
#include "assets/shaderset.hpp"
#include "assets/sound.hpp"
#include "assets/store.hpp"
#include "assets/streamerworld.hpp"
#include "assets/streamkey.hpp"
#include "assets/tables.hpp"
#include "assets/techset.hpp"
#include "assets/terraingfx.hpp"
#include "assets/text.hpp"
#include "assets/triggerlist.hpp"
#include "assets/ui.hpp"
#include "assets/vehicle.hpp"
#include "assets/weapon.hpp"
#include "assets/xanim.hpp"
#include "assets/xcollision.hpp"
#include "assets/xmodel.hpp"
#include "assets/xskeleton.hpp"
