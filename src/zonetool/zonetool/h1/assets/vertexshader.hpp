#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	REGISTER_TEMPLATED_ASSET_CLASS(vertex_shader, shader, MaterialVertexShader, ASSET_TYPE_VERTEXSHADER, shader_type::vertexshader);
}