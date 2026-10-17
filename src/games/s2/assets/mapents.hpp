#pragma once

namespace zonetool::s2
{
	struct map_ents_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class map_ents : public field_layout_asset<MapEnts, ASSET_TYPE_MAP_ENTS, map_ents_layout>
	{
	};

	struct addon_map_ents_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class addon_map_ents : public field_layout_asset<AddonMapEnts, ASSET_TYPE_ADDON_MAP_ENTS, addon_map_ents_layout>
	{
	};
}
