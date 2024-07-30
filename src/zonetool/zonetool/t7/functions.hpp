#pragma once

#include "game/t7/game.hpp"

namespace zonetool::t7
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	WEAK::t7::game::symbol<const char* (const XAsset* asset)> DB_GetXAssetName{ 0x14019F080 };
	WEAK::t7::game::symbol<bool(const char* zone, char unk)> DB_FileExists{ 0x1401D5C20 };
	WEAK::t7::game::symbol<XAssetHeader(XAssetType type, const char* name, bool errorIfMissing, int waitTime)> DB_FindXAssetHeader{ 0x1401D5FB0 };

	WEAK::t7::game::symbol<void(XZoneInfo* zoneInfo, uint zoneCount, qboolean sync, qboolean suppressSync)> DB_LoadXAssets{ 0x1401D8740 };
	WEAK::t7::game::symbol<void(unsigned int zoneIndex, bool createDefault, qboolean suppressSync)> DB_UnloadXZone{ 0x1401DAC50 };
}