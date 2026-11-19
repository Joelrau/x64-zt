#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class grading_clut : public asset_interface
	{
	private:
		std::string name_;
		GfxGradingClut* asset_ = nullptr;

	public:
		GfxGradingClut* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, GfxGradingClut* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(GfxGradingClut* asset);
	};
}
