#include <std_include.hpp>
#include "stterrain.hpp"

namespace zonetool::s4
{
	namespace
	{
		std::size_t vertex_data_size(const StMesh& mesh)
		{
			return (mesh.flags & ST_MESH_GPU_GENERATE ? 4 : 8) * mesh.vertexCount;
		}

		std::size_t layer_mask_size(const StMesh& mesh)
		{
			return 4 * (mesh.triCount - mesh.cutoutTriCount);
		}

		template <typename Walk>
		void walk_mesh(Walk walk, StMesh& mesh)
		{
			walk.array(mesh.vd0.data, vertex_data_size(mesh), 127);
			walk.array(mesh.layerMask.data, layer_mask_size(mesh), 127);
			walk.array(mesh.indices, 3 * mesh.triCount, 3);
			walk.asset(ASSET_TYPE_STREAM_KEY, mesh.streamKey);
		}

		template <typename Walk>
		void walk_grid(Walk walk, StTerrainGrid& grid)
		{
			if (const auto levels = walk.array(grid.levels, grid.numLevels, 7))
			{
				for (auto i = 0u; i < grid.numLevels; i++)
				{
					const auto count = levels[i].width * levels[i].height;
					if (const auto nodes = walk.array(levels[i].nodes, count, 7))
					{
						for (auto j = 0u; j < count; j++)
						{
							walk_mesh(walk, nodes[j].mesh);
						}
					}
				}
			}
		}

		template <typename Walk>
		void walk_surface(Walk walk, StDiskTerrainSurface& surface)
		{
			walk.array(surface.highLODIndices, surface.highLODIndexCount, 3);
			walk_grid(walk, surface.grid);

			if (const auto layers = walk.array(surface.layerMaterials, surface.layerMaterialsCount, 7))
			{
				for (auto i = 0u; i < surface.layerMaterialsCount; i++)
				{
					walk.asset(ASSET_TYPE_MATERIAL, layers[i].material);
					walk.array(layers[i].materialTextureIdxs, 5, 1);
				}
			}

			walk.array(surface.displacementScales, surface.displacementScalesCount, 3);
			walk.array(surface.clutterLayers, surface.clutterLayerCount, 3);
			walk.asset(ASSET_TYPE_STREAM_KEY, surface.clutterSampleBitmask.streamKey);
			walk.array(surface.clutterSampleBitmask.bitmask, surface.clutterSampleBitmask.wordCount, 3);
			walk.array(surface.clutterTileInstanceCounts, surface.clutterTileCount, 3);
			walk.array(surface.layerMaskMapIdxs, surface.layerMaskMapIdxsCount, 1);
			walk.asset(ASSET_TYPE_IMAGE, surface.heightMap);
			walk.asset(ASSET_TYPE_IMAGE, surface.cutoutMap);
			walk.asset(ASSET_TYPE_IMAGE, surface.combinedAlbedoMap);
			walk.asset(ASSET_TYPE_IMAGE, surface.combinedNogMap);
		}

		template <typename Walk>
		void walk_st_terrain(Walk walk, StTerrain& asset)
		{

			if (const auto surfaces = walk.array(asset.surfaces, asset.surfaceCount, 7))
			{
				for (auto i = 0u; i < asset.surfaceCount; i++)
				{
					walk_surface(walk, surfaces[i]);
				}
			}

			walk.array(asset.maps, asset.mapsCount, 3);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, asset.flattenedImages, asset.flattenedImagesCount);
			walk_asset_array(walk, ASSET_TYPE_IMAGE, asset.unknownImages, asset.unknownImagesCount);
			walk.array(asset.clutterSamplePoints.samplePoints, asset.clutterSamplePoints.sampleCount, 3);

			if (const auto lightmaps = walk.array(asset.lightmaps, asset.lightmapCount, 7))
			{
				for (auto i = 0u; i < asset.lightmapCount; i++)
				{
					for (auto& image : lightmaps[i].images)
					{
						walk.asset(ASSET_TYPE_IMAGE, image);
					}
				}
			}

			walk.array(asset.unknownRecords, asset.unknownRecordCount, 3);
		}
	}

	void st_terrain::read(zone_reader& reader, StTerrain* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_st_terrain(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void st_terrain::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_st_terrain(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void st_terrain::dump(StTerrain* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("stterrain\\"s + asset->name + ".stterrainb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_st_terrain(dump_walk{dump}, *asset);
		dump.close();
	}

	StTerrain* st_terrain::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("stterrain\\"s + name + ".stterrainb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing stterrain \"%s\"...", name.data());

		auto* asset = read.read_single<StTerrain>();
		asset->name = read.read_string();
		walk_st_terrain(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void st_terrain::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<StTerrain>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void st_terrain::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_st_terrain(depend_walk{zone}, *this->asset_);
	}

	std::string st_terrain::name()
	{
		return this->name_;
	}

	std::int32_t st_terrain::type()
	{
		return ASSET_TYPE_ST_TERRAIN;
	}
}
