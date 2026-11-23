#pragma once

#include "shader.hpp"

namespace zonetool::s2::converter::h1::techset
{
	struct converted_decl
	{
		zonetool::h1::MaterialVertexDeclaration* decl;
		std::vector<input_redirect> redirects;
	};

	const converted_decl& convert_decl(const MaterialVertexDeclaration* decl, utils::memory::allocator& allocator);
	void clear_decls();
}
