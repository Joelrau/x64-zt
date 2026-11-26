#pragma once

namespace zonetool::s4::converter::iw8::mapents
{
	std::vector<zonetool::iw8::DynEntityDef> make_scriptable_dynents(const MapEnts* asset);
	void dump(MapEnts* asset);
}
