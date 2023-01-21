#pragma once

namespace zonetool::s1
{
	WEAK::s1::game::symbol<const char*> g_assetNames{ 0x1409B3180 };

	WEAK::s1::game::symbol<void*> g_assetPool{ 0x1409B40D0 };
	WEAK::s1::game::symbol<int> g_poolSize{ 0x1409B4B90 };

	WEAK::s1::game::symbol<XZoneInfoInternal> g_zoneInfo{ 0x143B739F0 };
	WEAK::s1::game::symbol<unsigned int> g_zoneCount{ 0x14379DBCC };
	WEAK::s1::game::symbol<unsigned short> g_zoneIndex{ 0x14379E2A8 };
}