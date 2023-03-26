#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace pixelshader
		{
			zonetool::h1::MaterialPixelShader* convert(MaterialPixelShader* asset, ZoneMemory* mem);
			void dump(MaterialPixelShader* asset, ZoneMemory* mem);
		}
	}
}