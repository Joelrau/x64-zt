#pragma once

#include "game/iw7/game.hpp"

namespace zonetool::iw7
{
	WEAK::iw7::game::symbol<const char*> g_assetNames{ 0x141BAAC20 };

	WEAK::iw7::game::symbol<void*> g_assetPool{ 0x1414663D0 };
	WEAK::iw7::game::symbol<int> g_poolSize{ 0x141466290 };

	WEAK::iw7::game::symbol<unsigned short> g_zoneIndex{ 0x143A7A7D4 };

	WEAK::iw7::game::symbol<DB_LoadData> g_load{ 0x1452A8010 };

	WEAK::iw7::game::symbol<XZoneMemory*> g_streamZoneMem{ 0x145687E90 };
}