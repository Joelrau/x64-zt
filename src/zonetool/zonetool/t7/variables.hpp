#pragma once

#include "game/t7/game.hpp"

namespace zonetool::t7
{
	WEAK::t7::game::symbol<const char*> g_assetNames{ 0x1410BDC70 };

	WEAK::t7::game::symbol<XZone> g_zones{ 0x146E84AD0 };
	WEAK::t7::game::symbol<XZoneName> g_zoneNames{ 0x146E83270 };
	WEAK::t7::game::symbol<unsigned int> g_zoneCount{ 0x14699D21C };
	WEAK::t7::game::symbol<unsigned short> g_zoneIndex{ 0x1469BB268 };
}