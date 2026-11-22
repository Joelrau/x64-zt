#include <std_include.hpp>

#include "../include.hpp"
#include "physcollmap.hpp"
#include "physworld.hpp"

#include <h1/assets/physcollmap.hpp>

namespace zonetool::s2::converter::h1::physcollmap
{
	namespace
	{
		using namespace zonetool::h1::polytope;

		std::unordered_map<std::string, bool> converted_collmaps;

		std::vector<vec3> get_hull_directions()
		{
			std::vector<vec3> directions;
			for (auto x = -1; x <= 1; x++)
			{
				for (auto y = -1; y <= 1; y++)
				{
					for (auto z = -1; z <= 1; z++)
					{
						if (x || y || z)
						{
							directions.emplace_back(normalize({static_cast<double>(x), static_cast<double>(y), static_cast<double>(z)}));
						}
					}
				}
			}

			return directions;
		}

		zonetool::h1::dmPolytopeData* make_capsule(const vec3& start, const vec3& end, const double radius, utils::memory::allocator& allocator)
		{
			static const auto directions = get_hull_directions();
			std::vector<plane_t> planes;
			for (const auto& normal : directions)
			{
				planes.push_back({normal, std::max(dot(normal, start), dot(normal, end)) + radius});
			}

			const vec3 mins{std::min(start.x, end.x) - radius, std::min(start.y, end.y) - radius, std::min(start.z, end.z) - radius};
			const vec3 maxs{std::max(start.x, end.x) + radius, std::max(start.y, end.y) + radius, std::max(start.z, end.z) + radius};
			return make_polytope(mins, maxs, planes, true, allocator);
		}

		zonetool::h1::dmPolytopeData* convert_geom(const PhysCollmap* asset, const PhysGeomInfo& geom, utils::memory::allocator& allocator)
		{
			zonetool::h1::dmPolytopeData* polytope = nullptr;
			if (geom.type == PHYS_GEOM_POLYTOPE && geom.polytope)
			{
				polytope = allocator.allocate<zonetool::h1::dmPolytopeData>();
				*polytope = physworld::convert_polytope(*geom.polytope, allocator);
			}
			else if (geom.type == PHYS_GEOM_SPHERE && geom.sphere)
			{
				const auto center = to_vec3(geom.sphere->center);
				polytope = make_capsule(center, center, geom.sphere->radius, allocator);
			}
			else if (geom.type == PHYS_GEOM_CAPSULE && geom.capsule)
			{
				polytope = make_capsule(to_vec3(geom.capsule->start), to_vec3(geom.capsule->end), geom.capsule->radius, allocator);
			}

			if (!polytope)
			{
				ZONETOOL_WARNING("phys_collmap \"%s\": dropping a geom of type %d", asset->name, geom.type);
			}

			return polytope;
		}

		zonetool::h1::PhysCollmap* convert(const PhysCollmap* asset, utils::memory::allocator& allocator)
		{
			std::vector<zonetool::h1::dmPolytopeData*> polytopes;
			for (auto i = 0; i < asset->geomCount; i++)
			{
				if (auto* polytope = convert_geom(asset, asset->geoms[i], allocator))
				{
					polytopes.emplace_back(polytope);
				}
			}

			if (polytopes.empty())
			{
				return nullptr;
			}

			auto* new_asset = allocator.allocate<zonetool::h1::PhysCollmap>();
			new_asset->name = asset->name;
			new_asset->count = static_cast<unsigned int>(polytopes.size());
			new_asset->geoms = allocator.allocate_array<zonetool::h1::PhysGeomInfo>(polytopes.size());
			for (auto i = 0u; i < new_asset->count; i++)
			{
				new_asset->geoms[i].data = polytopes[i];
			}

			set_collmap_mass(new_asset);
			return new_asset;
		}
	}

	const char* reference(const PhysCollmap* asset)
	{
		if (!asset || !asset->name || !asset->geoms || asset->geomCount <= 0)
		{
			return nullptr;
		}

		if (const auto entry = converted_collmaps.find(asset->name); entry != converted_collmaps.end())
		{
			return entry->second ? asset->name : nullptr;
		}

		utils::memory::allocator allocator;
		auto* converted = convert(asset, allocator);
		if (converted)
		{
			zonetool::h1::phys_collmap::dump(converted);
			add_csv_row("phys_collmap", asset->name);
		}
		else
		{
			ZONETOOL_WARNING("phys_collmap \"%s\": no geom could be converted", asset->name);
		}

		converted_collmaps.emplace(asset->name, converted != nullptr);
		return converted ? asset->name : nullptr;
	}

	float get_volume(const PhysCollmap* asset)
	{
		auto volume = 0.0f;
		for (auto i = 0; asset && asset->geoms && i < asset->geomCount; i++)
		{
			const auto& geom = asset->geoms[i];
			if (geom.type == PHYS_GEOM_POLYTOPE && geom.polytope)
			{
				volume += geom.polytope->volume;
			}
			else if (geom.type == PHYS_GEOM_SPHERE && geom.sphere)
			{
				volume += 4.0f / 3.0f * std::numbers::pi_v<float> * std::pow(geom.sphere->radius, 3.0f);
			}
			else if (geom.type == PHYS_GEOM_CAPSULE && geom.capsule)
			{
				const auto& capsule = *geom.capsule;
				const auto length = std::hypot(capsule.end[0] - capsule.start[0], capsule.end[1] - capsule.start[1], capsule.end[2] - capsule.start[2]);
				volume += std::numbers::pi_v<float> * capsule.radius * capsule.radius * (length + 4.0f / 3.0f * capsule.radius);
			}
		}

		return volume;
	}
}
