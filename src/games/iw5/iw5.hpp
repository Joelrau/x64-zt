#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

#define IW5_READ_ONLY_ASSETS(x) \
	x(ASSET_TYPE_PHYSPRESET, phys_preset, PhysPreset) \
	x(ASSET_TYPE_PHYSCOLLMAP, phys_collmap, PhysCollmap) \
	x(ASSET_TYPE_XANIMPARTS, xanim_parts, XAnimParts) \
	x(ASSET_TYPE_XMODEL_SURFS, xmodel_surfs, XModelSurfs) \
	x(ASSET_TYPE_XMODEL, xmodel, XModel) \
	x(ASSET_TYPE_MATERIAL, material, Material) \
	x(ASSET_TYPE_PIXELSHADER, pixel_shader, MaterialPixelShader) \
	x(ASSET_TYPE_VERTEXSHADER, vertex_shader, MaterialVertexShader) \
	x(ASSET_TYPE_VERTEXDECL, vertex_decl, MaterialVertexDeclaration) \
	x(ASSET_TYPE_TECHNIQUE_SET, tech_set, MaterialTechniqueSet) \
	x(ASSET_TYPE_IMAGE, gfx_image, GfxImage) \
	x(ASSET_TYPE_SOUND, sound, snd_alias_list_t) \
	x(ASSET_TYPE_SOUND_CURVE, sound_curve, SndCurve) \
	x(ASSET_TYPE_LOADED_SOUND, loaded_sound, LoadedSound) \
	x(ASSET_TYPE_CLIPMAP, clip_map, clipMap_t) \
	x(ASSET_TYPE_COMWORLD, com_world, ComWorld) \
	x(ASSET_TYPE_GLASSWORLD, glass_world, GlassWorld) \
	x(ASSET_TYPE_MAP_ENTS, map_ents, MapEnts) \
	x(ASSET_TYPE_FXWORLD, fx_world, FxWorld) \
	x(ASSET_TYPE_GFXWORLD, gfx_world, GfxWorld) \
	x(ASSET_TYPE_LIGHT_DEF, gfx_light_def, GfxLightDef) \
	x(ASSET_TYPE_FX, fx, FxEffectDef) \
	x(ASSET_TYPE_IMPACT_FX, impact_fx, FxImpactTable) \
	x(ASSET_TYPE_RAWFILE, rawfile, RawFile) \
	x(ASSET_TYPE_SCRIPTFILE, scriptfile, ScriptFile) \
	x(ASSET_TYPE_STRINGTABLE, string_table, StringTable) \
	x(ASSET_TYPE_TRACER, tracer, TracerDef) \
	x(ASSET_TYPE_ATTACHMENT, attachment, WeaponAttachment) \
	x(ASSET_TYPE_WEAPON, weapon, WeaponCompleteDef) \
	x(ASSET_TYPE_VEHICLE, vehicle, VehicleDef) \
	x(ASSET_TYPE_SURFACE_FX, surface_fx, SurfaceFxTable) \
	x(ASSET_TYPE_MENU, menu, menuDef_t) \
	x(ASSET_TYPE_MENULIST, menu_list, MenuList)

#include "assets/rawfile.hpp"
#include "assets/scriptfile.hpp"
#include "assets/stringtable.hpp"
#include "assets/physpreset.hpp"
#include "assets/physcollmap.hpp"
#include "assets/soundcurve.hpp"
#include "assets/gfxlightdef.hpp"
#include "assets/loadedsound.hpp"
#include "assets/mapents.hpp"
#include "assets/gfximage.hpp"
#include "assets/techset.hpp"
#include "assets/material.hpp"
#include "assets/xanimparts.hpp"
#include "assets/xmodelsurfs.hpp"
#include "assets/xmodel.hpp"
#include "assets/sound.hpp"
#include "assets/fx.hpp"
#include "assets/fximpacttable.hpp"
#include "assets/comworld.hpp"
#include "assets/glassworld.hpp"
#include "assets/fxworld.hpp"
#include "assets/clipmap.hpp"
#include "assets/gfxworld.hpp"
#include "assets/tracer.hpp"
#include "assets/attachment.hpp"
#include "assets/weapon.hpp"
#include "assets/vehicle.hpp"
#include "assets/surfacefx.hpp"
#include "assets/menu.hpp"
#include "assets/menulist.hpp"

namespace zonetool::iw5
{
	const char* type_to_string(XAssetType type);
	std::int32_t type_to_int(const std::string& type);
}
