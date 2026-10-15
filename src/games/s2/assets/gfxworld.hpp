#pragma once

namespace zonetool::s2
{
	struct gfx_world_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class gfx_world : public field_layout_asset<GfxWorld, ASSET_TYPE_GFXWORLD, gfx_world_layout>
	{
	};
}
