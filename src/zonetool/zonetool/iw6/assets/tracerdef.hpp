#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class tracer_def : public asset_interface
	{
	private:
		std::string name_;
		TracerDef* asset_ = nullptr;

	public:
		TracerDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(TracerDef* asset);
	};
}