#include <std_include.hpp>
#include "xmodelsurfs.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_surface(zone_reader32& reader, XSurface& surface)
		{
			const auto& counts = surface.vertInfo.vertCount;
			reader.read_array(surface.vertInfo.vertsBlend, 1, counts[0] + 3 * counts[1] + 5 * counts[2] + 7 * counts[3]);

			reader.push_stream(XFILE_BLOCK_VERTEX);
			reader.read_array(surface.verts0.packedVerts0, 15, surface.vertCount);
			reader.pop_stream();

			if (const auto lists = reader.read_array(surface.vertList, 3, surface.vertListCount))
			{
				for (auto i = 0u; i < surface.vertListCount; i++)
				{
					if (const auto tree = reader.read_single(lists[i].collisionTree, 3))
					{
						reader.read_array(tree->nodes, 15, tree->nodeCount);
						reader.read_array(tree->leafs, 1, tree->leafCount);
					}
				}
			}

			reader.push_stream(XFILE_BLOCK_INDEX);
			reader.read_array(surface.triIndices, 15, surface.triCount * 3);
			reader.pop_stream();
		}
	}

	void xmodel_surfs::read(zone_reader32& reader, XModelSurfs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfs, 3, asset->numsurfs))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				read_surface(reader, surfaces[i]);
			}
		}

		reader.pop_stream();
	}
}
