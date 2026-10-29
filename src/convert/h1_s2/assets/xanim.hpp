#pragma once

namespace zonetool::h1::converter::s2::xanim
{
	void dump(XAnimParts* asset);
	bool dump_used(const std::string& name);
	zonetool::s2::XAnimParts* reference(const XAnimParts* asset, utils::memory::allocator& allocator);
	void clear();
}
