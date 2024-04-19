#include <std_include.hpp>
#include "zonetool/h1/converter/s1/include.hpp"
#include "fxeffectdef.hpp"

#include "zonetool/s1/assets/fxeffectdef.hpp"

namespace zonetool::h1
{
	namespace converter::s1
	{
		namespace fxeffectdef
		{
			zonetool::s1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::s1::FxEffectDef>();
				std::memcpy(new_asset, asset, sizeof(zonetool::s1::FxEffectDef));

				const auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
				new_asset->elemDefs = allocator.allocate_array<zonetool::s1::FxElemDef>(count);

				for (auto i = 0; i < count; i++)
				{
					const auto elem = &asset->elemDefs[i];
					const auto new_elem = &new_asset->elemDefs[i];

					std::memcpy(new_elem, elem, sizeof(zonetool::s1::FxElemDef));

					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						if (elem->visuals.markArray)
						{
							new_elem->visuals.markArray = allocator.allocate_array<zonetool::s1::FxElemMarkVisuals>(elem->visualCount);

							for (unsigned char j = 0; j < elem->visualCount; j++)
							{
								new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::s1::Material*>(
									elem->visuals.markArray[j].materials[0]); // mc
								new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::s1::Material*>(
									elem->visuals.markArray[j].materials[1]); // wc
								//new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::s1::Material*>(
									//elem->visuals.markArray[j].materials[1]); // wc displacement
							}
						}
					}
				}

				return new_asset;
			}

			void dump(FxEffectDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::s1::fx_effect_def::dump(converted_asset);
			}
		}
	}
}
