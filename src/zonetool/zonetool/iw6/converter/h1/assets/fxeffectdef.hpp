#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, zone_memory* mem);
			void dump(FxEffectDef* asset, zone_memory* mem);
		}
	}
}