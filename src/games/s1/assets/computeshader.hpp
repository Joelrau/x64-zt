#pragma once
#include "../s1.hpp"

namespace zonetool::s1
{
	REGISTER_TEMPLATED_ASSET_CLASS(compute_shader, shader, ComputeShader, ASSET_TYPE_COMPUTESHADER, shader_type::computeshader);
}