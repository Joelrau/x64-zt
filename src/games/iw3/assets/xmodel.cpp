#include <std_include.hpp>
#include "xmodel.hpp"

namespace zonetool::iw3
{
	namespace
	{
		void read_collision_tree(zone_reader32& reader, XSurfaceCollisionTree* tree)
		{
			reader.read_array(tree->nodes, 15, tree->nodeCount);
			reader.read_array(tree->leafs, 1, tree->leafCount);
		}

		void read_surface(zone_reader32& reader, XSurface* surface)
		{
			const auto& info = surface->vertInfo;
			reader.read_array(surface->vertInfo.vertsBlend, 1,
				info.vertCount[0] + 3 * info.vertCount[1] + 5 * info.vertCount[2] + 7 * info.vertCount[3]);

			reader.push_stream(XFILE_BLOCK_VERTEX);
			reader.read_array(surface->verts0, 15, surface->vertCount);
			reader.pop_stream();

			if (const auto lists = reader.read_array(surface->vertList, 3, surface->vertListCount))
			{
				for (auto i = 0u; i < surface->vertListCount; i++)
				{
					if (const auto tree = reader.read_single(lists[i].collisionTree, 3))
					{
						read_collision_tree(reader, tree);
					}
				}
			}

			reader.push_stream(XFILE_BLOCK_INDEX);
			reader.read_array(surface->triIndices, 15, surface->triCount);
			reader.pop_stream();
		}

		void read_brush(zone_reader32& reader, BrushWrapper* brush)
		{
			if (const auto sides = reader.read_array(brush->sides, 3, brush->numsides))
			{
				for (auto i = 0u; i < brush->numsides; i++)
				{
					reader.read_single(sides[i].plane, 3);
				}
			}

			reader.read_array(brush->baseAdjacentSide, 0, brush->totalEdgeCount);
			reader.read_array(brush->planes, 3, brush->numsides);
		}

		void read_physics_geometry(zone_reader32& reader, PhysGeomList* list)
		{
			if (const auto geoms = reader.read_array(list->geoms, 3, list->count))
			{
				for (auto i = 0u; i < list->count; i++)
				{
					if (const auto brush = reader.read_single(geoms[i].brush, 3))
					{
						read_brush(reader, brush);
					}
				}
			}
		}
	}

	void xmodel::read(zone_reader32& reader, XModel* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto bone_count = asset->numBones;
		const auto child_count = asset->numBones - asset->numRootBones;

		if (const auto names = reader.read_array(asset->boneNames, 1, bone_count))
		{
			reader.read_script_strings(names, bone_count);
		}

		reader.read_array(asset->parentList, 0, child_count);
		reader.read_array(asset->quats, 1, child_count);
		reader.read_array(asset->trans, 3, child_count * 4);
		reader.read_array(asset->partClassification, 0, bone_count);
		reader.read_array(asset->baseMat, 3, bone_count);

		if (const auto surfaces = reader.read_array(asset->surfs, 3, asset->numsurfs))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				read_surface(reader, &surfaces[i]);
			}
		}

		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materialHandles, asset->numsurfs);

		if (const auto surfaces = reader.read_array(asset->collSurfs, 3, asset->numCollSurfs))
		{
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				reader.read_array(surfaces[i].collTris, 3, surfaces[i].numCollTris);
			}
		}

		reader.read_array(asset->boneInfo, 3, bone_count);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);

		if (const auto geometry = reader.read_single(asset->physGeoms, 3))
		{
			read_physics_geometry(reader, geometry);
		}

		reader.pop_stream();
	}
}
