#pragma once

namespace zonetool::h1::converter::s2::scriptable
{
	zonetool::s2::ScriptableMapEnts convert_map_ents(const ScriptableMapEnts& map_ents, utils::memory::allocator& allocator);
	void dump(ScriptableDef* asset);
	void finish();
}
