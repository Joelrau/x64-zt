#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "fxeffectdef.hpp"

#include "zonetool/h1/assets/fxeffectdef.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, zone_memory* mem)
			{
				auto* new_asset = mem->allocate<zonetool::h1::FxEffectDef>();

				memcpy(new_asset, asset, sizeof(zonetool::h1::FxEffectDef));

				auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
				new_asset->elemDefs = mem->allocate<zonetool::h1::FxElemDef>(count);
				for (auto i = 0; i < count; i++)
				{
					auto* elem = &asset->elemDefs[i];
					auto* new_elem = &new_asset->elemDefs[i];

					memcpy(new_elem, elem, sizeof(zonetool::h1::FxElemDef));

					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						if (elem->visuals.markArray)
						{
							new_elem->visuals.markArray = mem->allocate<zonetool::h1::FxElemMarkVisuals>(elem->visualCount);

							for (unsigned char j = 0; j < elem->visualCount; j++)
							{
								new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[0]); // mc
								new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc
								new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc displacement
							}
						}
					}
				}

				return new_asset;
			}

			void dump(FxEffectDef* asset, zone_memory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::fx_effect_def::dump(converted_asset);
			}
		}
	}
}