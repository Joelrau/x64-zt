#pragma once

namespace zonetool::h1::converter::s2::xanim_melee
{
	zonetool::s2::XAnimParts* build(const zonetool::s2::XAnimParts* idle, const std::string& donor, const std::string& name,
		std::optional<zone_memory>& memory);
	void clear();
}
