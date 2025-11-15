#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "physpreset.hpp"

#include "zonetool/h1/assets/physpreset.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace physpreset
		{
			zonetool::h1::PhysPreset* convert(PhysPreset* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::PhysPreset>();

				COPY_VALUE(name);
				COPY_VALUE(type);
				COPY_VALUE(mass);
				COPY_VALUE(bounce);
				COPY_VALUE(friction);
				COPY_VALUE(bulletForceScale);
				COPY_VALUE(explosiveForceScale);
				new_asset->explosiveSpinScale = 0.0f;
				new_asset->unk2 = 42001553;
				COPY_VALUE(sndAliasPrefix);
				COPY_VALUE(piecesSpreadFraction);
				COPY_VALUE(piecesUpwardVelocity);
				COPY_VALUE(minMomentum);
				COPY_VALUE(maxMomentum);
				COPY_VALUE(minVolume);
				COPY_VALUE(maxVolume);
				COPY_VALUE(minPitch);
				COPY_VALUE(maxPitch);
				COPY_VALUE_CAST(volumeType);
				COPY_VALUE_CAST(pitchType);
				COPY_VALUE(tempDefaultToCylinder);
				COPY_VALUE(perSurfaceSndAlias);
				COPY_VALUE(name);
				COPY_VALUE(name);

				return new_asset;
			}

			void dump(PhysPreset* asset)
			{
				utils::memory::allocator allocator;
				auto* converted_asset = convert(asset, allocator);
				zonetool::h1::phys_preset::dump(converted_asset);
			}
		}
	}
}