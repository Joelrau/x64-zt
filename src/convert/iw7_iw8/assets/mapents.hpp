#pragma once

#include <iw8/assets/mapents.hpp>

#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::iw7::converter::iw8::mapents
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

	struct createfx_oneshot
	{
		std::string fx;
		std::string origin;
		std::string angles;
		std::string delay;
	};

	void convert_triggers(const MapTriggers& source, zonetool::iw8::MapTriggers& converted, utils::memory::allocator& allocator);

	void dump(MapEnts* asset);
}
