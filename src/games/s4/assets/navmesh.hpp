#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class nav_mesh : public asset_interface
	{
	private:
		std::string name_;
		NavMeshData* asset_ = nullptr;
		script_string_map script_strings_;

	public:
		NavMeshData* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, NavMeshData* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(NavMeshData* asset);
	};
}
