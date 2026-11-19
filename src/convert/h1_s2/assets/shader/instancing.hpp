#pragma once

#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	bool declares_instance_matrices(const std::vector<alys::shader::detail::instruction_t>& instructions);
	std::optional<std::uint32_t> find_instance_input(const std::vector<alys::shader::detail::instruction_t>& instructions);
	bool reads_instance_matrices(const alys::shader::detail::operand_t& operand);
	void patch_instance_rows(alys::shader::detail::instruction_t& instruction, const std::uint32_t instance_input, const bool lit_rows,
		std::unordered_set<std::uint32_t>& instance_rows);
}
