#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	class colorization_set : public asset_interface
	{
	private:
		std::string name_;
		ColorizationSet* asset_ = nullptr;

	public:
		ColorizationSet* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void read(zone_reader& reader, ColorizationSet* asset);
		static void dump(ColorizationSet* asset);
	};
}