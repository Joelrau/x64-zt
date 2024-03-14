#include <std_include.hpp>
#include "gfxworld.hpp"

namespace zonetool::iw7
{
	GfxWorld* gfx_world::parse(const std::string& name, zone_memory* mem)
	{
		// TODO:
	}

	void gfx_world::init(const std::string& name, zone_memory* mem)
	{
		// TODO:
	}

	void gfx_world::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_world::load_depending(zone_base* zone)
	{
	}

	std::string gfx_world::name()
	{
		return this->name_;
	}

	std::int32_t gfx_world::type()
	{
		return ASSET_TYPE_GFXWORLD;
	}

	void gfx_world::write(zone_base* zone, zone_buffer* buf)
	{
		// TODO:
	}

	void gfx_world::dump(GfxWorld* asset)
	{
		// TODO:
	}
}
