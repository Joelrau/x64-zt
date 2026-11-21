#pragma once

namespace zonetool::iw7::converter::iw8::xsurface
{
	using vec3 = std::array<float, 3>;

	vec3 unpack_unit_vec(std::uint32_t packed);
	std::uint32_t pack_tangent_frame(const vec3& normal, const vec3& tangent, bool negative_binormal);
	bool is_skinned(const XModel* model);
	std::optional<zonetool::iw8::Bounds> get_skinned_bounds(const XModel* model);
	zonetool::iw8::Bounds get_bounds(const XModel* model, const XSurface& surface);

	void dump(const XModel* model, XModelSurfs* asset);
}
