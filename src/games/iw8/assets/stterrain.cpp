#include <std_include.hpp>
#include "stterrain.hpp"

namespace zonetool::iw8
{
	namespace
	{
		std::size_t vertex_data_size(const StMesh& mesh)
		{
			return (mesh.flags & ST_MESH_GPU_GENERATE ? 4 : 8) * mesh.vertexCount;
		}

		std::size_t layer_mask_size(const StMesh& mesh)
		{
			return mesh.flags & ST_MESH_LAYER_MASK ? 4 * (mesh.triCount - mesh.cutoutTriCount) : 0;
		}

		void read_mesh(zone_reader& reader, StMesh& mesh)
		{
			reader.read_array(mesh.vd0.data, 127, vertex_data_size(mesh));
			reader.read_array(mesh.layerMask.data, 127, layer_mask_size(mesh));
			reader.read_array(mesh.indices, 3, 3 * mesh.triCount);
			reader.read_asset(ASSET_TYPE_STREAM_KEY, mesh.streamKey);
		}

		void read_grid(zone_reader& reader, StTerrainGrid& grid)
		{
			if (const auto levels = reader.read_array(grid.levels, 7, grid.numLevels))
			{
				for (auto i = 0u; i < grid.numLevels; i++)
				{
					const auto count = levels[i].width * levels[i].height;
					if (const auto nodes = reader.read_array(levels[i].nodes, 7, count))
					{
						for (auto j = 0u; j < count; j++)
						{
							read_mesh(reader, nodes[j].mesh);
						}
					}
				}
			}
		}

		void read_surface(zone_reader& reader, StDiskTerrainSurface& surface)
		{
			reader.read_array(surface.highLODIndices, 3, surface.highLODIndexCount);
			read_grid(reader, surface.grid);

			if (const auto layers = reader.read_array(surface.layerMaterials, 7, surface.layerMaterialsCount))
			{
				for (auto i = 0u; i < surface.layerMaterialsCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, layers[i].material);
					reader.read_array(layers[i].materialTextureIdxs, 1, 4);
				}
			}

			reader.read_array(surface.displacementScales, 3, surface.displacementScalesCount);
			reader.read_array(surface.clutterLayers, 3, surface.clutterLayerCount);
			reader.read_asset(ASSET_TYPE_STREAM_KEY, surface.clutterSampleBitmask.streamKey);
			reader.read_array(surface.clutterSampleBitmask.bitmask, 3, surface.clutterSampleBitmask.wordCount);
			reader.read_array(surface.clutterTileInstanceCounts, 3, surface.clutterTileCount);
			reader.read_array(surface.layerMaskMapIdxs, 1, surface.layerMaskMapIdxsCount);
			reader.read_asset(ASSET_TYPE_IMAGE, surface.heightMap);
			reader.read_asset(ASSET_TYPE_IMAGE, surface.cutoutMap);
			reader.read_asset(ASSET_TYPE_IMAGE, surface.combinedAlbedoMap);
			reader.read_asset(ASSET_TYPE_IMAGE, surface.combinedNogMap);
		}
	}

	void st_terrain::read(zone_reader& reader, StTerrain* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto surfaces = reader.read_array(asset->surfaces, 7, asset->surfaceCount))
		{
			for (auto i = 0u; i < asset->surfaceCount; i++)
			{
				read_surface(reader, surfaces[i]);
			}
		}

		reader.read_array(asset->maps, 3, asset->mapsCount);
		reader.read_asset_array(ASSET_TYPE_IMAGE, asset->flattenedImages, asset->flattenedImagesCount);
		reader.read_array(asset->clutterSamplePoints.samplePoints, 3, asset->clutterSamplePoints.sampleCount);

		if (const auto lightmaps = reader.read_array(asset->lightmaps, 7, asset->lightmapCount))
		{
			for (auto i = 0u; i < asset->lightmapCount; i++)
			{
				for (auto& image : lightmaps[i].images)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, image);
				}
			}
		}
		reader.pop_stream();
	}
}
