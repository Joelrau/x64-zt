#include "std_include.hpp"
#include "physcollmap.hpp"

namespace zonetool::h1
{
	void IPhysCollmap::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name().data()).physCollmap;
	}

	void IPhysCollmap::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IPhysCollmap::load_depending(IZone* zone)
	{
	}

	std::string IPhysCollmap::name()
	{
		return this->name_;
	}

	std::int32_t IPhysCollmap::type()
	{
		return ASSET_TYPE_PHYSCOLLMAP;
	}

	void IPhysCollmap::write(IZone* zone, ZoneBuffer* buf)
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
						buf->write(data_poly_data->vec4_array0, data_poly_data->count0);
						ZoneBuffer::clear_pointer(&dest_poly_data->vec4_array0);
					}

					if (data_poly_data->vec4_array1)
					{
						buf->align(15);
						buf->write(data_poly_data->vec4_array1, data_poly_data->count1);
						ZoneBuffer::clear_pointer(&dest_poly_data->vec4_array1);
					}

					if (data_poly_data->edges)
					{
						buf->align(3);
						buf->write(data_poly_data->edges, data_poly_data->count2);
						ZoneBuffer::clear_pointer(&dest_poly_data->edges);
					}

					if (data_poly_data->uint8_array0)
					{
						buf->align(0);
						buf->write(data_poly_data->uint8_array0, data_poly_data->count1);
						ZoneBuffer::clear_pointer(&dest_poly_data->uint8_array0);
					}

					if (data_poly_data->uint16_array0)
					{
						buf->align(1);
						buf->write(data_poly_data->uint16_array0, data_poly_data->count1);
						ZoneBuffer::clear_pointer(&dest_poly_data->uint16_array0);
					}

					if (data_poly_data->uint16_array1)
					{
						buf->align(1);
						buf->write(data_poly_data->uint16_array1, data_poly_data->count0);
						ZoneBuffer::clear_pointer(&dest_poly_data->uint16_array1);
					}

					ZoneBuffer::clear_pointer(&dest_geoms[i].data);
				}
			}

			ZoneBuffer::clear_pointer(&dest->geoms);
		}

		buf->pop_stream();
	}

	void IPhysCollmap::dump(PhysCollmap* asset)
	{
	}
}