#pragma once

#include <zonetool.hpp>

#include "structs.hpp"

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