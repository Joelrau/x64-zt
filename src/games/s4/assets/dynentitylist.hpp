#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class dyn_entity_list : public asset_interface
	{
	private:
		std::string name_;
		DynEntityList* asset_ = nullptr;

	public:
		DynEntityList* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, DynEntityList* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(DynEntityList* asset);
	};
}
