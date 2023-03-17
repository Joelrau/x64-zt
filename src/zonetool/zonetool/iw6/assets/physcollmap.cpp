#include "std_include.hpp"
#include "physcollmap.hpp"

namespace zonetool::iw6
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

	void write_cbrush_wrapper(IZone* zone, ZoneBuffer* buf, BrushWrapper* brush, cbrush_t* data)
	{
		auto* dest = data;

		if (data->sides)
		{
			auto* destsides = buf->write(data->sides, data->numsides);

			for (auto i = 0; i < data->numsides; i++)
			{
				buf->align(3);
				buf->write(data->sides[i].plane);
				ZoneBuffer::clear_pointer(&destsides[i].plane);
			}

			ZoneBuffer::clear_pointer(&dest->sides);
		}
		if (data->baseAdjacentSide)
		{
			buf->align(0);
			buf->write(data->baseAdjacentSide, brush->totalEdgeCount);
			ZoneBuffer::clear_pointer(&dest->baseAdjacentSide);
		}
	}

	void write_brush_wrapper(IZone* zone, ZoneBuffer* buf, BrushWrapper* data)
	{
		auto* dest = data;
		dest = buf->write(data);

		write_cbrush_wrapper(zone, buf, dest, &dest->brush);

		if (data->planes)
		{
			buf->align(3);
			buf->write(data->planes, data->brush.numsides);
			ZoneBuffer::clear_pointer(&dest->planes);
		}
	}

	void write_phys_geom_info(IZone* zone, ZoneBuffer* buf, PhysGeomInfo* data)
	{
		auto* dest = data;

		if (data->brushWrapper)
		{
			buf->align(3);
			write_brush_wrapper(zone, buf, dest->brushWrapper);
		}
	}

	void IPhysCollmap::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->geoms)
		{
			buf->align(3);
			auto* geoms = buf->write(data->geoms, data->count);

			for (auto i = 0u; i < data->count; i++)
			{
				write_phys_geom_info(zone, buf, &geoms[i]);
			}
		}

		buf->pop_stream();
	}

	void IPhysCollmap::dump(PhysCollmap* asset)
	{
	}
}