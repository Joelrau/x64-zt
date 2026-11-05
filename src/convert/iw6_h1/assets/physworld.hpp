#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physworld
		{
			struct triangle_t
			{
				int verts[3];
				int material;
			};

			struct position_hash
			{
				std::size_t operator()(const std::array<int, 3>& position) const
				{
					return (static_cast<std::size_t>(position[0]) * 73856093) ^ (static_cast<std::size_t>(position[1]) * 19349663)
						^ (static_cast<std::size_t>(position[2]) * 83492791);
				}
			};

			zonetool::h1::PhysWorld* generate(zonetool::h1::clipMap_t* asset, const MapEnts* map_ents, utils::memory::allocator& allocator);
		}
	}
}