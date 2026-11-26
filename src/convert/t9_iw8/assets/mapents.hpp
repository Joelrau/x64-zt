#pragma once

#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::t9::converter::iw8::mapents
{
	class script_context : public xsk::gsc::iw8::context
	{
	public:
		script_context() : context(xsk::gsc::instance::server)
		{
			const auto features = static_cast<xsk::gsc::feature::values>(this->features_);
			this->features_ = static_cast<xsk::gsc::feature::values>((features & ~xsk::gsc::feature::offs8) | xsk::gsc::feature::offs9); // 1.20 VM shifts call offsets by 9 (VM_Execute_0 0x141328B9D)
		}
	};

	using entity = std::vector<std::pair<std::string, std::string>>;

	struct trigger
	{
		entity keys;
		std::vector<zonetool::iw8::TriggerHull> hulls;
		std::vector<zonetool::iw8::TriggerSlab> slabs;
	};

	std::vector<entity> read_entities(const EntityList* asset);
	std::vector<trigger> convert_triggers(const TriggerList* asset, const std::string& map_name);
	void dump(const std::vector<entity>& entities, const std::vector<trigger>& triggers, const zonetool::iw8::Bounds& world_bounds,
		const std::string& ents_shape_data, unsigned int dyn_entity_count, const std::string& map_name);
}
