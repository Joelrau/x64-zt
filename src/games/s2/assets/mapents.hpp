#pragma once

namespace zonetool::s2
{
	class map_ents : public asset_base<MapEnts, ASSET_TYPE_MAP_ENTS>
	{
	public:
		static void read(zone_reader& reader, MapEnts* asset);
		MapEnts* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(MapEnts* asset);

		static void read_triggers(zone_reader& reader, MapTriggers* trigger);
		static void parse_triggers(assetmanager::reader& read, zone_memory* mem, MapTriggers* trigger);
		static void write_triggers(zone_buffer* buf, MapTriggers* trigger, MapTriggers* dest);
		static void dump_triggers(assetmanager::dumper& write, MapTriggers* trigger);
	};

	class addon_map_ents : public asset_base<AddonMapEnts, ASSET_TYPE_ADDON_MAP_ENTS>
	{
	public:
		static void read(zone_reader& reader, AddonMapEnts* asset);
		AddonMapEnts* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(AddonMapEnts* asset);
	};
}
