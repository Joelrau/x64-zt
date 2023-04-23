#include "std_include.hpp"
#include "xsurface.hpp"

namespace zonetool::h2
{
	void parse_subdiv(XSurface* surf, assetmanager::reader& reader)
	{
		if (surf->subdiv)
		{
			surf->subdiv = reader.read_single<XSurfaceSubdivInfo>();

			auto* levels = surf->subdiv->levels;
			auto level_count = surf->subdivLevelCount;

			levels = reader.read_array<XSurfaceSubdivLevel>();
			for (unsigned char level_index = 0; level_index < level_count; level_index++)
			{
				auto* level = &levels[level_index];

				level->rigidVertLists = reader.read_array<XSubdivRigidVertList>();
				level->faceIndices = reader.read_array<unsigned short>();
				level->regularPatchIndices = reader.read_array<unsigned short>();
				level->regularPatchFlags = reader.read_array<unsigned int>();
				level->facePoints = reader.read_array<unsigned int>();
				level->edgePoints = reader.read_array<unsigned int>();
				level->vertexPoints = reader.read_array<unsigned int>();
				level->normals = reader.read_array<unsigned int>();
				level->transitionPoints = reader.read_array<unsigned int>();
				level->regularPatchCones = reader.read_array<float>();
			}
		}
	}

	XModelSurfs* xsurface::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "xsurface\\" + name + ".xsurface_export";

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
			if ((asset->surfs[i].flags & 8) != 0)
			{
				asset->surfs[i].verts0.packedMotionVerts0 = read.read_array<GfxPackedMotionVertex>();
			}
			else
			{
				asset->surfs[i].verts0.packedVerts0 = read.read_array<GfxPackedVertex>();
			}

			asset->surfs[i].triIndices = read.read_array<Face>();
			asset->surfs[i].triIndices2 = read.read_array<Face>();

			asset->surfs[i].rigidVertLists = read.read_array<XRigidVertList>();
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

			asset->surfs[i].unknown0 = read.read_raw<UnknownXSurface0>();

			asset->surfs[i].blendVerts = read.read_raw<XBlendInfo>();
			asset->surfs[i].blendVertsTable = read.read_raw<BlendVertsUnknown>();

			asset->surfs[i].lmapUnwrap = read.read_raw<alignVertBufFloat16Vec2_t>();

			asset->surfs[i].tensionData = read.read_raw<alignCompBufFloat_t>();
			asset->surfs[i].tensionAccumTable = read.read_raw<alignCompBufUShort_t>();

			parse_subdiv(&asset->surfs[i], read);

			asset->surfs[i].blendShapes = read.read_array<BlendShape>();
			for (unsigned int bs_index = 0; bs_index < asset->surfs[i].blendShapesCount; bs_index++)
			{
				asset->surfs[i].blendShapes[bs_index].verts = read.read_array<BlendShapeVert>();
			}
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
		dest->vb0 = nullptr;
		dest->vb0View = nullptr;
		dest->indexBuffer = nullptr;
		dest->blendVertsBuffer = nullptr;
		dest->blendVertsView = nullptr;
		dest->vblmapBuffer = nullptr;
		dest->vblmapView = nullptr;
		dest->tensionAccumTableBuffer = nullptr;
		dest->tensionAccumTableView = nullptr;
		dest->tensionDataBuffer = nullptr;
		dest->tensionDataView = nullptr;
		dest->indexBufferView = nullptr;

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

			zone_buffer::clear_pointer(&dest->verts0.verts0);
		}

		if (data->triIndices)
		{
			buf->align(15);
			buf->write(data->triIndices, data->triCount);
			zone_buffer::clear_pointer(&dest->triIndices);
		}

		if (data->triIndices2)
		{
			buf->align(15);
			buf->write(data->triIndices2, data->triCount);
			zone_buffer::clear_pointer(&dest->triIndices2);
		}

		if (data->rigidVertLists)
		{
			buf->align(3);
			dest->rigidVertLists = buf->write(data->rigidVertLists, data->rigidVertListCount);
			for (unsigned char vert = 0; vert < data->rigidVertListCount; vert++)
			{
				if (data->rigidVertLists[vert].collisionTree)
				{
					buf->align(3);
					dest->rigidVertLists[vert].collisionTree = buf->write(data->rigidVertLists[vert].collisionTree);

					if (data->rigidVertLists[vert].collisionTree->nodes)
					{
						buf->align(15);
						dest->rigidVertLists[vert].collisionTree->nodes = buf->write(data->rigidVertLists[vert].collisionTree->nodes, data->rigidVertLists[vert].collisionTree->nodeCount);
					}

					if (data->rigidVertLists[vert].collisionTree->leafs)
					{
						buf->align(1);
						dest->rigidVertLists[vert].collisionTree->leafs = buf->write(data->rigidVertLists[vert].collisionTree->leafs, data->rigidVertLists[vert].collisionTree->leafCount);
					}
					zone_buffer::clear_pointer(&dest->rigidVertLists[vert].collisionTree);
				}
			}
			zone_buffer::clear_pointer(&dest->rigidVertLists);
		}

		if (data->unknown0)
		{
			buf->align(15);
			buf->write_stream(data->unknown0, 16, data->vertCount);
			zone_buffer::clear_pointer(&dest->unknown0);
		}

		if (data->blendVerts)
		{
			buf->align(1);
			buf->write_stream(data->blendVerts, 2, (data->blendVertCounts[0]
				+ 3 * data->blendVertCounts[1]
				+ 5 * data->blendVertCounts[2]
				+ 7 * data->blendVertCounts[3]
				+ 9 * data->blendVertCounts[4]
				+ 11 * data->blendVertCounts[5]
				+ 13 * data->blendVertCounts[6]
				+ 15 * data->blendVertCounts[7]));
			zone_buffer::clear_pointer(&dest->blendVerts);
		}

		if (data->blendVertsTable)
		{
			buf->align(0);
			buf->write_stream(data->blendVertsTable, 32, data->vertCount);
			zone_buffer::clear_pointer(&dest->blendVertsTable);
		}

		if (data->lmapUnwrap)
		{
			buf->align(3);
			buf->write_stream(data->lmapUnwrap, 4, data->vertCount);
			zone_buffer::clear_pointer(&dest->lmapUnwrap);
		}

		if (data->subdiv)
		{
			buf->align(3);
			dest->subdiv = buf->write(data->subdiv);
			if (data->subdiv->levels)
			{
				buf->align(15);
				dest->subdiv->levels = buf->write(data->subdiv->levels, data->subdivLevelCount);
				for (unsigned char level_index = 0; level_index < data->subdivLevelCount; level_index++)
				{
					if (data->subdiv->levels[level_index].rigidVertLists)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].rigidVertLists = buf->write(data->subdiv->levels[level_index].rigidVertLists,
							data->rigidVertListCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].rigidVertLists);
					}

					if (data->subdiv->levels[level_index].faceIndices)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].faceIndices = buf->write(data->subdiv->levels[level_index].faceIndices,
							data->subdiv->levels[level_index].faceCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].faceIndices);
					}

					if (data->subdiv->levels[level_index].regularPatchIndices)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].regularPatchIndices = buf->write(data->subdiv->levels[level_index].regularPatchIndices,
							data->subdiv->levels[level_index].regularPatchCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].regularPatchIndices);
					}

					if (data->subdiv->levels[level_index].regularPatchFlags)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].regularPatchFlags = buf->write(data->subdiv->levels[level_index].regularPatchFlags,
							data->subdiv->levels[level_index].regularPatchCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].regularPatchFlags);
					}

					if (data->subdiv->levels[level_index].facePoints)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].facePoints = buf->write(data->subdiv->levels[level_index].facePoints,
							data->subdiv->levels[level_index].facePointBufferSize & 0xFFFFFFFC);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].facePoints);
					}

					if (data->subdiv->levels[level_index].edgePoints)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].edgePoints = buf->write(data->subdiv->levels[level_index].edgePoints,
							data->subdiv->levels[level_index].edgePointCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].edgePoints);
					}

					if (data->subdiv->levels[level_index].vertexPoints)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].vertexPoints = buf->write(data->subdiv->levels[level_index].vertexPoints,
							data->subdiv->levels[level_index].vertexPointBufferSize & 0xFFFFFFFC);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].vertexPoints);
					}

					if (data->subdiv->levels[level_index].normals)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].normals = buf->write(data->subdiv->levels[level_index].normals,
							data->subdiv->levels[level_index].normalBufferSize & 0xFFFFFFFC);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].normals);
					}

					if (data->subdiv->levels[level_index].transitionPoints)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].transitionPoints = buf->write(data->subdiv->levels[level_index].transitionPoints,
							data->subdiv->levels[level_index].transitionPointCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].transitionPoints);
					}

					if (data->subdiv->levels[level_index].regularPatchCones)
					{
						buf->align(3);
						dest->subdiv->levels[level_index].regularPatchCones = buf->write(data->subdiv->levels[level_index].regularPatchCones,
							data->subdiv->levels[level_index].regularPatchCount);
						zone_buffer::clear_pointer(&dest->subdiv->levels[level_index].regularPatchCones);
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
				zone_buffer::clear_pointer(&dest->subdiv->levels);
			}

			dest->subdiv->cache.subdivCacheBuffer = nullptr;
			dest->subdiv->cache.subdivCacheView = nullptr;

			zone_buffer::clear_pointer(&dest->subdiv);
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

			zone_buffer::clear_pointer(&dest->tensionData);
		}

		if (data->tensionAccumTable)
		{
			buf->align(1);
			buf->write_stream(data->tensionAccumTable, 32, data->vertCount);
			zone_buffer::clear_pointer(&dest->tensionAccumTable);
		}

		if (data->blendShapes)
		{
			buf->align(15);
			dest->blendShapes = buf->write(data->blendShapes, data->blendShapesCount);
			for (unsigned int bs_index = 0; bs_index < data->blendShapesCount; bs_index++)
			{
				if (data->blendShapes[bs_index].verts)
				{
					buf->align(15);
					dest->blendShapes[bs_index].verts = buf->write(data->blendShapes[bs_index].verts, data->blendShapes[bs_index].vertCount);
					zone_buffer::clear_pointer(&dest->blendShapes[bs_index].verts);
				}

				dest->blendShapes[bs_index].blendShapeVertsBuffer = nullptr;
				dest->blendShapes[bs_index].blendShapeVertsView = nullptr;
			}
			zone_buffer::clear_pointer(&dest->blendShapes);
		}
	}

	void xsurface::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->surfs)
		{
			buf->align(3);
			dest->surfs = buf->write(data->surfs, data->numsurfs);
			for (unsigned short surf = 0; surf < data->numsurfs; surf++)
			{
				write_xsurface(zone, buf, &dest->surfs[surf], &data->surfs[surf]);
			}
		}

		buf->pop_stream();
	}

	void dump_subdiv(XSurface* surf, assetmanager::dumper& dumper)
	{
		if (surf->subdiv)
		{
			dumper.dump_single(surf->subdiv);

			auto* levels = surf->subdiv->levels;
			auto level_count = surf->subdivLevelCount;

			dumper.dump_array(levels, level_count);
			for (unsigned char level_index = 0; level_index < level_count; level_index++)
			{
				auto* level = &levels[level_index];

				dumper.dump_array(level->rigidVertLists, surf->rigidVertListCount);
				dumper.dump_array(level->faceIndices, level->faceCount);
				dumper.dump_array(level->regularPatchIndices, level->regularPatchCount);
				dumper.dump_array(level->regularPatchFlags, level->regularPatchCount);
				dumper.dump_array(level->facePoints, level->facePointBufferSize & 0xFFFFFFFC);
				dumper.dump_array(level->edgePoints, level->edgePointCount);
				dumper.dump_array(level->vertexPoints, level->vertexPointBufferSize & 0xFFFFFFFC);
				dumper.dump_array(level->normals, level->normalBufferSize & 0xFFFFFFFC);
				dumper.dump_array(level->transitionPoints, level->transitionPointCount);
				dumper.dump_array(level->regularPatchCones, level->regularPatchCount);
			}
		}
	}

	void xsurface::dump(XModelSurfs* asset)
	{
		const auto path = "xsurface\\"s + asset->name + ".xsurface_export";

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
			if ((asset->surfs[i].flags & 8) != 0)
			{
				dump.dump_array(asset->surfs[i].verts0.packedMotionVerts0, asset->surfs[i].vertCount);
			}
			else
			{
				dump.dump_array(asset->surfs[i].verts0.packedVerts0, asset->surfs[i].vertCount);
			}

			dump.dump_array(asset->surfs[i].triIndices, asset->surfs[i].triCount);
			dump.dump_array(asset->surfs[i].triIndices2, asset->surfs[i].triCount);

			dump.dump_array(asset->surfs[i].rigidVertLists, asset->surfs[i].rigidVertListCount);
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

			dump.dump_raw(asset->surfs[i].unknown0, 16 * asset->surfs[i].vertCount);

			dump.dump_raw(asset->surfs[i].blendVerts, 2 * (asset->surfs[i].blendVertCounts[0]
				+ 3 * asset->surfs[i].blendVertCounts[1]
				+ 5 * asset->surfs[i].blendVertCounts[2]
				+ 7 * asset->surfs[i].blendVertCounts[3]
				+ 9 * asset->surfs[i].blendVertCounts[4]
				+ 11 * asset->surfs[i].blendVertCounts[5]
				+ 13 * asset->surfs[i].blendVertCounts[6]
				+ 15 * asset->surfs[i].blendVertCounts[7]));
			dump.dump_raw(asset->surfs[i].blendVertsTable, 32 * asset->surfs[i].vertCount);

			dump.dump_raw(asset->surfs[i].lmapUnwrap, 4 * asset->surfs[i].vertCount);

			dump.dump_raw(asset->surfs[i].tensionData, 4 * (asset->surfs[i].blendVertCounts[0]
				+ asset->surfs[i].blendVertCounts[1]
				+ asset->surfs[i].blendVertCounts[2]
				+ asset->surfs[i].blendVertCounts[3]
				+ asset->surfs[i].blendVertCounts[4]
				+ asset->surfs[i].blendVertCounts[5]
				+ asset->surfs[i].blendVertCounts[6]
				+ asset->surfs[i].blendVertCounts[7]));
			dump.dump_raw(asset->surfs[i].tensionAccumTable, 32 * asset->surfs[i].vertCount);

			dump_subdiv(&asset->surfs[i], dump);

			dump.dump_array(asset->surfs[i].blendShapes, asset->surfs[i].blendShapesCount);
			for (unsigned int bs_index = 0; bs_index < asset->surfs[i].blendShapesCount; bs_index++)
			{
				dump.dump_array(asset->surfs[i].blendShapes[bs_index].verts, asset->surfs[i].blendShapes[bs_index].vertCount);
			}
		}

		dump.close();
	}
}