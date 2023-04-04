#pragma once

namespace zonetool::iw6
{
	class asset_interface
	{
	public:
		virtual void init(const std::string& name, zone_memory* mem)
		{
		}

		virtual void init(void* asset, zone_memory* mem)
		{
		}

		virtual void prepare(zone_buffer* buf, zone_memory* mem)
		{
		}

		virtual void* pointer() { return nullptr; }
		virtual bool referenced() { return false; }
		virtual std::string name() { return ""; }
		virtual std::int32_t type() { return -1; }

		virtual void write(zone_base* zone, zone_buffer* buffer)
		{
		}

		virtual void load_depending(zone_base* zone)
		{
		}
	};
}
