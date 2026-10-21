#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexdecl
		{
			zonetool::h1::MaterialVertexDeclaration* convert(MaterialVertexDeclaration* asset, utils::memory::allocator& allocator);
			void dump(MaterialVertexDeclaration* asset);
		}
	}
}