#pragma once

namespace zonetool::iw8::converter::iw7::xsurface
{
	struct tangent_frame
	{
		std::array<float, 3> normal;
		std::array<float, 3> tangent;
		float binormal_sign;
	};

	tangent_frame unpack_tangent_frame(std::uint32_t packed);
	std::array<float, 3> unpack_unit_vec(std::uint32_t packed);
	std::uint32_t pack_unit_vec(const std::array<float, 3>& value, std::uint32_t alpha);

	zonetool::iw7::XModelSurfs* convert(const XModelSurfs* asset, utils::memory::allocator& allocator);
	void dump(XModelSurfs* asset);
}
