#pragma once

namespace zonetool::h1::converter::s2::techset
{
	std::vector<std::int32_t> slot_candidates(const std::int32_t slot);
	std::int32_t find_group_technique(const MaterialTechniqueSet* techset, const std::int32_t group, const std::int32_t type);
}
