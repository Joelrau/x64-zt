#pragma once

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, ZoneMemory* mem);
			void dump(FxEffectDef* asset, ZoneMemory* mem);
		}
	}
}