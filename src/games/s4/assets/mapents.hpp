#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	template <typename Walk>
	void walk_map_triggers(Walk walk, MapTriggers& triggers)
	{
		if (const auto models = walk.array(triggers.models, triggers.count, 7))
		{
			for (auto i = 0u; i < triggers.count; i++)
			{
				walk.asset(ASSET_TYPE_PHYSICSASSET, models[i].physicsAsset);
			}
		}

		walk.array(triggers.hulls, triggers.hullCount, 3);
		walk.array(triggers.slabs, triggers.slabCount, 3);
		walk.array(triggers.windings, triggers.windingCount, 3);
		walk.array(triggers.windingPoints, triggers.windingPointCount, 3);
	}

	class map_ents : public asset_interface
	{
	private:
		std::string name_;
		MapEnts* asset_ = nullptr;
		script_string_map script_strings_;

	public:
		MapEnts* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, MapEnts* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(MapEnts* asset);
	};
}
