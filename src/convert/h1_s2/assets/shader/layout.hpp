#pragma once

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto h1_constant_buffer_count = 5u;
	constexpr auto h1_instance_matrix_buffer = 6u;
	constexpr auto s2_instance_matrix_buffer = 7u;
	constexpr auto h1_instance_row_shift = 2u;
	constexpr auto s2_instance_row_shift = 3u;
	constexpr auto h1_model_lighting_buffer = 7u;
	constexpr auto h1_reactive_motion_buffer = 9u;

	constexpr auto s2_lit_instance_rows = 9u;
	constexpr auto s2_lit_instance_lighting_row = 8u;
}
