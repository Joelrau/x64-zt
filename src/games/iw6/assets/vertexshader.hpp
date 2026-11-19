#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	REGISTER_TEMPLATED_ASSET_CLASS(vertex_shader, shader, MaterialVertexShader, ASSET_TYPE_VERTEXSHADER, shader_type::vertexshader);
}