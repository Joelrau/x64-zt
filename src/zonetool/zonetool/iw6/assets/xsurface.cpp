#include "std_include.hpp"
#include "xsurface.hpp"

namespace zonetool::iw6
{
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

			asset->surfs[i].blendVerts = read.read_raw<XBlendInfo>();
			asset->surfs[i].blendVertsTable = read.read_raw<BlendVertsUnknown>();

			asset->surfs[i].lmapUnwrap = read.read_raw<float[2]>();

			asset->surfs[i].tensionData = read.read_raw<float>();
			asset->surfs[i].tensionAccumTable = read.read_raw<unsigned short>();
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
			buf->write_stream(data->lmapUnwrap, 8, data->vertCount);
			zone_buffer::clear_pointer(&dest->lmapUnwrap);
		}

		if (data->subdiv || data->subdivLevelCount)
		{
			dest->subdiv = nullptr;
			dest->subdivLevelCount = 0;
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

			dump.dump_raw(asset->surfs[i].blendVerts, 2 * (asset->surfs[i].blendVertCounts[0]
				+ 3 * asset->surfs[i].blendVertCounts[1]
				+ 5 * asset->surfs[i].blendVertCounts[2]
				+ 7 * asset->surfs[i].blendVertCounts[3]
				+ 9 * asset->surfs[i].blendVertCounts[4]
				+ 11 * asset->surfs[i].blendVertCounts[5]
				+ 13 * asset->surfs[i].blendVertCounts[6]
				+ 15 * asset->surfs[i].blendVertCounts[7]));
			dump.dump_raw(asset->surfs[i].blendVertsTable, 32 * asset->surfs[i].vertCount);

			dump.dump_raw(asset->surfs[i].lmapUnwrap, 8 * asset->surfs[i].vertCount);

			dump.dump_raw(asset->surfs[i].tensionData, 4 * (asset->surfs[i].blendVertCounts[0]
				+ asset->surfs[i].blendVertCounts[1]
				+ asset->surfs[i].blendVertCounts[2]
				+ asset->surfs[i].blendVertCounts[3]
				+ asset->surfs[i].blendVertCounts[4]
				+ asset->surfs[i].blendVertCounts[5]
				+ asset->surfs[i].blendVertCounts[6]
				+ asset->surfs[i].blendVertCounts[7]));
			dump.dump_raw(asset->surfs[i].tensionAccumTable, 32 * asset->surfs[i].vertCount);
		}

		dump.close();
	}
}