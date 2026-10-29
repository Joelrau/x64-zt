#pragma once

namespace zonetool::h1::converter::s2::xanim_retarget
{
	bool retarget(zonetool::s2::XAnimParts* parts, std::optional<zone_memory>& memory);
	void clear();
}
