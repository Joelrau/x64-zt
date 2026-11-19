#pragma once

namespace zonetool::iw7::converter::iw8::xsurface
{
	using vec3 = std::array<float, 3>;

	vec3 unpack_unit_vec(std::uint32_t packed);
	std::uint32_t pack_tangent_frame(const vec3& normal, const vec3& tangent, bool negative_binormal);

	void dump(XModelSurfs* asset);
}
