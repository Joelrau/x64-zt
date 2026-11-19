#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace pixelshader
		{
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, utils::memory::allocator& allocator);
			void dump(MaterialPixelShader* asset);
		}
	}
}