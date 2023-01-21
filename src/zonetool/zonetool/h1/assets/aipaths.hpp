#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class IAIPaths : public IAsset
	{
	private:
		std::string name_;
		PathData* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

	public:
		PathData* parse(const std::string& name, ZoneMemory* mem);
		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(PathData* asset);
	};
}