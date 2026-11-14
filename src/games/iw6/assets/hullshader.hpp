#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	REGISTER_TEMPLATED_ASSET_CLASS(hull_shader, shader, MaterialHullShader, ASSET_TYPE_HULLSHADER, shader_type::hullshader, false);
}