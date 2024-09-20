#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "tracerdef.hpp"

#include "zonetool/h1/assets/tracerdef.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tracer_def
		{
			zonetool::h1::TracerDef* convert(TracerDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::TracerDef>();

				REINTERPRET_CAST_SAFE(name);
				new_asset->material = reinterpret_cast<zonetool::h1::Material*>(asset->material);
				new_asset->effectDef = nullptr;
				COPY_VALUE(drawInterval);
				COPY_VALUE(speed);
				COPY_VALUE(beamLength);
				COPY_VALUE(beamWidth);
				COPY_VALUE(screwRadius);
				COPY_VALUE(screwDist);
				COPY_ARR(colors);

				return new_asset;
			}

			void dump(TracerDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::tracer_def::dump(converted_asset);
			}
		}
	}
}