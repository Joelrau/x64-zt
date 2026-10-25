#include <std_include.hpp>

#include "../include.hpp"
#include "physcollmap.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physcollmap
{
	namespace
	{
		constexpr auto phys_geom_polytope = 1;

		std::unordered_set<std::string> dumped_collmaps;

		// h1 geoms are all polytopes, s2 tags each geom with its type and part (none here) and keeps the bounds after 12 unused bytes
		void dump_collmap(const PhysCollmap* collmap)
		{
			utils::memory::allocator allocator;
			const auto new_collmap = allocator.allocate<zonetool::s2::PhysCollmap>();
			new_collmap->name = collmap->name;
			new_collmap->geoms = allocator.allocate_array<zonetool::s2::PhysGeomInfo>(collmap->count);
			for (auto i = 0u; i < collmap->count; i++)
			{
				const auto* polytope = collmap->geoms[i].data;
				if (!polytope)
				{
					continue;
				}

				auto& geom = new_collmap->geoms[new_collmap->geomCount++];
				geom.type = phys_geom_polytope;
				geom.partIndex = -1;
				geom.polytope = allocator.allocate<zonetool::s2::dmPolytopeData>();
				*geom.polytope = physworld::convert_polytope(*polytope);
			}

			std::memcpy(new_collmap->boundsMidPoint, collmap->bounds.midPoint, sizeof(new_collmap->boundsMidPoint));
			std::memcpy(new_collmap->boundsHalfSize, collmap->bounds.halfSize, sizeof(new_collmap->boundsHalfSize));

			zonetool::s2::phys_collmap::dump(new_collmap);
			add_csv_row("phys_collmap", collmap->name);
		}
	}

	zonetool::s2::PhysCollmap* reference(const PhysCollmap* collmap, utils::memory::allocator& allocator)
	{
		if (!collmap || !collmap->name || !collmap->geoms || !collmap->count)
		{
			return nullptr;
		}

		if (dumped_collmaps.insert(collmap->name).second)
		{
			dump_collmap(collmap);
		}

		return make_stub<zonetool::s2::PhysCollmap>(collmap->name, allocator);
	}

	void clear()
	{
		dumped_collmaps.clear();
	}
}
