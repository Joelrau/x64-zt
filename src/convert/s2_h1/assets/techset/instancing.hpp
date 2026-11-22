#pragma once

#include <formats/dxbc.hpp>

namespace zonetool::s2::converter::h1::techset
{
	constexpr auto s2_instance_matrix_buffer = 7u;

	void convert_instancing(std::vector<alys::shader::detail::instruction_t>& instructions);
}
