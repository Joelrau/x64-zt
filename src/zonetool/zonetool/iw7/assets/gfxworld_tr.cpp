#include <std_include.hpp>
#include "gfxworld_tr.hpp"

namespace zonetool::iw7
{
	GfxWorldTransientZone* gfx_world_tr::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "transient_zones\\"s + name + ".gfxmap_tr"s;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing gfxmap_tr \"%s\"...", name.data());

		GfxWorldTransientZone* asset = read.read_single<GfxWorldTransientZone>();
		asset->name = read.read_string();
		
		asset->vd.vertices = read.read_array<GfxWorldVertex>();
		asset->vld.data = read.read_array<unsigned char>();

		asset->aabbTreeCounts = read.read_array<GfxCellTreeCount>();
		asset->aabbTrees = read.read_array<GfxCellTree>();
		for (unsigned int i = 0; i < asset->cellCount; i++)
		{
			asset->aabbTrees[i].aabbTree = read.read_array<GfxAabbTree>();
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				asset->aabbTrees[i].aabbTree[j].smodelIndexes = read.read_array<unsigned short>();
			}
		}

		read.close();

		return asset;
	}

	void gfx_world_tr::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;
		this->asset_ = this->parse(name, mem);

		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse gfxmap_tr \"%s\"", name.data());
		}
	}

	void gfx_world_tr::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_world_tr::load_depending(zone_base* zone)
	{
	}

	std::string gfx_world_tr::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world_tr::type()
	{
		return ASSET_TYPE_GFXWORLD_TRANSIENT_ZONE;
	}

	void gfx_world_tr::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name_);
		
		if (data->vd.vertices)
		{
			buf->align(3);
			buf->write(data->vd.vertices, data->vertexCount);
			buf->clear_pointer(&dest->vd.vertices);
		}
		dest->vd.worldVb = nullptr;

		if (data->vld.data)
		{
			buf->align(3);
			buf->write(data->vld.data, data->vertexLayerDataSize);
			buf->clear_pointer(&dest->vld.data);
		}
		dest->vld.layerVb = nullptr;

		if (data->aabbTreeCounts)
		{
			buf->align(3);
			buf->write(data->aabbTreeCounts, data->cellCount);
			buf->clear_pointer(&dest->aabbTreeCounts);
		}

		if (data->aabbTrees && data->aabbTreeCounts)
		{
			buf->align(127);
			auto cell_tree = buf->write(data->aabbTrees, data->cellCount);

			for (unsigned int i = 0; i < data->cellCount; i++)
			{
				if (data->aabbTrees[i].aabbTree)
				{
					buf->align(7);

					buf->push_stream(XFILE_BLOCK_TEMP);
					buf->align(3);
					buf->write_stream(&data->aabbTreeCounts[i].aabbTreeCount, 4);
					buf->pop_stream();

					auto gfx_aabb_tree = buf->write(data->aabbTrees[i].aabbTree,
						data->aabbTreeCounts[i].aabbTreeCount);

					for (int i2 = 0; i2 < data->aabbTreeCounts[i].aabbTreeCount; i2++)
					{
						if (data->aabbTrees[i].aabbTree[i2].smodelIndexes)
						{
							gfx_aabb_tree[i2].smodelIndexes = buf->write_s(
								1, data->aabbTrees[i].aabbTree[i2].smodelIndexes,
								data->aabbTrees[i].aabbTree[i2].smodelIndexCount);
						}
					}

					buf->clear_pointer(&cell_tree[i].aabbTree);
				}
			}

			buf->clear_pointer(&dest->aabbTrees);
		}

		buf->pop_stream();
	}

	void gfx_world_tr::dump(GfxWorldTransientZone* asset)
	{
		const auto path = "transient_zones\\"s + asset->name + ".gfxmap_tr"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		
		write.dump_array(asset->vd.vertices, asset->vertexCount);
		write.dump_array(asset->vld.data, asset->vertexLayerDataSize);

		write.dump_array(asset->aabbTreeCounts, asset->cellCount);
		write.dump_array(asset->aabbTrees, asset->cellCount);
		for (unsigned int i = 0; i < asset->cellCount; i++)
		{
			write.dump_array(asset->aabbTrees[i].aabbTree, asset->aabbTreeCounts[i].aabbTreeCount);
			for (int j = 0; j < asset->aabbTreeCounts[i].aabbTreeCount; j++)
			{
				write.dump_array(asset->aabbTrees[i].aabbTree[j].smodelIndexes,
					asset->aabbTrees[i].aabbTree[j].smodelIndexCount);
			}
		}

		write.close();
	}
}