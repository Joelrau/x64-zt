#pragma once

namespace zonetool::h1
{
	class IAsset
	{
	public:
		virtual void init(const std::string& name, ZoneMemory* mem)
		{
		}

		virtual void init(void* asset, ZoneMemory* mem)
		{
		}

		virtual void prepare(ZoneBuffer* buf, ZoneMemory* mem)
		{
		}

		virtual void* pointer() { return nullptr; }
		virtual bool referenced() { return false; }
		virtual std::string name() { return ""; }
		virtual std::int32_t type() { return -1; }

		virtual void write(IZone* zone, ZoneBuffer* buffer)
		{
		}

		virtual void load_depending(IZone* zone)
		{
		}
	};
}
