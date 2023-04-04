#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class clip_map : public asset_interface
	{
	private:
		std::string name_;
		clipMap_t* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void parse_info(ClipInfo* info, assetmanager::reader& read, zone_memory* mem);
		void write_info(zone_base* zone, zone_buffer* buf, ClipInfo* data, ClipInfo* dest);
		static void dump_info(ClipInfo* info, assetmanager::dumper& write);

	public:
		clipMap_t* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(clipMap_t* asset);
	};
}