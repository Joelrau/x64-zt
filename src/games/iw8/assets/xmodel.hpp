#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class xmodel : public asset_interface
	{
	private:
		std::string name_;
		XModel* asset_ = nullptr;
		std::unordered_map<scr_string_t*, const char*> script_strings_;

		void parse_script_strings(assetmanager::reader& read, scr_string_t*& strings, std::size_t count);
		void prepare_script_strings(zone_buffer* buf, scr_string_t* strings, std::size_t count);

	public:
		XModel* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, XModel* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(XModel* asset);
	};
}
