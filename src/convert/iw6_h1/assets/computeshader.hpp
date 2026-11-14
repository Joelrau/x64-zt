#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace computeshader
		{
			zonetool::h1::ComputeShader* convert(ComputeShader* asset, utils::memory::allocator& allocator);
			void dump(ComputeShader* asset);
		}
	}
}