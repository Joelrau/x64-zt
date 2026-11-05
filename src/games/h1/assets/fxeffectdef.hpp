#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class fx_effect_def : public asset_interface
	{
	private:
		std::string name_;
		FxEffectDef* asset_ = nullptr;

	public:
		FxEffectDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, FxEffectDef* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(FxEffectDef* asset);
	};
}
