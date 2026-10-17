#pragma once

namespace zonetool::s2
{
	struct scriptable_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class scriptable_def : public field_layout_asset<ScriptableDef, ASSET_TYPE_SCRIPTABLE, scriptable_layout>
	{
	};
}
