#pragma once
#include "../iw6.hpp"

namespace zonetool::iw6
{
	REGISTER_TEMPLATED_ASSET_CLASS(domain_shader, shader, MaterialDomainShader, ASSET_TYPE_DOMAINSHADER, shader_type::domainshader, false);
}