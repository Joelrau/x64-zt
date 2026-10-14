#pragma once

namespace zonetool::s2::converter::h1::xsurface
{
	zonetool::h1::XModelSurfs* convert(const zonetool::s2::XModelSurfs* asset, const std::unordered_set<int>& vertex_color_surfs,
		utils::memory::allocator& allocator);
	void dump(zonetool::s2::XModelSurfs* asset, const std::unordered_set<int>& vertex_color_surfs = {});
	bool is_dumped(const std::string& name);
}
