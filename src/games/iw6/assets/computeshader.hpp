#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	REGISTER_TEMPLATED_ASSET_CLASS(compute_shader, shader, ComputeShader, ASSET_TYPE_COMPUTESHADER, shader_type::computeshader, false);
}