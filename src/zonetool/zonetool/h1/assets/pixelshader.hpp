#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	REGISTER_TEMPLATED_ASSET_CLASS(pixel_shader, shader, MaterialPixelShader, ASSET_TYPE_PIXELSHADER, shader_type::pixelshader);
}