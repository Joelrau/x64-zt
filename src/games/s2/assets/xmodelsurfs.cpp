#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_collision_tree(zone_reader& reader, XSurfaceCollisionLeaf* tree)
		{
			reader.read_array(tree->nodes, 15, tree->nodeCount);
			reader.read_array(tree->leafs, 1, tree->leafCount);
		}

		void read_surface(zone_reader& reader, XSurface* surface)
		{
			reader.read_asset(ASSET_TYPE_XSURFACE_SHARED, surface->shared);
			reader.read_stream(surface->triIndices, 15, 6, surface->triCount);

			if (const auto lists = reader.read_array(surface->rigidVertLists, 7, surface->rigidVertListCount))
			{
				for (auto i = 0; i < surface->rigidVertListCount; i++)
				{
					if (const auto tree = reader.read_single(lists[i].collisionTree, 7))
					{
						read_collision_tree(reader, tree);
					}
				}
			}

			reader.read_array(surface->verts, 15, surface->vertCount);
		}
	}

	void xmodel_surfs::read_surfaces(zone_reader& reader, XSurface* surfaces, const std::size_t count)
	{
		for (std::size_t i = 0; i < count; i++)
		{
			read_surface(reader, &surfaces[i]);
		}
	}

	void xmodel_surfs::read(zone_reader& reader, XModelSurfs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfs, 7, asset->numsurfs))
		{
			read_surfaces(reader, surfaces, asset->numsurfs);
		}

		reader.pop_stream();
	}
}
