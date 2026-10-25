#pragma once

namespace zonetool::h1::converter::s2::xanim
{
	void dump(XAnimParts* asset);
	bool dump_used(const std::string& name);
	void clear();
}
