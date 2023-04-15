#include "std_include.hpp"
#include "physcollmap.hpp"

namespace zonetool::h1
{
	void phys_collmap::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).physCollmap;
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

		buf->push_stream(3);

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

					if (data_poly_data->vec4_array0)
					{
						buf->align(15);
						buf->write(data_poly_data->vec4_array0, data_poly_data->m_vertexCount);
						zone_buffer::clear_pointer(&dest_poly_data->vec4_array0);
					}

					if (data_poly_data->vec4_array1)
					{
						buf->align(15);
						buf->write(data_poly_data->vec4_array1, data_poly_data->m_faceCount);
						zone_buffer::clear_pointer(&dest_poly_data->vec4_array1);
					}

					if (data_poly_data->m_aSubEdges)
					{
						buf->align(3);
						buf->write(data_poly_data->m_aSubEdges, data_poly_data->m_subEdgeCount);
						zone_buffer::clear_pointer(&dest_poly_data->m_aSubEdges);
					}

					if (data_poly_data->m_aFaceSubEdges)
					{
						buf->align(0);
						buf->write(data_poly_data->m_aFaceSubEdges, data_poly_data->m_faceCount);
						zone_buffer::clear_pointer(&dest_poly_data->m_aFaceSubEdges);
					}

					if (data_poly_data->uint16_array0)
					{
						buf->align(1);
						buf->write(data_poly_data->uint16_array0, data_poly_data->m_faceCount);
						zone_buffer::clear_pointer(&dest_poly_data->uint16_array0);
					}

					if (data_poly_data->uint16_array1)
					{
						buf->align(1);
						buf->write(data_poly_data->uint16_array1, data_poly_data->m_vertexCount);
						zone_buffer::clear_pointer(&dest_poly_data->uint16_array1);
					}

					zone_buffer::clear_pointer(&dest_geoms[i].data);
				}
			}

			zone_buffer::clear_pointer(&dest->geoms);
		}

		buf->pop_stream();
	}

	void phys_collmap::dump(PhysCollmap* asset)
	{
	}
}