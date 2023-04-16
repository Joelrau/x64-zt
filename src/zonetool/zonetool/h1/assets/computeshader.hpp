#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	REGISTER_TEMPLATED_ASSET_CLASS(compute_shader, shader, ComputeShader, ASSET_TYPE_COMPUTESHADER, shader_type::computeshader);
}