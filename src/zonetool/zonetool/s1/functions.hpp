#pragma once

#include "game/s1/game.hpp"

namespace zonetool::s1
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	extern bool zone_exists(const std::string& zone);

	extern XAssetHeader DB_FindXAssetHeader_Safe(XAssetType type, const std::string& name);
	extern void DB_EnumXAssets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);

	WEAK::s1::game::symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{ 0x14026EC80 };
	WEAK::s1::game::symbol<const char* (const XAsset* asset)> DB_GetXAssetName{ 0x140240DD0 };
	WEAK::s1::game::symbol<void(const XAsset* asset, const char* name) > DB_SetXAssetName{ 0x140240E00 };
	WEAK::s1::game::symbol<XAssetHeader(XAssetType type, const char* name, int createDefault)> DB_FindXAssetHeader{ 0x14026F0F0 };
	WEAK::s1::game::symbol<XAssetEntry* (XAssetType type, const char* name)> DB_FindXAssetEntry{ 0x14026F020 };
	WEAK::s1::game::symbol<bool(XAssetType, const char* name)> DB_IsXAssetDefault{ 0x140270320 };
	WEAK::s1::game::symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{ 0x140270F30 };
	WEAK::s1::game::symbol<bool(const char* zone, int source)> DB_FileExists{ 0x14026EDC0 };
	WEAK::s1::game::symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ 0x140240DF0 };
}