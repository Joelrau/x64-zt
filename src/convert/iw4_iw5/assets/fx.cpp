#include <std_include.hpp>

#include "../include.hpp"
#include "fx.hpp"

namespace zonetool::iw4::converter::iw5
{
	namespace fx
	{
		zonetool::iw5::FxEffectDef* convert(zonetool::iw4::FxEffectDef* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw5::FxEffectDef>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(flags);
			COPY_VALUE(totalSize);
			COPY_VALUE(msecLoopingLife);
			COPY_VALUE(elemDefCountLooping);
			COPY_VALUE(elemDefCountOneShot);
			COPY_VALUE(elemDefCountEmission);

			const auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
			const auto elements = allocator.allocate<zonetool::iw5::FxElemDef>(count);
			static_assert(offsetof(zonetool::iw5::FxElemDef, randomSeed) == sizeof(zonetool::iw4::FxElemDef));
			for (auto i = 0; i < count; i++)
			{
				std::memcpy(&elements[i], &asset->elemDefs.get()[i], sizeof(zonetool::iw4::FxElemDef));
			}
			new_asset->elemDefs = elements;

			return new_asset;
		}
	}
}
