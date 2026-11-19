#pragma once

#include "common.hpp"

namespace zonetool::h1::converter::s2::techset
{
	extern const std::unordered_map<std::int32_t, std::int32_t> code_consts;
	extern const std::unordered_map<std::int32_t, std::array<float, 4>> code_literals;

	constexpr std::array<zonetool::s2::MaterialShaderArgumentType, MTL_ARG_COUNT> argument_types
	{
		zonetool::s2::MTL_ARG_CODE_CONST,
		zonetool::s2::MTL_ARG_CODE_TEXTURE,
		zonetool::s2::MTL_ARG_CODE_SAMPLER,
		zonetool::s2::MTL_ARG_MATERIAL_CONST,
		zonetool::s2::MTL_ARG_LITERAL_CONST,
		zonetool::s2::MTL_ARG_MATERIAL_TEXTURE,
		zonetool::s2::MTL_ARG_MATERIAL_SAMPLER,
	};

	float* allocate_literal(const float* value);
	zonetool::s2::MaterialShaderArgument make_arg(const zonetool::s2::MaterialShaderArgumentType type, const unsigned char stage, const unsigned short dest);
	void check_group_size(const std::vector<zonetool::s2::MaterialShaderArgument>& args);
	unsigned short arg_size(const std::uint32_t size, const std::uint32_t rows);
	std::vector<zonetool::s2::MaterialShaderArgument> convert_args(const MaterialShaderArgument* args, const std::size_t count, const bool literals_allowed,
		const bool model_lighting);
	std::uint64_t convert_custom_buffer_flags(const std::uint32_t flags, const std::optional<model_lighting>& lighting, const light_grid grid);
}
