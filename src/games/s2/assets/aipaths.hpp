#pragma once

namespace zonetool::s2
{
	struct path_data_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class path_data : public field_layout_asset<PathData, ASSET_TYPE_PATHDATA, path_data_layout>
	{
	};
}
