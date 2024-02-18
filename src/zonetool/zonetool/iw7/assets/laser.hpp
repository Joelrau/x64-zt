#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class laser : public asset_interface
	{
	private:
		std::string name_;
		LaserDef* asset_ = nullptr;

	public:
		LaserDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(LaserDef* asset);
	};
}