#pragma once

#include "game/iw8/game.hpp"

namespace zonetool::iw8
{
	extern const char* get_asset_name(XAssetType type, void* pointer);
	extern const char* get_asset_name(XAsset* asset);
	extern const char* type_to_string(XAssetType type);
	extern std::int32_t type_to_int(std::string type);

	extern bool zone_exists(const std::string& zone);

	extern void DB_EnumXAssets(const XAssetType type, const std::function<void(XAssetHeader)>& callback, const bool includeOverride);
	extern bool Material_TechSetHasTechnique(const MaterialTechniqueSet* techSet, MaterialTechniqueType techType);

	WEAK::iw8::game::symbol<void(XAssetType type, void(__cdecl* func)(XAssetHeader, void*), const void* inData, bool includeOverride)> DB_EnumXAssets_Internal{ 0x0A76CE0 };
	WEAK::iw8::game::symbol<const char* (XAssetType type, XAssetHeader header)> DB_GetXAssetHeaderName{ 0x09E5BA0 };
	WEAK::iw8::game::symbol<void(XAssetType type, XAssetHeader header, const char* name)> DB_SetXAssetHeaderName{ 0x09E5C40 };
	WEAK::iw8::game::symbol<void(DB_FastfileInfo* zoneInfo, unsigned int zoneCount, char syncMode, bool loadServerZone)> DB_LoadFastfiles{ 0x11ACD30 };
	WEAK::iw8::game::symbol<void(int flags)> DB_UnloadFastfilesByZoneFlags{ 0x0A7BCE0 };
	WEAK::iw8::game::symbol<bool(const char* zone, int source)> DB_FileExists{ 0x03BCF10 };
	WEAK::iw8::game::symbol<size_t(XAssetType type)> DB_GetXAssetTypeSize{ 0x09E5BD0 };
	WEAK::iw8::game::symbol<int(const char* name)> DB_Zones_GetZoneIndexFromName{ 0x03BC3A0 };
}