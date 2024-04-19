#pragma once

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace fxeffectdef
		{
			zonetool::s1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator);
			void dump(FxEffectDef* asset);
		}
	}
}
