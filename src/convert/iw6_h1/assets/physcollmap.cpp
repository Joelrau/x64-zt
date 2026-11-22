#include <std_include.hpp>
#include "../include.hpp"
#include "physcollmap.hpp"

#include <h1/assets/physcollmap.hpp>

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physcollmap
		{
			namespace
			{
				constexpr auto min_box_half_size = 0.19685;

				using namespace zonetool::h1::polytope;

				zonetool::h1::dmPolytopeData* make_brush(const BrushWrapper& brush, utils::memory::allocator& allocator)
				{
					std::vector<plane_t> sides;
					for (auto i = 0; brush.brush.sides && i < brush.brush.numsides; i++)
					{
						if (const auto* side = brush.brush.sides[i].plane)
						{
							sides.push_back({to_vec3(side->normal), side->dist});
						}
					}

					const auto center = to_vec3(brush.bounds.midPoint);
					const auto half_size = to_vec3(brush.bounds.halfSize);
					return make_polytope(center - half_size, center + half_size, sides, true, allocator);
				}

				zonetool::h1::dmPolytopeData* make_box(const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					const auto center = to_vec3(geom.bounds.midPoint);
					std::vector<plane_t> planes;
					vec3 extent{};
					for (auto i = 0; i < 3; i++)
					{
						const auto axis = normalize(to_vec3(geom.orientation[i]));
						const auto half_size = std::max(static_cast<double>(geom.bounds.halfSize[i]), min_box_half_size);
						planes.push_back({axis, dot(axis, center) + half_size});
						planes.push_back({axis * -1.0, half_size - dot(axis, center)});
						extent = extent + vec3{std::abs(axis.x), std::abs(axis.y), std::abs(axis.z)} * half_size;
					}

					return make_polytope(center - extent, center + extent, planes, false, allocator);
				}

				zonetool::h1::dmPolytopeData* make_cylinder(const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					const auto center = to_vec3(geom.bounds.midPoint);
					const auto axis = normalize(to_vec3(geom.orientation[0]));
					const auto side = normalize(to_vec3(geom.orientation[1]));
					const auto up = normalize(to_vec3(geom.orientation[2]));
					const double radius = geom.bounds.halfSize[0];
					const double half_height = geom.bounds.halfSize[2];
					const auto step = 2.0 * std::numbers::pi / 16;

					std::vector<plane_t> planes;
					for (auto i = 0; i < 16; i++)
					{
						const auto angle = (i + 0.5) * step - std::numbers::pi / 2.0;
						const auto normal = side * std::cos(angle) + up * std::sin(angle);
						planes.push_back({normal, dot(normal, center) + radius * std::cos(step / 2.0)});
					}

					planes.push_back({axis * -1.0, half_height - dot(axis, center)});
					planes.push_back({axis, half_height + dot(axis, center)});

					const auto disc_extent = [&](const double axis_part) { return std::sqrt(std::max(0.0, 1.0 - axis_part * axis_part)) * radius; };
					const vec3 extent{std::abs(axis.x) * half_height + disc_extent(axis.x), std::abs(axis.y) * half_height + disc_extent(axis.y),
						std::abs(axis.z) * half_height + disc_extent(axis.z)};
					return make_polytope(center - extent, center + extent, planes, false, allocator);
				}

				zonetool::h1::dmPolytopeData* convert_geom(const PhysCollmap* asset, const PhysGeomInfo& geom, utils::memory::allocator& allocator)
				{
					auto* polytope = geom.brushWrapper ? make_brush(*geom.brushWrapper, allocator)
						: geom.type == PHYS_GEOM_BOX ? make_box(geom, allocator) : make_cylinder(geom, allocator);
					if (!polytope)
					{
						ZONETOOL_WARNING("physcollmap \"%s\": dropping a geom of type %d that makes no closed hull", asset->name, geom.type);
					}

					return polytope;
				}
			}

			zonetool::h1::PhysCollmap* convert(PhysCollmap* asset, utils::memory::allocator& allocator)
			{
				std::vector<zonetool::h1::dmPolytopeData*> polytopes;
				for (auto i = 0u; asset->geoms && i < asset->count; i++)
				{
					if (auto* polytope = convert_geom(asset, asset->geoms[i], allocator))
					{
						polytopes.emplace_back(polytope);
					}
				}

				if (polytopes.empty())
				{
					ZONETOOL_ERROR("physcollmap \"%s\": no geom could be converted", asset->name);
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

			void dump(PhysCollmap* asset)
			{
				utils::memory::allocator allocator;
				if (auto* converted_asset = convert(asset, allocator))
				{
					zonetool::h1::phys_collmap::dump(converted_asset);
				}
			}
		}
	}
}
