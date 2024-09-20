#include "std_include.hpp"
#include "physcollmap.hpp"

namespace zonetool::h1
{
	PhysCollmap* phys_collmap::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "physcollmap\\"s + name + ".pc";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing physcollmap \"%s\"...", name.data());

		auto* asset = read.read_single<PhysCollmap>();
		asset->name = read.read_string();
		asset->geoms = read.read_array<PhysGeomInfo>();

		for (auto i = 0u; i < asset->count; i++)
		{
			asset->geoms[i].data = read.read_single<dmPolytopeData>();
			if (asset->geoms[i].data)
			{
				asset->geoms[i].data->m_aVertices = read.read_array<dmFloat4>();
				asset->geoms[i].data->m_aPlanes = read.read_array<dmPlane>();
				asset->geoms[i].data->m_surfaceTypes = read.read_array<dm_uint16>();
				asset->geoms[i].data->m_vertexMaterials = read.read_array<dm_uint16>();
				asset->geoms[i].data->m_aSubEdges = read.read_array<dmSubEdge>();
				asset->geoms[i].data->m_aFaceSubEdges = read.read_array<dm_uint8>();
			}
		}

		read.close();

		return asset;
	}

	void phys_collmap::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physCollmap;
		}
	}

	void phys_collmap::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void phys_collmap::load_depending(zone_base* zone)
	{
	}

	std::string phys_collmap::name()
	{
		return this->name_;
	}

	std::int32_t phys_collmap::type()
	{
		return ASSET_TYPE_PHYSCOLLMAP;
	}

	void phys_collmap::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->geoms)
		{
			buf->align(3);
			auto* data_geoms = data->geoms;
			auto* dest_geoms = buf->write(data_geoms, data->count);
			for (unsigned int i = 0; i < data->count; i++)
			{
				if (data_geoms[i].data)
				{
					buf->align(3);
					auto* data_poly_data = data_geoms[i].data;
					auto* dest_poly_data = buf->write(data_poly_data);

					if (data_poly_data->m_aVertices)
					{
						buf->align(15);
						buf->write(data_poly_data->m_aVertices, data_poly_data->m_vertexCount);
						buf->clear_pointer(&dest_poly_data->m_aVertices);
					}

					if (data_poly_data->m_aPlanes)
					{
						buf->align(15);
						buf->write(data_poly_data->m_aPlanes, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_aPlanes);
					}

					if (data_poly_data->m_aSubEdges)
					{
						buf->align(3);
						buf->write(data_poly_data->m_aSubEdges, data_poly_data->m_subEdgeCount);
						buf->clear_pointer(&dest_poly_data->m_aSubEdges);
					}

					if (data_poly_data->m_aFaceSubEdges)
					{
						buf->align(0);
						buf->write(data_poly_data->m_aFaceSubEdges, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_aFaceSubEdges);
					}

					if (data_poly_data->m_surfaceTypes)
					{
						buf->align(1);
						buf->write(data_poly_data->m_surfaceTypes, data_poly_data->m_faceCount);
						buf->clear_pointer(&dest_poly_data->m_surfaceTypes);
					}

					if (data_poly_data->m_vertexMaterials)
					{
						buf->align(1);
						buf->write(data_poly_data->m_vertexMaterials, data_poly_data->m_vertexCount);
						buf->clear_pointer(&dest_poly_data->m_vertexMaterials);
					}

					buf->clear_pointer(&dest_geoms[i].data);
				}
			}

			buf->clear_pointer(&dest->geoms);
		}

		buf->pop_stream();
	}

	void phys_collmap::dump(PhysCollmap* asset)
	{
		const auto path = "physcollmap\\"s + asset->name + ".pc";

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->geoms, asset->count);

		for (auto i = 0u; i < asset->count; i++)
		{
			const auto data = asset->geoms[i].data;
			write.dump_single(data);
			if (data)
			{
				write.dump_array(data->m_aVertices, data->m_vertexCount);
				write.dump_array(data->m_aPlanes, data->m_faceCount);
				write.dump_array(data->m_surfaceTypes, data->m_faceCount);
				write.dump_array(data->m_vertexMaterials, data->m_vertexCount);
				write.dump_array(data->m_aSubEdges, data->m_subEdgeCount);
				write.dump_array(data->m_aFaceSubEdges, data->m_faceCount);
			}
		}
	}
}
