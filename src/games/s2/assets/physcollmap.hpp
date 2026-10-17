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

	struct phys_world_layout
	{
		template <typename V>
		static void visit(const V& w);
	};

	class phys_worldmap : public field_layout_asset<PhysWorld, ASSET_TYPE_PHYSWORLDMAP, phys_world_layout>
	{
	};
}
