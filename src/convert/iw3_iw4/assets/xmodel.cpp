#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "material.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace xmodel
	{
		namespace
		{
			void convert_surface(const zonetool::iw3::XSurface* asset, zonetool::iw4::XSurface* new_asset)
			{
				static_assert(sizeof(zonetool::iw3::GfxPackedVertex) == sizeof(zonetool::iw4::GfxPackedVertex));
				static_assert(sizeof(zonetool::iw3::XRigidVertList) == sizeof(zonetool::iw4::XRigidVertList));
				static_assert(sizeof(zonetool::iw3::XSurfaceTri16) == sizeof(zonetool::iw4::Face));
				static_assert(sizeof(zonetool::iw3::XSurfaceVertexInfo) == sizeof(zonetool::iw4::XSurfaceVertexInfo));

				new_asset->tileMode = asset->tileMode;
				new_asset->deformed = asset->deformed;
				new_asset->vertCount = asset->vertCount;
				new_asset->triCount = asset->triCount;
				new_asset->zoneHandle = 0;
				new_asset->baseTriIndex = asset->baseTriIndex;
				new_asset->baseVertIndex = asset->baseVertIndex;
				new_asset->triIndices.value = asset->triIndices.value;
				std::memcpy(&new_asset->vertexInfo, &asset->vertInfo, sizeof(new_asset->vertexInfo));
				new_asset->verticies.value = asset->verts0.value;
				new_asset->vertListCount = static_cast<int>(asset->vertListCount);
				new_asset->rigidVertLists.value = asset->vertList.value;
				std::memcpy(new_asset->partBits, asset->partBits, sizeof(asset->partBits));
			}

			void compute_bounds(zonetool::iw4::Bounds& bounds, const float* mins, const float* maxs)
			{
				for (auto i = 0; i < 3; i++)
				{
					bounds.halfSize[i] = (maxs[i] - mins[i]) / 2.0f;
					bounds.midPoint[i] = bounds.halfSize[i] + mins[i];
				}
			}

			zonetool::iw4::cplane_s convert_plane(const zonetool::iw3::cplane_s& plane)
			{
				zonetool::iw4::cplane_s new_plane{};
				std::memcpy(new_plane.normal, plane.normal, sizeof(plane.normal));
				new_plane.dist = plane.dist;
				new_plane.type = plane.type;
				new_plane.signbits = 0;
				return new_plane;
			}

			// IW3 phys geom types: none, box, brushmodel, brush, cylinder, capsule. IW4 inserts
			// collmap at 4, which moves cylinder and capsule up by one.
			int convert_geom_type(const int type)
			{
				switch (type)
				{
				case 4:
					return zonetool::iw4::PHYS_GEOM_CYLINDER;
				case 5:
					return zonetool::iw4::PHYS_GEOM_CAPSULE;
				default:
					return type;
				}
			}

			unsigned char clamp_byte(const int value)
			{
				return static_cast<unsigned char>(value < 0 || value > 255 ? 255 : value);
			}

			zonetool::iw4::BrushWrapper* convert_brush_wrapper(const zonetool::iw3::BrushWrapper* brush, x86::allocator& allocator)
			{
				const auto new_brush = allocator.allocate<zonetool::iw4::BrushWrapper>();
				compute_bounds(new_brush->bounds, brush->mins, brush->maxs);

				const auto side_count = std::min(brush->numsides, 0xFFFFu);
				new_brush->brush.numsides = static_cast<unsigned short>(side_count);
				new_brush->brush.glassPieceIndex = 0;

				const auto source_planes = brush->planes.get();
				zonetool::iw4::cplane_s* planes = nullptr;
				if (side_count && source_planes)
				{
					planes = allocator.allocate<zonetool::iw4::cplane_s>(side_count);
					for (auto i = 0u; i < side_count; i++)
					{
						planes[i] = convert_plane(source_planes[i]);
					}
				}
				new_brush->planes = planes;

				const auto source_sides = brush->sides.get();
				if (side_count && source_sides)
				{
					const auto sides = allocator.allocate<zonetool::iw4::cbrushside_t>(side_count);
					for (auto i = 0u; i < side_count; i++)
					{
						const auto& side = source_sides[i];
						const auto source_plane = side.plane.get();
						if (source_plane)
						{
							// a side points into the brush's own plane array; keep the same index
							const auto index = source_planes ? source_plane - source_planes : -1;
							if (planes && index >= 0 && index < static_cast<std::ptrdiff_t>(side_count))
							{
								sides[i].plane = &planes[index];
							}
							else
							{
								const auto plane = allocator.allocate<zonetool::iw4::cplane_s>();
								*plane = convert_plane(*source_plane);
								sides[i].plane = plane;
							}
						}

						sides[i].materialNum = static_cast<unsigned short>(side.materialNum);
						sides[i].firstAdjacentSideOffset = static_cast<char>(clamp_byte(side.firstAdjacentSideOffset));
						sides[i].edgeCount = side.edgeCount;
					}
					new_brush->brush.sides = sides;
				}

				new_brush->totalEdgeCount = brush->totalEdgeCount;
				if (brush->totalEdgeCount > 0 && brush->baseAdjacentSide)
				{
					const auto edges = allocator.allocate<unsigned char>(brush->totalEdgeCount);
					std::memcpy(edges, brush->baseAdjacentSide.get(), brush->totalEdgeCount);
					new_brush->brush.baseAdjacentSide = edges;
				}

				for (auto a = 0; a < 2; a++)
				{
					for (auto b = 0; b < 3; b++)
					{
						new_brush->brush.axialMaterialNum[a][b] = brush->axialMaterialNum[a][b];
						new_brush->brush.firstAdjacentSideOffsets[a][b] = clamp_byte(brush->firstAdjacentSideOffsets[a][b]);
						new_brush->brush.edgeCount[a][b] = static_cast<unsigned char>(brush->edgeCount[a][b]);
					}
				}

				return new_brush;
			}

			// IW3 models carry their physics volumes as physGeoms; IW4 and later as a PhysCollmap.
			// The IW7 conversion builds dynamic Havok bodies from the collmap, so without this
			// CoD4 clutter (bikes, fruit, buckets) would get no dynamic body.
			zonetool::iw4::PhysCollmap* convert_phys_geoms(const zonetool::iw3::XModel* asset, x86::allocator& allocator)
			{
				const auto geoms = asset->physGeoms.get();
				if (!geoms || !geoms->count || !geoms->geoms)
				{
					return nullptr;
				}

				const auto collmap = allocator.allocate<zonetool::iw4::PhysCollmap>();
				collmap->name = allocator.duplicate_string(asset->name.get());
				collmap->count = geoms->count;
				const auto new_geoms = allocator.allocate<zonetool::iw4::PhysGeomInfo>(geoms->count);
				collmap->geoms = new_geoms;

				static_assert(sizeof(zonetool::iw3::PhysMass) == sizeof(zonetool::iw4::PhysMass));
				std::memcpy(&collmap->mass, &geoms->mass, sizeof(collmap->mass));

				// IW3 authors inertia about the centre of mass, IW4 about the model origin:
				// shift it by the parallel axis theorem (per unit mass).
				{
					const auto x = collmap->mass.centerOfMass[0];
					const auto y = collmap->mass.centerOfMass[1];
					const auto z = collmap->mass.centerOfMass[2];
					collmap->mass.momentsOfInertia[0] += y * y + z * z;
					collmap->mass.momentsOfInertia[1] += x * x + z * z;
					collmap->mass.momentsOfInertia[2] += x * x + y * y;
					collmap->mass.productsOfInertia[0] -= x * y;
					collmap->mass.productsOfInertia[1] -= x * z;
					collmap->mass.productsOfInertia[2] -= y * z;
				}

				float mins[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
				float maxs[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

				for (auto i = 0u; i < geoms->count; i++)
				{
					const auto& geom = geoms->geoms.get()[i];
					auto& new_geom = new_geoms[i];

					const auto brush = geom.brush.get();
					new_geom.brush = brush ? convert_brush_wrapper(brush, allocator) : nullptr;
					new_geom.type = convert_geom_type(geom.type);
					std::memcpy(new_geom.orientation, geom.orientation, sizeof(geom.orientation));

					for (auto axis = 0; axis < 3; axis++)
					{
						new_geom.bounds.midPoint[axis] = geom.offset[axis];
						new_geom.bounds.halfSize[axis] = geom.halfLengths[axis];
					}

					// IW3 packs cylinders and capsules as (half height, radius, unused) with the
					// symmetry axis on local X
					if ((new_geom.type == zonetool::iw4::PHYS_GEOM_CYLINDER || new_geom.type == zonetool::iw4::PHYS_GEOM_CAPSULE)
						&& geom.halfLengths[2] == 0.0f)
					{
						new_geom.bounds.halfSize[2] = geom.halfLengths[1];
					}

					for (auto axis = 0; axis < 3; axis++)
					{
						const auto extent =
							std::fabs(new_geom.orientation[axis][0]) * new_geom.bounds.halfSize[0] +
							std::fabs(new_geom.orientation[axis][1]) * new_geom.bounds.halfSize[1] +
							std::fabs(new_geom.orientation[axis][2]) * new_geom.bounds.halfSize[2];
						mins[axis] = std::min(mins[axis], new_geom.bounds.midPoint[axis] - extent);
						maxs[axis] = std::max(maxs[axis], new_geom.bounds.midPoint[axis] + extent);
					}
				}

				compute_bounds(collmap->bounds, mins, maxs);
				return collmap;
			}

			x86::pointer<zonetool::iw4::Material>* convert_materials(const zonetool::iw3::XModel* asset, x86::allocator& allocator)
			{
				const auto materials = asset->materialHandles.get();
				if (!materials)
				{
					return nullptr;
				}

				std::unordered_map<zonetool::iw3::Material*, zonetool::iw4::Material*> converted;
				const auto new_materials = allocator.allocate_array<x86::pointer<zonetool::iw4::Material>>(asset->numsurfs);
				for (auto i = 0; i < asset->numsurfs; i++)
				{
					const auto material = materials[i].get();
					if (!material)
					{
						continue;
					}

					auto& new_material = converted[material];
					if (!new_material)
					{
						new_material = material::convert(material, allocator);
					}

					new_materials[i] = new_material;
				}

				return new_materials;
			}
		}

		zonetool::iw4::XModel* convert(zonetool::iw3::XModel* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::XModel>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(numBones);
			COPY_VALUE(numRootBones);
			new_asset->numSurfaces = asset->numsurfs;
			COPY_VALUE(lodRampType);
			new_asset->scale = 1.0f;

			COPY_POINTER_RAW(boneNames);
			COPY_POINTER_RAW(parentList);
			COPY_POINTER_RAW(partClassification);
			static_assert(sizeof(zonetool::iw3::XModelQuat) == sizeof(zonetool::iw4::XModelAngle));
			new_asset->tagAngles.value = asset->quats.value;
			new_asset->tagPositions.value = asset->trans.value;
			static_assert(sizeof(zonetool::iw3::DObjAnimMat) == sizeof(zonetool::iw4::DObjAnimMat));
			new_asset->animMatrix.value = asset->baseMat.value;
			new_asset->materials = convert_materials(asset, allocator);

			for (auto i = 0; i < asset->numLods && i < 4; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = new_asset->lods[i];

				new_lod.dist = lod.dist;
				new_lod.numSurfacesInLod = static_cast<short>(lod.numsurfs);
				new_lod.surfIndex = static_cast<short>(lod.surfIndex);
				std::memcpy(new_lod.partBits, lod.partBits, sizeof(lod.partBits));
				new_lod.lod = lod.lod;

				const auto surfaces = allocator.allocate<zonetool::iw4::XModelSurfs>();
				const auto name = std::string(asset->name.get()) + "_lod" + std::to_string(i);
				surfaces->name = allocator.duplicate_string(name);
				surfaces->numsurfs = lod.numsurfs;
				std::memcpy(surfaces->partBits, lod.partBits, sizeof(lod.partBits));

				const auto surfs = allocator.allocate_array<zonetool::iw4::XSurface>(lod.numsurfs);
				surfaces->surfs = surfs;
				for (auto surf = 0; surf < lod.numsurfs; surf++)
				{
					convert_surface(&asset->surfs.get()[lod.surfIndex + surf], &surfs[surf]);
				}

				new_lod.surfaces = surfaces;
			}

			new_asset->numLods = static_cast<char>(asset->numLods);
			new_asset->collLod = static_cast<char>(asset->collLod);
			new_asset->flags = static_cast<char>(asset->flags);

			new_asset->numColSurfs = asset->numCollSurfs;
			new_asset->contents = asset->contents;
			if (asset->collSurfs && asset->numCollSurfs > 0)
			{
				const auto surfaces = allocator.allocate_array<zonetool::iw4::XModelCollSurf_s>(asset->numCollSurfs);
				for (auto i = 0; i < asset->numCollSurfs; i++)
				{
					const auto& source = asset->collSurfs.get()[i];
					surfaces[i].collTris.value = source.collTris.value;
					surfaces[i].numCollTris = source.numCollTris;
					compute_bounds(surfaces[i].bounds, source.mins, source.maxs);
					surfaces[i].boneIdx = source.boneIdx;
					surfaces[i].contents = source.contents;
					surfaces[i].surfFlags = source.surfFlags;
				}

				new_asset->collSurfs = surfaces;
			}

			if (asset->boneInfo && asset->numBones > 0)
			{
				const auto infos = allocator.allocate_array<zonetool::iw4::XBoneInfo>(asset->numBones);
				for (auto i = 0; i < asset->numBones; i++)
				{
					const auto& source = asset->boneInfo.get()[i];
					infos[i].radiusSquared = source.radiusSquared;
					compute_bounds(infos[i].packedBounds, source.bounds[0], source.bounds[1]);
					for (auto axis = 0; axis < 3; axis++)
					{
						infos[i].packedBounds.midPoint[axis] += source.offset[axis];
					}
				}

				new_asset->boneInfo = infos;
			}

			COPY_VALUE(radius);
			new_asset->memUsage = asset->memUsage;
			new_asset->bad = asset->bad;
			new_asset->physPreset.value = asset->physPreset.value;
			new_asset->physCollmap = convert_phys_geoms(asset, allocator);
			compute_bounds(new_asset->bounds, asset->mins, asset->maxs);

			return new_asset;
		}
	}
}
