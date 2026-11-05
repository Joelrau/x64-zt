#pragma once

#include "common.hpp"

namespace zonetool::h1::converter::s2::techset
{
	struct material_rows
	{
		shader::constant_rows pixel;
		shader::constant_rows vertex;
	};

	struct pass_args
	{
		std::vector<MaterialShaderArgument> per_prim;
		std::vector<MaterialShaderArgument> per_obj;
		std::vector<MaterialShaderArgument> stable;
	};

	void relocate_texture_args(std::vector<zonetool::s2::MaterialShaderArgument>& args, const std::map<std::uint32_t, std::uint32_t>& slots);
	shader::primary_lights take_light_consts(std::vector<MaterialShaderArgument>& per_prim, std::vector<MaterialShaderArgument>& per_obj,
		std::vector<MaterialShaderArgument>& stable);
	pass_args split_args(const MaterialPass& pass);
	void add_light_tile_args(const shader::primary_lights& lights, std::vector<zonetool::s2::MaterialShaderArgument>& stable);
	void convert_pass(const MaterialPass& pass, const light_grid grid, const std::int32_t slot, const bool sprite, zonetool::s2::MaterialPass& result,
		const material_rows* light_pass_rows = nullptr, const bool lit_instances = false);
	zonetool::s2::MaterialTechnique* convert_technique(const MaterialTechnique* technique, const light_grid grid, const std::int32_t slot, const bool sprite);
}
