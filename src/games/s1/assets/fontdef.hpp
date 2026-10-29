#pragma once
#include "../s1.hpp"

namespace zonetool::s1
{
	class font_def : public asset_interface
	{
	private:
		std::string name_;
		Font_s* asset_ = nullptr;

	public:
		Font_s* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, Font_s* asset);
		void write(zone_base* zone, zone_buffer* buf) override;

		static void dump(Font_s* asset);
	};
}
