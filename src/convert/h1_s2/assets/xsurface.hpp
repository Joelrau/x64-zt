#pragma once

namespace zonetool::h1::converter::s2::xsurface
{
	void dump(zonetool::h1::XModelSurfs* asset);
	void assign_base_vert_indices(const zonetool::h1::XModel* model);
	void finish();
	float surface_area(const zonetool::h1::XModelSurfs* asset);
	std::uint32_t triangle_count(const zonetool::h1::XModelSurfs* asset);
}
