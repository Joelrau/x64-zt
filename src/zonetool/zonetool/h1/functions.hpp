#pragma once

#include "game/h1/game.hpp"

namespace zonetool::h1
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	extern bool zone_exists(const std::string& zone);

	extern void DB_EnumXAssets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);

	WEAK ::h1::game::symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{ 0x1402BA830 };
	WEAK ::h1::game::symbol<const char* (const XAsset* asset)> DB_GetXAssetName{ 0x14028BE50 };
	WEAK ::h1::game::symbol<void(const XAsset* asset, const char* name) > DB_SetXAssetName{ 0x14028C260 };
	WEAK ::h1::game::symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{ 0x1402BCF90 };
	WEAK ::h1::game::symbol<bool(const char* zone, int source)> DB_FileExists{ 0x1402BA970 };
	WEAK ::h1::game::symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ 0x14028BE70 };
}