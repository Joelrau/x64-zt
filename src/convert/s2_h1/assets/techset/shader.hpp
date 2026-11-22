#pragma once

#include "program_analysis.hpp"
#include "light_array.hpp"
#include "sun_shadow.hpp"

namespace zonetool::s2::converter::h1::techset
{
	struct input_redirect
	{
		std::uint32_t texcoord;
		std::string target;
		std::uint32_t target_index;
		std::uint32_t component;
		bool absolute;
	};

	struct converted_program
	{
		std::string data;
		std::set<std::uint32_t> constant_buffers;
		resource_usage usage;
		std::size_t light_loops;
		bool light_array;
		std::optional<light_array_shadow_slots> light_array_shadows;
		std::optional<sun_shadow_slots> sun_shadow;
		std::map<std::uint32_t, std::uint32_t> sampler_moves;
	};

	struct constant_move
	{
		std::uint32_t buffer;
		std::uint32_t from;
		std::uint32_t to;
	};

	struct sun_direction
	{
		std::uint32_t buffer;
		std::uint32_t row;
		float w;
	};

	struct atmos_fog_rows
	{
		std::uint32_t buffer;
		std::array<std::optional<std::uint32_t>, 8> rows;
		std::optional<std::uint32_t> lut_texture;
	};

	converted_program convert_program(const unsigned char* program, unsigned int size, const std::vector<input_redirect>& redirects,
		const std::vector<constant_move>& moves, const std::optional<sun_shadow_request>& sun_shadow, const std::set<std::uint32_t>& srgb_textures,
		const std::optional<sun_direction>& sun, const std::optional<light_array_request>& lights,
		const std::set<std::uint32_t>& moved_samplers, const std::set<std::uint32_t>& reserved_samplers, const std::optional<atmos_fog_rows>& fog);
}
