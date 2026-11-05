#pragma once

#include "../techset.hpp"

namespace zonetool::h1::converter::s2::techset
{
	struct velocity_arg
	{
		unsigned short dest;
		unsigned short code_const;
		unsigned char row_count;
	};

	struct velocity_technique
	{
		std::int32_t slot;
		const char* name;
		const char* vertex_shader;
		const char* pixel_shader;
		bool hidir;
		bool skinned;
		std::vector<velocity_arg> per_prim_args;
	};

	bool use_stock_velocity_technique(const MaterialTechniqueSet* techset, const std::int32_t slot, native_techset& result);
}
