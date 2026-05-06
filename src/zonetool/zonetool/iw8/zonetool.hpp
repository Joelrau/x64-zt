#pragma once

#include <utils/hook.hpp>
#include <utils/string.hpp>
#include "game/iw8/game.hpp"
#include "component/iw8/command.hpp"
#include "component/iw8/scheduler.hpp"

#include "structs.hpp"
#include "functions.hpp"
#include "variables.hpp"

#include "zonetool/utils/utils.hpp"
#include "zonetool/shared/shared.hpp"

// assets
#include "assets/aipaths.hpp"
#include "assets/rawfile.hpp"
#include "assets/scriptabledef.hpp"
#include "assets/scriptfile.hpp"
#include "assets/xanim.hpp"
#include "assets/xmodel.hpp"
#include "assets/xsurface.hpp"

#include "zone.hpp"

namespace zonetool::iw8
{
	extern std::unordered_set<std::pair<std::uint32_t, std::string>, pair_hash<std::uint32_t, std::string>> ignore_assets;

	XAssetHeader db_find_x_asset_header(XAssetType type, const char* name, int create_default);
	XAssetHeader db_find_x_asset_header_safe(XAssetType type, const std::string& name);

	template <typename T>
	XAssetHeader db_find_x_asset_header_copy(XAssetType type, const std::string& name, zone_memory* mem)
	{
		auto header = db_find_x_asset_header_safe(type, name);
		if (header.data)
		{
			T* newData = mem->allocate<T>();
			memcpy(newData, header.data, sizeof(T));
			header.data = reinterpret_cast<void*>(newData);
		}
		return header;
	}

	void initialize();
	void start();
}