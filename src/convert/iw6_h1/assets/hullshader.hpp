#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace hullshader
		{
			zonetool::h1::MaterialHullShader* convert(MaterialHullShader* asset, utils::memory::allocator& allocator);
			void dump(MaterialHullShader* asset);
		}
	}
}