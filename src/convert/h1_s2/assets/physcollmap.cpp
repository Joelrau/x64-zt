#include <std_include.hpp>

#include "../include.hpp"
#include "physcollmap.hpp"
#include "physworld.hpp"

namespace zonetool::h1::converter::s2::physcollmap
{
	namespace
	{
		std::unordered_set<std::string> dumped_collmaps;

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
				geom.type = zonetool::s2::PHYS_GEOM_POLYTOPE;
				geom.partIndex = -1;
				geom.polytope = allocator.allocate<zonetool::s2::dmPolytopeData>();
				*geom.polytope = physworld::convert_polytope(*polytope);
			}

			std::memcpy(new_collmap->bounds.midPoint, collmap->bounds.midPoint, sizeof(new_collmap->bounds.midPoint));
			std::memcpy(new_collmap->bounds.halfSize, collmap->bounds.halfSize, sizeof(new_collmap->bounds.halfSize));

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
