#pragma once

namespace zonetool::s2::cast_xanim
{
	XAnimParts* parse(const std::string& name, zone_memory* mem);
	void dump(XAnimParts* parts);
}
