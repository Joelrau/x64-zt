#pragma once

namespace zonetool::s2
{
	class phys_collmap : public asset_base<PhysCollmap, ASSET_TYPE_PHYSCOLLMAP>
	{
	public:
		static void read_polytope(zone_reader& reader, dmPolytopeData* data);
		static void read(zone_reader& reader, PhysCollmap* asset);

		static void write_polytope(zone_buffer* buf, dmPolytopeData* data, dmPolytopeData* dest);
		static void dump_polytope(assetmanager::dumper& write, dmPolytopeData* data);
		static void parse_polytope(assetmanager::reader& read, dmPolytopeData* data);

		PhysCollmap* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(PhysCollmap* asset);
	};

	class phys_worldmap : public asset_base<PhysWorld, ASSET_TYPE_PHYSWORLDMAP>
	{
	public:
		static void read(zone_reader& reader, PhysWorld* asset);
		PhysWorld* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(PhysWorld* asset);
	};
}
