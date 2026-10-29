#pragma once

namespace zonetool::s2
{
	class clip_map : public asset_base<clipMap_t, ASSET_TYPE_CLIPMAP>
	{
	public:
		static void read(zone_reader& reader, clipMap_t* asset);
		clipMap_t* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(clipMap_t* asset);

		static void read_info(zone_reader& reader, ClipInfo* info);
		static void parse_info(assetmanager::reader& read, zone_memory* mem, ClipInfo* info);
		static void write_info(zone_base* zone, zone_buffer* buf, ClipInfo* info, ClipInfo* dest);
		static void dump_info(assetmanager::dumper& write, ClipInfo* info);
		static void add_info_dependencies(zone_base* zone, ClipInfo* info);
	};
}
