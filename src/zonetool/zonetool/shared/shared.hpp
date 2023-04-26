#pragma once

#include "../utils/utils.hpp"
#include "../shared/interfaces/zonebuffer.hpp"
#include "../shared/interfaces/zone.hpp"
#include "../shared/interfaces/asset.hpp"

#define ASSET_TEMPLATE typename S, std::int32_t Type, typename Types, typename H, typename E

#define REGISTER_TEMPLATED_ASSET_CLASS(__name__, __class__, __struct__, __type__, ...) \
	using __name__ = zonetool::__class__<__struct__, __type__, XAssetType, XAssetHeader, XAssetEntry, __VA_ARGS__> \

#define REGISTER_TEMPLATED_ASSET(__name__, __struct__, __type__, ...) \
	using __name__ = zonetool::__name__<__struct__, __type__, XAssetType, XAssetHeader, XAssetEntry, __VA_ARGS__> \

#define TYPEOF_MEMBER(__struct__, __member__) \
	std::remove_pointer<typename std::remove_all_extents<decltype(std::declval<__struct__>().__member__)>::type>::type \

#include "../shared/assets/rawfile.hpp"
#include "../shared/assets/shader.hpp"
#include "../shared/assets/vertexdecl.hpp"
#include "../shared/assets/laserdef.hpp"
#include "../shared/assets/clipmap.hpp"
