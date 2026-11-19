#pragma once

namespace zonetool::h1::converter::s2::xanim_dive
{
	std::optional<std::array<zonetool::s2::XAnimParts*, 2>> build(const zonetool::s2::XAnimParts* idle, const std::string& donor,
		const std::array<std::string, 2>& names, std::optional<zone_memory>& memory);
	void clear();
}
