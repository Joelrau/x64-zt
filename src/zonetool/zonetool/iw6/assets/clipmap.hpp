#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class IClipMap : public IAsset
	{
	private:
		std::string name_;
		clipMap_t* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void parse_info(ClipInfo* info, assetmanager::reader& read, ZoneMemory* mem);
		void write_info(IZone* zone, ZoneBuffer* buf, ClipInfo* data, ClipInfo* dest);
		static void dump_info(ClipInfo* info, assetmanager::dumper& write);

	public:
		clipMap_t* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(clipMap_t* asset);
	};
}