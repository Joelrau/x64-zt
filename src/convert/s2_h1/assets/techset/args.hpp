#pragma once

namespace zonetool::s2::converter::h1::techset
{
	struct row_move
	{
		std::uint8_t stages;
		std::uint32_t buffer;
		std::uint32_t from;
		std::uint32_t to;
	};

	struct converted_args
	{
		std::vector<zonetool::h1::MaterialShaderArgument> per_prim;
		std::vector<zonetool::h1::MaterialShaderArgument> per_obj;
		std::vector<zonetool::h1::MaterialShaderArgument> stable;
		std::vector<row_move> moves;
		unsigned short stable_size;
	};

	converted_args convert_args(const MaterialPass& pass, std::uint32_t reserved_stable_rows, bool sun_light_array, utils::memory::allocator& allocator);
	std::uint32_t convert_custom_buffer_flags(std::uint64_t flags);
}
