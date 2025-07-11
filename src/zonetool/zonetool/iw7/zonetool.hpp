#pragma once

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/iw7/game.hpp"
#include "component/iw7/command.hpp"
#include "component/iw7/scheduler.hpp"

#include "structs.hpp"
#include "functions.hpp"
#include "variables.hpp"

#include "zonetool/utils/utils.hpp"
#include "zonetool/shared/shared.hpp"

#include "assets/ddl.hpp"
#include "assets/fxeffectdef.hpp"
#include "assets/fxparticlesimanimation.hpp"
#include "assets/gesture.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxlightdef.hpp"
#include "assets/gfxlightmap.hpp"
#include "assets/laser.hpp"
#include "assets/localize.hpp"
#include "assets/luafile.hpp"
#include "assets/material.hpp"
#include "assets/netconststrings.hpp"
#include "assets/particle_system.hpp"
#include "assets/rawfile.hpp"
#include "assets/reticle.hpp"
#include "assets/rumble.hpp"
#include "assets/rumble_graph.hpp"
#include "assets/scriptabledef.hpp"
#include "assets/scriptfile.hpp"
#include "assets/streaming_info.hpp"
#include "assets/stringtable.hpp"
#include "assets/tracer.hpp"
#include "assets/ttfdef.hpp"
#include "assets/vectorfield.hpp"
#include "assets/weapon_anim_package.hpp"
#include "assets/weapon_sfx_package.hpp"
#include "assets/weapon_vfx_package.hpp"
#include "assets/weaponattachment.hpp"
#include "assets/weapondef.hpp"
#include "assets/xanim.hpp"
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

#include "assets/clipmap.hpp"
#include "assets/comworld.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfxworld.hpp"
#include "assets/gfxworld_tr.hpp"
#include "assets/glassworld.hpp"
#include "assets/mapents.hpp"

#include "zone.hpp"

namespace zonetool::iw7
{
	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

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

	void initialize();
	void start();
}