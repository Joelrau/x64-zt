#pragma once

namespace zonetool::h1::converter::s2::bone_rig
{
	struct skeleton;
}

namespace zonetool::h1::converter::s2::xanim_retarget
{
	bool retarget(zonetool::s2::XAnimParts* parts, std::optional<zone_memory>& memory);
	const bone_rig::skeleton* s2_arm_rig();
	void clear();
}
