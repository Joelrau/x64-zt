#pragma once

namespace zonetool::h2
{
	WEAK::h2::game::symbol<const char*> g_assetNames{0x140BEF280};

	WEAK::h2::game::symbol<void*> g_assetPool{0x140BF3620};
	WEAK::h2::game::symbol<int> g_poolSize{0x140BF2E40};

	WEAK::h2::game::symbol<XZoneInfoInternal> g_zoneInfo{0x14417A540};
	WEAK::h2::game::symbol<unsigned int> g_zoneCount{0x1422F45F4};
	WEAK::h2::game::symbol<unsigned short> g_zoneIndex{0x1422F8DC8};
}