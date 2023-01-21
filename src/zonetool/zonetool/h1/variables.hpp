#pragma once

namespace zonetool::h1
{
	WEAK::h1::game::symbol<const char*> g_assetNames{ 0x140FEA240 };

	WEAK::h1::game::symbol<void*> g_assetPool{ 0x140FEB5D0 };
	WEAK::h1::game::symbol<int> g_poolSize{ 0x140FEADF0 };

	WEAK::h1::game::symbol<XZoneInfoInternal> g_zoneInfo{ 0x145122460 };
	WEAK::h1::game::symbol<unsigned int> g_zoneCount{ 0x143498F0C };
	WEAK::h1::game::symbol<unsigned short> g_zoneIndex{ 0x1434A9B68 };
}