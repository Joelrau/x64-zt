#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace vertexdecl
		{
			zonetool::s1::MaterialVertexDeclaration* convert(MaterialVertexDeclaration* asset, utils::memory::allocator& allocator);
			void dump(MaterialVertexDeclaration* asset);
		}
	}
}
