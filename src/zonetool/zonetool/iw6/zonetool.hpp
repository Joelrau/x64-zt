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

#include "assets/localize.hpp"
#include "assets/luafile.hpp"
#include "assets/rawfile.hpp"
#include "assets/netconststrings.hpp"
#include "assets/rawfile.hpp"
#include "assets/scriptfile.hpp"
#include "assets/stringtable.hpp"

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