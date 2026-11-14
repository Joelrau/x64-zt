#pragma once

#include "dxbc.hpp"

namespace zonetool::h1::converter::s2::shader
{
	constexpr auto light_grid_index_slot = 5u;
	constexpr auto light_grid_transform_slot = 6u;

	std::map<std::uint32_t, std::uint32_t> relocate_textures(const std::vector<alys::shader::detail::instruction_t>& instructions, const std::uint32_t lighting_texture);

	struct vertex_lighting
	{
		std::size_t coords_write;
		std::size_t position_write;
		std::uint32_t output;
		std::uint32_t semantic;
	};

	bool reads_model_lighting_row(const alys::shader::detail::instruction_t& instruction, const model_lighting& lighting);
	vertex_lighting find_vertex_lighting(alys::shader::shader_object& shader, const model_lighting& lighting);
	void emit_vertex_lighting(alys::shader::assembler& a, const alys::shader::detail::operand_t& position, const model_lighting& lighting,
		const std::optional<std::uint32_t> instance_input, const std::uint32_t temp);
	void emit_light_grid_lookup(alys::shader::assembler& a, const program_patch& patch, const alys::shader::detail::operand_t& sampler,
		const std::uint32_t input, const std::uint32_t temp, const bool directional);
	void emit_probe_lookup(alys::shader::assembler& a, const alys::shader::detail::instruction_t& sample, const program_patch& patch,
		const std::uint32_t input, const std::uint32_t temp);

	constexpr auto sun_color_temp_count = 5u;

	std::optional<std::uint32_t> sun_color_index(const alys::shader::detail::operand_t& operand);
	bool reads_sun_color(const std::vector<alys::shader::detail::instruction_t>& instructions);
	void emit_probe_sun_color(alys::shader::assembler& a, const program_patch& patch, const std::uint32_t input, const std::uint32_t temp);
	void declare_light_grid(alys::shader::assembler& a, const program_patch& patch);
}
