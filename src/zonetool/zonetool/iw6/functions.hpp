#pragma once

#include "game/iw6/game.hpp"

namespace zonetool::iw6
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	extern bool zone_exists(const std::string& zone);

	extern XAssetHeader db_find_x_asset_header_safe(XAssetType type, const std::string& name);
	extern void DB_EnumXAssets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);

	WEAK::iw6::game::symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{ 0x14031F000 };
	WEAK::iw6::game::symbol<const char* (const XAsset* asset)> DB_GetXAssetName{ 0x1402FB160 };
	WEAK::iw6::game::symbol<void(const XAsset* asset, const char* name) > DB_SetXAssetName{ 0x1402FB190 };
	WEAK::iw6::game::symbol<XAssetHeader(XAssetType type, const char* name, int createDefault)> DB_FindXAssetHeader{ 0x14031F3A0 };
	WEAK::iw6::game::symbol<XAssetEntry* (XAssetType type, const char* name)> DB_FindXAssetEntry{ 0x14031F2D0 };
	WEAK::iw6::game::symbol<bool(XAssetType, const char* name)> DB_IsXAssetDefault{ 0x1403204D0 };
	WEAK::iw6::game::symbol<void(XZoneInfo* zoneInfo, unsigned int zoneCount, DBSyncMode syncMode)> DB_LoadXAssets{ 0x140320F20 };
	WEAK::iw6::game::symbol<bool(const char* zone, int source)> DB_FileExists{ 0x14031F0E0 };
	WEAK::iw6::game::symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ 0x1402FB180 };

	WEAK::iw6::game::symbol<scr_string_t(const char* s, int sys)> SL_AllocString{ 0x140432060 };
}