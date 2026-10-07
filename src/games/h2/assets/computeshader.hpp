#pragma once
#include "../h2.hpp"

namespace zonetool::h2
{
	REGISTER_TEMPLATED_ASSET_CLASS(compute_shader, shader, ComputeShader, ASSET_TYPE_COMPUTESHADER, shader_type::computeshader);
}