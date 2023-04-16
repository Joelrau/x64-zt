#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	REGISTER_TEMPLATED_ASSET_CLASS(domain_shader, shader, MaterialDomainShader, ASSET_TYPE_DOMAINSHADER, shader_type::domainshader);
}