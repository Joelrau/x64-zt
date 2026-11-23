#pragma once

#include <formats/dxbc.hpp>

namespace zonetool::s2::converter::h1::techset
{
	using instruction_list = std::vector<alys::shader::detail::instruction_t>;

	struct resource_usage
	{
		std::map<std::uint32_t, std::set<std::uint32_t>> rows;
		std::set<std::uint32_t> whole_buffers;
		std::set<std::uint32_t> textures;
		std::set<std::uint32_t> samplers;
	};

	std::size_t remove_light_loops(instruction_list& instructions);
	void remove_dead_code(instruction_list& instructions);
	resource_usage collect_usage(const instruction_list& instructions);
}
