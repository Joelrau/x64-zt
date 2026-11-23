#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace gfxworld
	{
		struct light_cell_box
		{
			unsigned int cells = 0;
			float lo[3] = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
			float hi[3] = {std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest()};
		};

		struct proxy_mesh
		{
			std::vector<float> vertices;
			std::vector<unsigned short> indices;

			unsigned short add_vertex(const float* point)
			{
				this->vertices.insert(this->vertices.end(), {point[0], point[1], point[2]});
				return static_cast<unsigned short>(this->vertices.size() / 3 - 1);
			}

			void add_triangle(const unsigned short a, const unsigned short b, const unsigned short c)
			{
				this->indices.insert(this->indices.end(), {a, c, b}); // reversed winding, inward normals
			}
		};

		zonetool::iw7::GfxWorld* convert(zonetool::iw5::GfxWorld* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::GfxWorld* asset);
	}
}
