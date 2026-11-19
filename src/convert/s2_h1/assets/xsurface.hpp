#pragma once

namespace zonetool::s2::converter::h1::xsurface
{
	struct vertex_streams
	{
		const std::uint16_t* indices;
		const float* positions;
		const std::uint32_t* colors;
		const std::uint32_t* tex_coords;
		const std::uint32_t* normals;
		const std::uint32_t* tangents;
		const XSurfaceSkinVertex* skin;
	};

	zonetool::h1::XModelSurfs* convert(const zonetool::s2::XModelSurfs* asset, const std::unordered_map<int, std::uint32_t>& vertex_color_surfs,
		unsigned int vertex_lighting_index, utils::memory::allocator& allocator);
	void dump(zonetool::s2::XModelSurfs* asset, const std::unordered_map<int, std::uint32_t>& vertex_color_surfs = {}, unsigned int vertex_lighting_index = 0);
	bool is_dumped(const std::string& name);
}
