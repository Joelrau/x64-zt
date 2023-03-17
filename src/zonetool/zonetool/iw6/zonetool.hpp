#pragma once

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/iw6/game.hpp"
#include "component/iw6/command.hpp"
#include "component/iw6/scheduler.hpp"

#include "structs.hpp"
#include "functions.hpp"
#include "variables.hpp"

#include "zonetool/utils/utils.hpp"

#include "zonebuffer.hpp"

#include "interfaces/zone.hpp"
#include "interfaces/asset.hpp"

#include "assets/clipmap.hpp"
#include "assets/computeshader.hpp"
#include "assets/comworld.hpp"
#include "assets/domainshader.hpp"
#include "assets/dopplerpreset.hpp"
#include "assets/fontdef.hpp"
#include "assets/fxeffectdef.hpp"
#include "assets/fxparticlesimanimation.hpp"
#include "assets/fxworld.hpp"
#include "assets/gfximage.hpp"
#include "assets/gfxlightdef.hpp"
#include "assets/gfxworld.hpp"
#include "assets/glassworld.hpp"
#include "assets/hullshader.hpp"
#include "assets/loadedsound.hpp"
#include "assets/localize.hpp"
#include "assets/lpfcurve.hpp"
#include "assets/luafile.hpp"
#include "assets/mapents.hpp"
#include "assets/material.hpp"
#include "assets/netconststrings.hpp"
#include "assets/physcollmap.hpp"
#include "assets/physpreset.hpp"
#include "assets/pixelshader.hpp"
#include "assets/rawfile.hpp"
#include "assets/reverbcurve.hpp"
#include "assets/scriptabledef.hpp"
#include "assets/scriptfile.hpp"
#include "assets/sound.hpp"
#include "assets/soundcurve.hpp"
#include "assets/stringtable.hpp"
#include "assets/structureddatadefset.hpp"
#include "assets/techset.hpp"
#include "assets/tracerdef.hpp"
#include "assets/vertexdecl.hpp"
#include "assets/vertexshader.hpp"
#include "assets/weaponattachment.hpp"
#include "assets/weapondef.hpp"
#include "assets/xanim.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

#include "zone.hpp"

namespace zonetool::iw6
{
	template <typename T>
	XAssetHeader DB_FindXAssetHeader_Copy(XAssetType type, const std::string& name, ZoneMemory* mem)
	{
		auto header = DB_FindXAssetHeader_Safe(type, name);
		if (header.data)
		{
			T* newData = mem->Alloc<T>();
			memcpy(newData, header.data, sizeof(T));
			header.data = reinterpret_cast<void*>(newData);
		}
		return header;
	}

	void initialize();
	void start();
}