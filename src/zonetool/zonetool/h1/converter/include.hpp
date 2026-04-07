#pragma once

#include "zonetool/h1/structs.hpp"

#include "zonetool/utils/utils.hpp"

#define TECHSET_PREFIX "_h1"s

#define COPY_VALUE(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		new_asset->__name__ = asset->__name__;

#define COPY_VALUE_CAST(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		new_asset->__name__ = *reinterpret_cast<decltype(new_asset->__name__)*>(&asset->__name__);

#define COPY_ARR(__name__) \
		static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
		std::memcpy(&new_asset->__name__, &asset->__name__, sizeof(new_asset->__name__));

#define COPY_ASSET(__name__) \
		if(asset->__name__) \
		{ \
			new_asset->__name__ = allocator.allocate<typename std::remove_reference<decltype(*new_asset->__name__)>::type>(); \
			new_asset->__name__->name = asset->__name__->name; \
		}

#define COPY_ASSET_ARR(__name__, __count__) \
		if(asset->__name__) \
		{ \
			new_asset->__name__ = allocator.allocate_array<typename std::remove_reference<decltype(*new_asset->__name__)>::type>(__count__); \
			for (auto i = 0; i < __count__; i++) \
			{ \
				if (asset->__name__[i] != nullptr) \
				{ \
					new_asset->__name__[i] = allocator.allocate<typename std::remove_reference<decltype(**new_asset->__name__)>::type>(); \
					new_asset->__name__[i]->name = asset->__name__[i]->name; \
				} \
			} \
		} \


#define REINTERPRET_CAST_SAFE(__name__) \
		static_assert(sizeof(*new_asset->__name__) == sizeof(*asset->__name__)); \
		new_asset->__name__ = reinterpret_cast<decltype(new_asset->__name__)>(asset->__name__);

#define REINTERPRET_CAST_SAFE_ARR(__name__, count) \
		for (auto i_ = 0; i_ < count; i_++) \
		{ \
			REINTERPRET_CAST_SAFE(__name__[i_]); \
		} \

#define REINTERPRET_CAST_SAFE_TO_FROM(__TO__, __FROM__) \
	static_assert(sizeof(*__FROM__) == sizeof(*__TO__)); \
	__TO__ = reinterpret_cast<decltype(__TO__)>(__FROM__);