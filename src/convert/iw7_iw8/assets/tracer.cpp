#include <std_include.hpp>

#include "../include.hpp"
#include "tracer.hpp"
#include "material.hpp"

#include <iw8/assets/tracer.hpp>

namespace zonetool::iw7::converter::iw8::tracer
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::TracerDef* convert(const TracerDef* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::TracerDef>();
			result->name = asset->name;
			result->drawInterval = asset->drawInterval;
			result->speed = asset->speed;
			result->drawLegacyTracer = true;
			result->fadeOverTime = asset->fadeOverTime;
			result->fadeTime = asset->fadeTime;
			result->beamLength = asset->beamLength;
			result->beamWidth = asset->beamWidth;
			result->screwRadius = asset->screwRadius;
			result->screwDist = asset->screwDist;
			std::memcpy(result->colors, asset->colors, sizeof(result->colors));

			if (asset->material)
			{
				result->material = allocator.allocate<target::Material>();
				result->material->name = asset->material->name;
			}

			return result;
		}
	}

	void dump(TracerDef* asset)
	{
		if (!asset || !mark_dumped(ASSET_TYPE_TRACER, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::iw8::tracer_def::dump(convert(asset, allocator));
		material::dump_effect(asset->material);
		add_csv_row("tracer", asset->name);
	}
}
