#pragma once

#include <formats/havok_tagfile.hpp>

namespace zonetool::t9::converter::iw8::havok
{
	const std::vector<::havok::tagfile::type_info>& schema();
	const std::vector<::havok::tagfile::type_info>& physics_asset_schema();
	const std::vector<std::uint32_t>& physics_asset_compendium_types();
}
