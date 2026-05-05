#include "std_include.hpp"
#include "xsurface.hpp"

namespace zonetool::iw8
{
	void parse_subdiv(XSurface* surf, assetmanager::reader& reader)
	{
		if (surf->subdiv)
		{
			surf->subdiv = reader.read_single<XSurfaceSubdivInfo>();

			if (surf->subdiv->levels)
			{
				surf->subdiv->levels = reader.read_array<XSurfaceSubdivLevel>();
				auto* levels = surf->subdiv->levels;
				auto level_count = surf->subdivLevelCount;
				for (unsigned char level_index = 0; level_index < level_count; level_index++)
				{
					auto* level = &levels[level_index];

					level->rigidVertLists = reader.read_raw<XSubdivRigidVertList>();
					level->faceIndices = reader.read_raw<unsigned short>();
					level->regularPatchIndices = reader.read_raw<unsigned short>();
					level->regularPatchFlags = reader.read_raw<unsigned int>();
					level->facePoints = reader.read_raw<unsigned int>();
					level->edgePoints = reader.read_raw<unsigned int>();
					level->vertexPoints = reader.read_raw<unsigned int>();
					level->normals = reader.read_raw<unsigned int>();
					level->transitionPoints = reader.read_raw<unsigned int>();
					level->regularPatchCones = reader.read_raw<float>();
				}
			}
		}
	}

	XModelSurfs* xsurface::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "xsurface\\" + name + ".xsb";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodel surface \"%s\"...", name.data());

		auto* asset = read.read_single<XModelSurfs>();
		asset->name = read.read_string();

		asset->surfs = read.read_array<XSurface>();
		for (unsigned short i = 0; i < asset->numsurfs; i++)
		{
			asset->surfs[i].shared = read.read_single<XSurfaceShared>();
			if (asset->surfs[i].shared->data.residentData)
			{
				asset->surfs[i].shared->data.residentData = read.read_array<char>();
			}

			asset->surfs[i].lmapCoords = read.read_array<PackedLmapCoords>();

			asset->surfs[i].rigidVertLists = read.read_array<XRigidVertList>();
			/*
			for (unsigned char vert = 0; vert < asset->surfs[i].rigidVertListCount; vert++)
			{
				if (asset->surfs[i].rigidVertLists[vert].collisionTree)
				{
					asset->surfs[i].rigidVertLists[vert].collisionTree = read.read_single<XSurfaceCollisionTree>();

					if (asset->surfs[i].rigidVertLists[vert].collisionTree->leafs)
					{
						asset->surfs[i].rigidVertLists[vert].collisionTree->leafs = read.read_array<XSurfaceCollisionLeaf>();
					}
					if (asset->surfs[i].rigidVertLists[vert].collisionTree->nodes)
					{
						asset->surfs[i].rigidVertLists[vert].collisionTree->nodes = read.read_array<XSurfaceCollisionNode>();
					}
				}
			}
			*/

			asset->surfs[i].blendVerts = read.read_raw<XBlendInfo>();

			parse_subdiv(&asset->surfs[i], read);

			if (asset->surfs[i].childBounds)
				asset->surfs[i].childBounds = read.read_array<Bounds>();

			if (asset->surfs[i].blendShapesPerVert)
			{
				asset->surfs[i].blendShapesPerVert = read.read_array<BlendShapesPerVert>();

				asset->surfs[i].blendShapesPerVert->perVertBlendShapeData = read.read_array<unsigned __int64>();
				asset->surfs[i].blendShapesPerVert->surfBlendShapesData = read.read_array<unsigned short>();
			}

			if (asset->surfs[i].blendShapesRecalcTangentFrameData)
			{
				asset->surfs[i].blendShapesRecalcTangentFrameData = read.read_single<BlendShapesRecalcTangentFrameData>();
				asset->surfs[i].blendShapesRecalcTangentFrameData->topologicalData = read.read_array<unsigned short>();
			}
		}

		asset->shared = read.read_single<XSurfaceShared>();
		if (asset->shared->data.residentData)
		{
			asset->shared->data.residentData = read.read_array<char>();
		}

		read.close();

		return asset;
	}

	void xsurface::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).modelSurfs;
		}
	}

	void xsurface::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void xsurface::load_depending(zone_base* zone)
	{
	}

	std::string xsurface::name()
	{
		return this->name_;
	}

	std::int32_t xsurface::type()
	{
		return ASSET_TYPE_XMODEL_SURFS;
	}

	void write_xsurface(zone_base* zone, zone_buffer* buf, XSurface* dest, XSurface* data)
	{
		/*
		if (data->verts0.verts0)
		{
			buf->align(15);
			if ((data->flags & 8) != 0)
			{
				buf->write(reinterpret_cast<GfxPackedMotionVertex*>(data->verts0.verts0), data->vertCount);
			}
			else
			{
				buf->write(reinterpret_cast<GfxPackedVertex*>(data->verts0.verts0), data->vertCount);
			}

			buf->clear_pointer(&dest->verts0.verts0);
		}

		if (data->triIndices)
		{
			buf->align(15);
			buf->write(data->triIndices, data->triCount);
			buf->clear_pointer(&dest->triIndices);
		}
		*/

		/*
		// TODO
		if (data->shared)
		{
			buf->align(7);
			dest->shared = buf->write(data->shared);
			//

			if ( (dest->shared->flags & 1) != 0 )
			{

			}
		}

		if (data->rigidVertLists)
		{
			buf->align(7);
			dest->rigidVertLists = buf->write(data->rigidVertLists, data->rigidVertListCount);
			for (unsigned char vert = 0; vert < data->rigidVertListCount; vert++)
			{
				if (data->rigidVertLists[vert].collisionTree)
				{
					buf->align(7);
					dest->rigidVertLists[vert].collisionTree = buf->write(data->rigidVertLists[vert].collisionTree);

					if (data->rigidVertLists[vert].collisionTree->nodes)
					{
						buf->align(15);
						dest->rigidVertLists[vert].collisionTree->nodes = buf->write(data->rigidVertLists[vert].collisionTree->nodes, data->rigidVertLists[vert].collisionTree->nodeCount);
						buf->clear_pointer(&dest->rigidVertLists[vert].collisionTree->nodes);
					}

					if (data->rigidVertLists[vert].collisionTree->leafs)
					{
						buf->align(1);
						dest->rigidVertLists[vert].collisionTree->leafs = buf->write(data->rigidVertLists[vert].collisionTree->leafs, data->rigidVertLists[vert].collisionTree->leafCount);
						buf->clear_pointer(&dest->rigidVertLists[vert].collisionTree->leafs);
					}

					buf->clear_pointer(&dest->rigidVertLists[vert].collisionTree);
				}
			}
			buf->clear_pointer(&dest->rigidVertLists);
		}

		if (data->blendVerts)
		{
			buf->align(3);
			buf->write_stream(data->blendVerts, data->blendVertSize & 0xFFFFFFFE);
			buf->clear_pointer(&dest->blendVerts);
		}

		if (data->lmapUnwrap)
		{
			buf->align(3);
			buf->write_stream(data->lmapUnwrap, 8, data->vertCount);
			buf->clear_pointer(&dest->lmapUnwrap);
		}

		if (data->subdiv)
		{
			buf->align(15);
			dest->subdiv = buf->write(data->subdiv);
			if (data->subdiv->levels)
			{
				buf->align(7);
				dest->subdiv->levels = buf->write(data->subdiv->levels, data->subdivLevelCount);
				for (unsigned char level_index = 0; level_index < data->subdivLevelCount; level_index++)
				{
					if (data->subdiv->levels[level_index].rigidVertLists)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].rigidVertLists, 16, data->rigidVertListCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].rigidVertLists);
					}

					if (data->subdiv->levels[level_index].faceIndices)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].faceIndices, 12, data->subdiv->levels[level_index].faceCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].faceIndices);
					}

					if (data->subdiv->levels[level_index].regularPatchIndices)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].regularPatchIndices, 32, data->subdiv->levels[level_index].regularPatchCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].regularPatchIndices);
					}

					if (data->subdiv->levels[level_index].regularPatchFlags)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].regularPatchFlags, 4, data->subdiv->levels[level_index].regularPatchCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].regularPatchFlags);
					}

					if (data->subdiv->levels[level_index].facePoints)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].facePoints, 1, data->subdiv->levels[level_index].facePointBufferSize & 0xFFFFFFFC);
						buf->clear_pointer(&dest->subdiv->levels[level_index].facePoints);
					}

					if (data->subdiv->levels[level_index].edgePoints)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].edgePoints, 8, data->subdiv->levels[level_index].edgePointCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].edgePoints);
					}

					if (data->subdiv->levels[level_index].vertexPoints)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].vertexPoints, 1, data->subdiv->levels[level_index].vertexPointBufferSize & 0xFFFFFFFC);
						buf->clear_pointer(&dest->subdiv->levels[level_index].vertexPoints);
					}

					if (data->subdiv->levels[level_index].normals)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].normals, 1, data->subdiv->levels[level_index].normalBufferSize & 0xFFFFFFFC);
						buf->clear_pointer(&dest->subdiv->levels[level_index].normals);
					}

					if (data->subdiv->levels[level_index].transitionPoints)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].transitionPoints, 4, data->subdiv->levels[level_index].transitionPointCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].transitionPoints);
					}

					if (data->subdiv->levels[level_index].regularPatchCones)
					{
						buf->align(3);
						buf->write_stream(data->subdiv->levels[level_index].regularPatchCones, 32, data->subdiv->levels[level_index].regularPatchCount);
						buf->clear_pointer(&dest->subdiv->levels[level_index].regularPatchCones);
					}

					dest->subdiv->levels[level_index].regularPatchIndexBuffer = nullptr;
					dest->subdiv->levels[level_index].faceIndexBuffer = nullptr;
					dest->subdiv->levels[level_index].regularPatchIndexBufferView = nullptr;
					dest->subdiv->levels[level_index].regularPatchFlagsView = nullptr;
					dest->subdiv->levels[level_index].facePointsView = nullptr;
					dest->subdiv->levels[level_index].edgePointsView = nullptr;
					dest->subdiv->levels[level_index].vertexPointsView = nullptr;
					dest->subdiv->levels[level_index].normalsView = nullptr;
					dest->subdiv->levels[level_index].transitionPointsView = nullptr;
					dest->subdiv->levels[level_index].regularPatchConesView = nullptr;
				}
				buf->clear_pointer(&dest->subdiv->levels);
			}

			dest->subdiv->cache.subdivCacheBuffer = nullptr;
			dest->subdiv->cache.subdivCacheView = nullptr;

			buf->clear_pointer(&dest->subdiv);
		}

		if (data->tensionData)
		{
			buf->align(3);
			buf->write_stream(data->tensionData, 4, (data->blendVertCounts[0]
				+ data->blendVertCounts[1]
				+ data->blendVertCounts[2]
				+ data->blendVertCounts[3]
				+ data->blendVertCounts[4]
				+ data->blendVertCounts[5]
				+ data->blendVertCounts[6]
				+ data->blendVertCounts[7]));

			buf->clear_pointer(&dest->tensionData);
		}

		if (data->tensionAccumTable)
		{
			buf->align(1);
			buf->write_stream(data->tensionAccumTable, 32, data->vertCount);
			buf->clear_pointer(&dest->tensionAccumTable);
		}

		if ((data->flags & 0x500) == 0x500)
		{
			buf->align(3);
			buf->write_stream(data->shProbeSimplexVertData.data0, 32, data->vertCount);
			buf->clear_pointer(&dest->shProbeSimplexVertData.data0);
		}
		else if ((data->flags & 0x100) != 0)
		{
			buf->align(3);
			buf->write_stream(data->shProbeSimplexVertData.data1, 8, data->vertCount);
			buf->clear_pointer(&dest->shProbeSimplexVertData.data1);
		}
		else if ((data->flags & 0x400) != 0)
		{
			buf->align(3);
			buf->write_stream(data->shProbeSimplexVertData.data2, 24, data->vertCount);
			buf->clear_pointer(&dest->shProbeSimplexVertData.data2);
		}
		*/
	}

	void xsurface::write(zone_base* zone, zone_buffer* buf)
	{
		printf(__FUNCTION__ ": todo\n");
		/*
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL); // 5

		dest->name = buf->write_str(this->name());

		if (data->surfs)
		{
			buf->align(3); // TODO: are we sure its not 15?? even IW7 is
			dest->surfs = buf->write(data->surfs, data->numsurfs);
			for (unsigned short surf = 0; surf < data->numsurfs; surf++)
			{
				write_xsurface(zone, buf, &dest->surfs[surf], &data->surfs[surf]);
			}
		}

		buf->pop_stream();
		*/
	}

	void dump_subdiv(XSurface* surf, assetmanager::dumper& dumper)
	{
		if (surf->subdiv)
		{
			dumper.dump_single(surf->subdiv);

			if (surf->subdiv->levels)
			{
				dumper.dump_array(surf->subdiv->levels, surf->subdivLevelCount);
				auto* levels = surf->subdiv->levels;
				auto level_count = surf->subdivLevelCount;
				for (unsigned char level_index = 0; level_index < level_count; level_index++)
				{
					auto* level = &levels[level_index];

					dumper.dump_raw(level->rigidVertLists, 16 * surf->rigidVertListCount);
					dumper.dump_raw(level->faceIndices, 12 * level->faceCount);
					dumper.dump_raw(level->regularPatchIndices, 32 * level->regularPatchCount);
					dumper.dump_raw(level->regularPatchFlags, 4 * level->regularPatchCount);
					dumper.dump_raw(level->facePoints, level->facePointBufferSize & 0xFFFFFFFC);
					dumper.dump_raw(level->edgePoints, 8 * level->edgePointCount);
					dumper.dump_raw(level->vertexPoints, level->vertexPointBufferSize & 0xFFFFFFFC);
					dumper.dump_raw(level->normals, level->normalBufferSize & 0xFFFFFFFC);
					dumper.dump_raw(level->transitionPoints, 4 * level->transitionPointCount);
					dumper.dump_raw(level->regularPatchCones, 32 * level->regularPatchCount);
				}
			}
		}
	}

	void xsurface::dump(XModelSurfs* asset)
	{
		const auto path = "xsurface\\"s + asset->name + ".xsb";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_array(asset->surfs, asset->numsurfs);
		for (unsigned short i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_single(asset->surfs[i].shared);
			if (asset->surfs[i].shared->data.residentData)
				dump.dump_array(asset->surfs[i].shared->data.residentData, asset->surfs[i].shared->dataSize);

			if (asset->surfs[i].lmapCoords)
				dump.dump_array(asset->surfs[i].lmapCoords, asset->surfs[i].vertCount);

			dump.dump_array(asset->surfs[i].rigidVertLists, asset->surfs[i].rigidVertListCount);
			/*
			for (unsigned char vert = 0; vert < asset->surfs[i].rigidVertListCount; vert++)
			{
				if (asset->surfs[i].rigidVertLists)
				{
					if (asset->surfs[i].rigidVertLists[vert].collisionTree)
					{
						dump.dump_single(asset->surfs[i].rigidVertLists[vert].collisionTree);

						if (asset->surfs[i].rigidVertLists[vert].collisionTree->leafs)
						{
							dump.dump_array(asset->surfs[i].rigidVertLists[vert].collisionTree->leafs,
								asset->surfs[i].rigidVertLists[vert].collisionTree->leafCount);
						}
						if (asset->surfs[i].rigidVertLists[vert].collisionTree->nodes)
						{
							dump.dump_array(asset->surfs[i].rigidVertLists[vert].collisionTree->nodes,
								asset->surfs[i].rigidVertLists[vert].collisionTree->nodeCount);
						}
					}
				}
			}
			*/

			dump.dump_raw(asset->surfs[i].blendVerts, asset->surfs[i].blendVertSize & 0xFFFFFFFE);

			dump_subdiv(&asset->surfs[i], dump);

			if (asset->surfs[i].childBounds)
				dump.dump_array(asset->surfs[i].childBounds, asset->surfs[i].rigidVertListCount);

			if (asset->surfs[i].blendShapesPerVert)
			{
				dump.dump_array(asset->surfs[i].blendShapesPerVert, asset->surfs[i].vertCount);

				dump.dump_array(asset->surfs[i].blendShapesPerVert->perVertBlendShapeData, (unsigned __int64)asset->surfs[i].blendShapesPerVert->perVertBlendShapeDataSize >> 3);
				dump.dump_array(asset->surfs[i].blendShapesPerVert->surfBlendShapesData, asset->surfs[i].blendShapesPerVert->surfBlendShapeCount);
			}

			if (asset->surfs[i].blendShapesRecalcTangentFrameData)
			{
				dump.dump_single(asset->surfs[i].blendShapesRecalcTangentFrameData);

				dump.dump_raw(asset->surfs[i].blendShapesRecalcTangentFrameData->topologicalData, asset->surfs[i].blendShapesRecalcTangentFrameData->bufferSize);
			}
		}

		dump.dump_single(asset->shared);
		if (asset->shared->data.residentData)
		{
			dump.dump_array(asset->shared->data.residentData, asset->shared->dataSize);
		}

		dump.close();
	}
}
