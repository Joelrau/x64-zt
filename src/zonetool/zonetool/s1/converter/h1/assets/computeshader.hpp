#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace computeshader
		{
			zonetool::h1::ComputeShader* convert(ComputeShader* asset, ZoneMemory* mem);
			void dump(ComputeShader* asset, ZoneMemory* mem);
		}
	}
}