#include <std_include.hpp>

#include "zonetool/h1/converter/h2/include.hpp"
#include "laserdef.hpp"
#include "zonetool/h2/assets/laserdef.hpp"

#define REINTERPRET_CAST_COPY_NAME(__name__) \
	if (asset->__name__ != nullptr) \
	{ \
		new_asset->__name__ = allocator.allocate<std::remove_pointer<decltype(new_asset->__name__)>::type>(); \
		new_asset->__name__->name = allocator.duplicate_string(asset->__name__->name); \
	} \

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace laserdef
		{
			zonetool::h2::LaserDef* convert(LaserDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::LaserDef>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_COPY_NAME(laserMaterial);
				REINTERPRET_CAST_COPY_NAME(laserLightMaterial);
				REINTERPRET_CAST_COPY_NAME(laserEndEffect);
				REINTERPRET_CAST_COPY_NAME(friendlyTeamLaser);

				COPY_VALUE_CAST(laserTag);

				new_asset->hdrColorScale[0] = asset->hdrColorScale[0];
				new_asset->hdrColorScale[1] = asset->hdrColorScale[1];
				new_asset->hdrColorScale[2] = asset->hdrColorScale[2];
				new_asset->hdrColorScale[3] = asset->hdrColorScale[3];

				new_asset->laserLightHdrColorScale[0] = asset->laserLightHdrColorScale[0];
				new_asset->laserLightHdrColorScale[1] = asset->laserLightHdrColorScale[1];
				new_asset->laserLightHdrColorScale[2] = asset->laserLightHdrColorScale[2];
				new_asset->laserLightHdrColorScale[3] = asset->laserLightHdrColorScale[3];

				new_asset->range = asset->range;
				new_asset->radius = asset->radius;
				new_asset->endOffset = asset->endOffset;
				new_asset->flarePct = asset->flarePct;
				new_asset->texCoordOffset = asset->texCoordOffset;
				new_asset->laserLightRadius = asset->laserLightRadius;
				new_asset->laserLightBeginOffset = asset->laserLightBeginOffset;
				new_asset->laserLightEndOffset = asset->laserLightEndOffset;
				new_asset->laserLightBodyTweak = asset->laserLightBodyTweak;

				new_asset->ownerOnly = asset->ownerOnly;
				new_asset->nightvisionOnly = asset->nightvisionOnly;
				new_asset->useHalfCylinderGeometry = asset->useHalfCylinderGeometry;
				new_asset->laserLight = asset->laserLight;
				new_asset->laserLightNvgOnly = asset->laserLightNvgOnly;
				new_asset->laserSightLaser = asset->laserSightLaser;

				return new_asset;
			}

			void dump(zonetool::h1::LaserDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::laser_def::dump(converted_asset);
			}
		}
	}
}

#undef REINTERPRET_CAST_COPY_NAME
