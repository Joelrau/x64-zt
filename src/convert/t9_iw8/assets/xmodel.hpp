#pragma once

namespace zonetool::t9::converter::iw8::xmodel
{
	struct static_model_surface
	{
		zonetool::iw8::Bounds bounds;
		std::string material;
	};

	struct static_model_lod
	{
		float dist;
		std::vector<static_model_surface> surfaces;
	};

	struct static_model_info
	{
		std::string name;
		zonetool::iw8::Bounds bounds;
		std::vector<static_model_lod> lods;
		std::vector<std::array<float, 3>> positions;
		std::vector<std::uint32_t> indices;
	};

	std::array<float, 3> unpack_unit_vec(std::uint32_t packed);
	std::uint32_t pack_tangent_frame(const std::array<float, 3>& normal, const std::array<float, 3>& tangent, bool negative_binormal);

	void dump(XModel* asset);
	const static_model_info* dump_static(XModel* asset);
}
