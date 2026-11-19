#pragma once

namespace zonetool::s2
{
	class mayhem : public asset_base<Mayhem, ASSET_TYPE_MAYHEM>
	{
	public:
		static void read(zone_reader& reader, Mayhem* asset);
		Mayhem* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(Mayhem* asset);
	};
}
