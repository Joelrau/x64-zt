#pragma once

#define WEAK __declspec(selectany)

namespace zonetool::iw6
{
	WEAK::iw6::game::symbol<const char*> g_assetNames{ 0x1409E40C0 };

	WEAK::iw6::game::symbol<void*> g_assetPool{ 0x1409E4F20 };
	WEAK::iw6::game::symbol<int> g_poolSize{ 0x1409E4E20 };

	WEAK::iw6::game::symbol<XZoneInfoInternal> g_zoneInfo{ 0x143A50B50 };
	WEAK::iw6::game::symbol<unsigned int> g_zoneCount{ 0x143806498 };
	WEAK::iw6::game::symbol<unsigned short> g_zoneIndex{ 0x14380649C };
}