#pragma once

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(zonetool::h2::FxEffectDef* asset, utils::memory::allocator& allocator);
			void dump(zonetool::h2::FxEffectDef* asset);
		}
	}
}
