#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			struct vec3
			{
				double x;
				double y;
				double z;
			};

			struct plane_t
			{
				vec3 normal;
				double offset;
			};

			struct polygon_t
			{
				std::vector<vec3> points;
				plane_t plane;
			};

			struct face_t
			{
				std::vector<int> verts;
				plane_t plane;
			};

			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator);
			void dump(PhysCollmap* asset);
		}
	}
}