#pragma once

#define COPY_VALUE(__name__) \
	static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
	new_asset->__name__ = asset->__name__;

#define COPY_ARR(__name__) \
	static_assert(sizeof(new_asset->__name__) == sizeof(asset->__name__)); \
	std::memcpy(&new_asset->__name__, &asset->__name__, sizeof(new_asset->__name__));

#define COPY_POINTER(__name__) \
	static_assert(sizeof(*asset->__name__.get()) == sizeof(*new_asset->__name__.get())); \
	new_asset->__name__.value = asset->__name__.value;

#define COPY_POINTER_RAW(__name__) \
	new_asset->__name__.value = asset->__name__.value;
