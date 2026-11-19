#pragma once

#include "../shader/shader.hpp"

namespace zonetool::h1::converter::s2::techset
{
	using shader::light_grid;
	using shader::model_lighting;
	using shader::program_patch;
	using shader::stable_literal_row;
	using shader::sun_shadow_lookup;

	extern utils::memory::allocator native_allocator;

	constexpr auto constant_row_size = 16u;
	constexpr auto h1_instance_probe_ambient_flag = 0x400u;
}
