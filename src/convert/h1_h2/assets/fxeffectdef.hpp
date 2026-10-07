#pragma once

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace fxeffectdef
		{
			zonetool::h2::FxEffectDef* convert(zonetool::h1::FxEffectDef* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h1::FxEffectDef* asset);
		}
	}
}
