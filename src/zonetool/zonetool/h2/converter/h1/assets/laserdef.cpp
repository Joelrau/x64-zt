#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "laserdef.hpp"
#include "zonetool/h1/assets/laserdef.hpp"

#define REINTERPRET_CAST_COPY_NAME(__name__) \
	if (asset->__name__ != nullptr) \
	{ \
		new_asset->__name__ = allocator.allocate<std::remove_pointer<decltype(new_asset->__name__)>::type>(); \
		new_asset->__name__->name = allocator.duplicate_string(asset->__name__->name); \
	} \

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace laserdef
		{
			zonetool::h1::LaserDef* convert(zonetool::h2::LaserDef* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::LaserDef>();

				REINTERPRET_CAST_SAFE(name);
				REINTERPRET_CAST_COPY_NAME(laserMaterial);
				REINTERPRET_CAST_COPY_NAME(laserLightMaterial);
				REINTERPRET_CAST_COPY_NAME(effect);
				REINTERPRET_CAST_COPY_NAME(altLaser);

				COPY_VALUE_CAST(value);
				
				// color ?
				new_asset->float_values[4] = asset->float_values[0];
				new_asset->float_values[5] = asset->float_values[1];
				new_asset->float_values[6] = asset->float_values[2];

				new_asset->float_values[11] = asset->float_values[3];
				new_asset->float_values[12] = asset->float_values[4];
				new_asset->float_values[13] = asset->float_values[5];
				new_asset->float_values[14] = asset->float_values[6];
				new_asset->float_values[15] = asset->float_values[7];
				new_asset->float_values[16] = asset->float_values[8];
				new_asset->float_values[17] = asset->float_values[9];
				new_asset->float_values[18] = asset->float_values[10];
				new_asset->float_values[19] = asset->float_values[11];
				new_asset->float_values[20] = asset->float_values[12];
				new_asset->float_values[21] = asset->float_values[13];
				new_asset->float_values[22] = asset->float_values[14];
				new_asset->float_values[23] = asset->float_values[15];
				new_asset->float_values[24] = asset->float_values[16];

				COPY_ARR(char_values);

				return new_asset;
			}

			void dump(zonetool::h2::LaserDef* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::laser_def::dump(converted_asset);
			}
		}
	}
}

#undef REINTERPRET_CAST_COPY_NAME
