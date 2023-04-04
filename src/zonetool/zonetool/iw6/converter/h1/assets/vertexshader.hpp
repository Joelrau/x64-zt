#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexshader
		{
			zonetool::h1::MaterialVertexShader* convert(MaterialVertexShader* asset, zone_memory* mem);
			void dump(MaterialVertexShader* asset, zone_memory* mem);
		}
	}
}