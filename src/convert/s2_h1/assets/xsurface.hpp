#pragma once

namespace zonetool::s2::converter::h1::xsurface
{
	zonetool::h1::XModelSurfs* convert(const zonetool::s2::XModelSurfs* asset, utils::memory::allocator& allocator);
	void dump(zonetool::s2::XModelSurfs* asset);
	bool is_dumped(const std::string& name);
}
