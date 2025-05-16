#pragma once

#include "zonetool/iw7/structs.hpp"
#include "zonetool/iw7/functions.hpp"
#include "zonetool/iw7/variables.hpp"

#include "zonetool/utils/utils.hpp"

#define COPY_VALUE(name) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name)); \
		new_asset->name = asset->name;

#define COPY_VALUE_CAST(name) \
		new_asset->name = *reinterpret_cast<decltype(new_asset->name)*>(&asset->name);

#define COPY_ARR(name) \
		static_assert(sizeof(new_asset->name) == sizeof(asset->name)); \
		std::memcpy(&new_asset->name, &asset->name, sizeof(new_asset->name));

#define REINTERPRET_CAST(name) \
		new_asset->name = reinterpret_cast<decltype(new_asset->name)>(asset->name);

#define REINTERPRET_CAST_SAFE(name) \
		static_assert(sizeof(*new_asset->name) == sizeof(*asset->name)); \
		new_asset->name = reinterpret_cast<decltype(new_asset->name)>(asset->name);

#define REINTERPRET_CAST_SAFE_ARR(name, count) \
		for (auto i_ = 0; i_ < count; i_++) \
		{ \
			REINTERPRET_CAST_SAFE(name[i_]); \
		} \

#define REINTERPRET_CAST_SAFE_TO_FROM(__TO__, __FROM__) \
	static_assert(sizeof(*__FROM__) == sizeof(*__TO__)); \
	__TO__ = reinterpret_cast<decltype(__TO__)>(__FROM__);