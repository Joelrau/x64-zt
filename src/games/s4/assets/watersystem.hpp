#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class water_system : public asset_interface
	{
	private:
		std::string name_;
		WaterSystem* asset_ = nullptr;

	public:
		WaterSystem* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, WaterSystem* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WaterSystem* asset);
	};
}
