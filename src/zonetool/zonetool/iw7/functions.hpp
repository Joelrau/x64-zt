#pragma once

#include "game/iw7/game.hpp"

namespace zonetool::iw7
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	extern bool zone_exists(const std::string& zone);

	extern void DB_EnumXAssets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);

	WEAK::iw7::game::symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{ 0x140A76CE0 };
	WEAK::iw7::game::symbol<const char* (XAssetType type, XAssetHeader header)> DB_GetXAssetHeaderName{ 0x1409E5BA0 };
	WEAK::iw7::game::symbol<void(XAssetType type, XAssetHeader header, const char* name)> DB_SetXAssetHeaderName{ 0x1409E5C40 };
	WEAK::iw7::game::symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, char syncMode)> DB_LoadXAssets{ 0x140A78630 };
	WEAK::iw7::game::symbol<void(int flags)> DB_UnloadFastfilesByZoneFlags{ 0x140A7BCE0 };
	WEAK::iw7::game::symbol<bool(const char* zone, int source)> DB_FileExists{ 0x1403BCF10 };
	WEAK::iw7::game::symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ 0x1409E5BD0 };
}