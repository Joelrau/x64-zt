#include <std_include.hpp>
#include "physcollmap.hpp"

namespace zonetool::iw6
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

	namespace
	{
		void read_brush_wrapper(zone_reader& reader, BrushWrapper* brush)
		{
			if (const auto sides = reader.read_array(brush->brush.sides, 3, brush->brush.numsides))
			{
				for (auto i = 0; i < brush->brush.numsides; i++)
				{
					reader.read_single(sides[i].plane, 3);
				}
			}

			reader.read_array(brush->brush.baseAdjacentSide, 0, brush->totalEdgeCount);
			reader.read_array(brush->planes, 3, brush->brush.numsides);
		}
	}

	void phys_collmap::read(zone_reader& reader, PhysCollmap* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto geoms = reader.read_array(asset->geoms, 3, asset->count))
		{
			for (auto i = 0u; i < asset->count; i++)
			{
				if (reader.read_single(geoms[i].brushWrapper, 3))
				{
					read_brush_wrapper(reader, geoms[i].brushWrapper);
				}
			}
		}

		reader.pop_stream();
	}

	void write_cbrush_wrapper(zone_base* zone, zone_buffer* buf, BrushWrapper* brush, cbrush_t* data)
	{
		auto* dest = data;

		if (data->sides)
		{
			auto* destsides = buf->write(data->sides, data->numsides);

			for (auto i = 0; i < data->numsides; i++)
			{
				buf->align(3);
				buf->write(data->sides[i].plane);
				buf->clear_pointer(&destsides[i].plane);
			}

			buf->clear_pointer(&dest->sides);
		}
		if (data->baseAdjacentSide)
		{
			buf->align(0);
			buf->write(data->baseAdjacentSide, brush->totalEdgeCount);
			buf->clear_pointer(&dest->baseAdjacentSide);
		}
	}

	void write_brush_wrapper(zone_base* zone, zone_buffer* buf, BrushWrapper* data)
	{
		auto* dest = data;
		dest = buf->write(data);

		write_cbrush_wrapper(zone, buf, dest, &dest->brush);

		if (data->planes)
		{
			buf->align(3);
			buf->write(data->planes, data->brush.numsides);
			buf->clear_pointer(&dest->planes);
		}
	}

	void write_phys_geom_info(zone_base* zone, zone_buffer* buf, PhysGeomInfo* data)
	{
		auto* dest = data;

		if (data->brushWrapper)
		{
			buf->align(3);
			write_brush_wrapper(zone, buf, dest->brushWrapper);
		}
	}

	void phys_collmap::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

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

	void phys_collmap::dump(PhysCollmap* asset)
	{
	}
}