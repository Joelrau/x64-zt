#pragma once

#include "../zonetool.hpp"
#include "../zone/zone_buffer.hpp"
#include "../zone/zone_reader.hpp"
#include "../zone/zone.hpp"
#include "../zone/asset.hpp"

#define ASSET_TEMPLATE typename S, std::int32_t Type, typename Types, typename H, typename E, typename Streams

#define REGISTER_TEMPLATED_ASSET_CLASS(__name__, __class__, __struct__, __type__, ...) \
	using __name__ = zonetool::__class__<__struct__, __type__, XAssetType, XAssetHeader, XAssetEntry, XFileBlock, __VA_ARGS__> \

#define REGISTER_TEMPLATED_ASSET(__name__, __struct__, __type__, ...) \
	using __name__ = zonetool::__name__<__struct__, __type__, XAssetType, XAssetHeader, XAssetEntry, XFileBlock, __VA_ARGS__> \

#define TYPEOF_MEMBER(__struct__, __member__) \
	std::remove_pointer<typename std::remove_all_extents<decltype(std::declval<__struct__>().__member__)>::type>::type \

#include "rawfile.hpp"
#include "luafile.hpp"
#include "scriptfile.hpp"
#include "stringtable.hpp"
#include "localize.hpp"
#include "ttfdef.hpp"

#include "shader.hpp"
#include "vertexdecl.hpp"
#include "laserdef.hpp"
#include "clipmap.hpp"
