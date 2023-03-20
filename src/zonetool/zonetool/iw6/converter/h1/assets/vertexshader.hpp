#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexshader
		{
			zonetool::h1::MaterialVertexShader* convert(MaterialVertexShader* asset, ZoneMemory* mem);
			void dump(MaterialVertexShader* asset, ZoneMemory* mem);
		}
	}
}