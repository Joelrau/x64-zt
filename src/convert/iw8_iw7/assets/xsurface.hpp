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
	std::uint32_t pack_unit_vec(const std::array<float, 3>& value, std::uint32_t alpha);

	void dump(XModelSurfs* asset);
}
