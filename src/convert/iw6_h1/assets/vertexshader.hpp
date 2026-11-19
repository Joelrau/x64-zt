#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace vertexshader
		{
			zonetool::h1::MaterialVertexShader* convert(MaterialVertexShader* asset, utils::memory::allocator& allocator);
			void dump(MaterialVertexShader* asset);
		}
	}
}