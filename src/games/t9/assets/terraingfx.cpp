#include <std_include.hpp>
#include "terraingfx.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_mesh(auto& reader, TerrainMesh& mesh)
		{
			reader.read_array(mesh.data1, 127, mesh.data1Size);
			reader.read_array(mesh.data2, 127, mesh.data2Size);
			reader.read_array(mesh.values, 1, mesh.valueCount);
			reader.read_asset(ASSET_TYPE_STREAMKEY, mesh.streamKey);
		}

		void read_cell_grids(auto& reader, TerrainCellGridList& list)
		{
			const auto grids = reader.read_array(list.grids, 7, list.gridCount);
			if (!grids)
			{
				return;
			}

			for (auto i = 0u; i < list.gridCount; i++)
			{
				const auto cell_count = static_cast<std::size_t>(grids[i].sizeX) * grids[i].sizeY;
				if (const auto cells = reader.read_array(grids[i].cells, 15, cell_count))
				{
					for (auto j = 0u; j < cell_count; j++)
					{
						if (const auto cell_material = reader.read_array(cells[j].cellMaterial, 15))
						{
							reader.read_asset(ASSET_TYPE_MATERIAL, cell_material->material);
						}

						read_mesh(reader, cells[j].mesh);
					}
				}
			}
		}

		void read_tile(auto& reader, TerrainTile& tile)
		{
			if (const auto meshes = reader.read_array(tile.meshes, 15, tile.meshCount))
			{
				for (auto i = 0u; i < tile.meshCount; i++)
				{
					read_mesh(reader, meshes[i]);
				}
			}

			if (const auto materials = reader.read_array(tile.meshMaterials, 15, tile.meshCount))
			{
				for (auto i = 0u; i < tile.meshCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, materials[i].material);
				}
			}

			reader.read_array(tile.indices, 1, tile.indexCount);
			read_cell_grids(reader, tile.cellGrids);
			reader.read_array(tile.values1, 3, tile.values1Count);
			reader.read_array(tile.values2, 3, tile.values2Count);

			if (const auto layers = reader.read_array(tile.layers, 7, tile.layerCount))
			{
				for (auto i = 0ull; i < tile.layerCount; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, layers[i].material);
					reader.read_array(layers[i].data, 1);
				}
			}

			reader.read_asset(ASSET_TYPE_IMAGE, tile.image1);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image2);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image3);
			reader.read_asset_array(ASSET_TYPE_IMAGE, tile.images, tile.imageCount);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image4);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image5);
			reader.read_array(tile.values3, 1, tile.values3Count);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image6);
			reader.read_asset(ASSET_TYPE_IMAGE, tile.image7);
		}

		void read_region(auto& reader, TerrainRegion& region)
		{
			if (const auto parts = reader.read_array(region.parts, 7, region.partCount))
			{
				for (auto i = 0u; i < region.partCount; i++)
				{
					auto& part = parts[i];
					reader.push_stream(XFILE_BLOCK_MEMMAPPED);
					reader.read_array(part.values, 1, part.bitCount);
					reader.pop_stream();

					reader.push_stream(XFILE_BLOCK_MEMMAPPED);
					reader.read_array(part.bits, 0, (part.bitCount + 7u) >> 3);
					reader.pop_stream();

					reader.push_stream(XFILE_BLOCK_MEMMAPPED);
					reader.read_array(part.data, 0, part.dataSize);
					reader.pop_stream();
				}
			}

			if (const auto entries = reader.read_array(region.entries, 7, region.entryCount))
			{
				for (auto i = 0u; i < region.entryCount; i++)
				{
					reader.read_array(entries[i], 7);
				}
			}
		}

		void read_image_block(auto& reader, TerrainImageBlock& block)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, block.image1);
			reader.read_asset(ASSET_TYPE_IMAGE, block.image2);

			if (const auto buffer = reader.read_array(block.buffer, 15))
			{
				read_gfx_buffer(reader, buffer);
			}

			if (const auto sets = reader.read_array(block.imageSets, 7, block.imageSetCount))
			{
				for (auto i = 0ull; i < block.imageSetCount; i++)
				{
					for (auto& image : sets[i].images)
					{
						reader.read_asset(ASSET_TYPE_IMAGE, image);
					}
				}
			}
		}
	}

	void read_terrain_regions(auto& reader, TerrainRegionList* list)
	{
		if (const auto regions = reader.read_array(list->regions, 7, list->regionCount))
		{
			for (auto i = 0u; i < list->regionCount; i++)
			{
				read_region(reader, regions[i]);
			}
		}
	}

	void terrain_gfx::read(auto& reader, TerrainGfx* asset)
	{
		if (const auto tiles = reader.read_array(asset->tiles, 7, asset->tileCount))
		{
			for (auto i = 0u; i < asset->tileCount; i++)
			{
				read_tile(reader, tiles[i]);
			}
		}

		reader.read_asset_array(ASSET_TYPE_IMAGE, asset->images, asset->imageCount);
		reader.read_array(asset->values1, 3, asset->values1Count);

		if (const auto list = reader.read_array(asset->regionList, 7))
		{
			read_terrain_regions(reader, list);
		}

		reader.read_array(asset->values2, 7, asset->values2Count);

		if (const auto blends = reader.read_array(asset->blends, 7, asset->blendCount))
		{
			for (auto i = 0ull; i < asset->blendCount; i++)
			{
				reader.read_array(blends[i].values1, 3, blends[i].values1Count);
				reader.read_array(blends[i].values2, 3, blends[i].values2Count);
				reader.read_asset(ASSET_TYPE_IMAGE, blends[i].image);
			}
		}

		if (const auto patches = reader.read_array(asset->patches, 7, asset->patchCount))
		{
			for (auto i = 0ull; i < asset->patchCount; i++)
			{
				reader.read_array(patches[i].values, 3, patches[i].valueCount);
			}
		}

		reader.read_array(asset->values3, 7, asset->values3Count);
		read_image_block(reader, asset->imageBlock);
	}
}

namespace zonetool::t9
{
	template void read_terrain_regions(zone_reader&, TerrainRegionList*);
	template void terrain_gfx::read(zone_reader&, TerrainGfx*);
	template void read_terrain_regions(zone_writer&, TerrainRegionList*);
	template void terrain_gfx::read(zone_writer&, TerrainGfx*);
}
