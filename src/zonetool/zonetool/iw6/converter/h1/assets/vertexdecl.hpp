#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexdecl
		{
			zonetool::h1::MaterialVertexDeclaration* convert(MaterialVertexDeclaration* asset, zone_memory* mem);
			void dump(MaterialVertexDeclaration* asset, zone_memory* mem);
		}
	}
}