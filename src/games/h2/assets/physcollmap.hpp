#pragma once
#include "../h2.hpp"

namespace zonetool::h2
{
	class phys_collmap : public asset_interface
	{
	private:
		std::string name_;
		PhysCollmap* asset_ = nullptr;

	public:
		PhysCollmap* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read_polytope(zone_reader& reader, dmPolytopeData* polytope);
		static void read(zone_reader& reader, PhysCollmap* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysCollmap* asset);
	};
}
