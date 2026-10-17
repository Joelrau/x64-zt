#pragma once

namespace zonetool::s2
{
	class gfx_world : public asset_base<GfxWorld, ASSET_TYPE_GFXWORLD>
	{
	public:
		static void read(zone_reader& reader, GfxWorld* asset);
		GfxWorld* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(GfxWorld* asset);
	};
}
