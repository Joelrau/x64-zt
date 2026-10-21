#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator);
			void dump(FxEffectDef* asset);
		}
	}
}
