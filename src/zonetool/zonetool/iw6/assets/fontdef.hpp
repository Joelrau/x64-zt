#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
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

		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(Font_s* asset);
	};
}