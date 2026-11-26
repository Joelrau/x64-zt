#pragma once

#include <xsk/gsc/engine/iw8.hpp>

namespace zonetool::s4::converter::iw8::scriptfile
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

	void dump(ScriptFile* asset);
}
