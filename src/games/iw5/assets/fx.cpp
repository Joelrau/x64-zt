#include <std_include.hpp>
#include "fx.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_visuals(zone_reader32& reader, const FxElemDef* element, FxElemVisuals& visuals)
		{
			switch (element->elemType)
			{
			case FX_ELEM_TYPE_SPRITE_BILLBOARD:
			case FX_ELEM_TYPE_SPRITE_ORIENTED:
			case FX_ELEM_TYPE_TAIL:
			case FX_ELEM_TYPE_TRAIL:
			case FX_ELEM_TYPE_CLOUD:
			case FX_ELEM_TYPE_SPARKCLOUD:
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				reader.read_asset(ASSET_TYPE_MATERIAL, visuals.material);
				break;
			case FX_ELEM_TYPE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, visuals.model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				reader.read_string(visuals.effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
				reader.read_string(visuals.soundName);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				reader.read_asset(ASSET_TYPE_LIGHT_DEF, visuals.lightDef);
				break;
			default:
				break;
			}
		}

		void read_extended(zone_reader32& reader, FxElemDef* element)
		{
			auto& extended = element->extended;
			switch (element->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				if (const auto trail = reader.read_single(extended.trailDef, 3))
				{
					reader.read_array(trail->verts, 3, trail->vertCount);
					reader.read_array(trail->inds, 1, trail->indCount);
				}
				break;
			case FX_ELEM_TYPE_SPARKFOUNTAIN:
				reader.read_single(extended.sparkFountainDef, 3);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				reader.read_single(extended.spotLightDef, 3);
				break;
			default:
				reader.read_single(extended.unknownDef, 0);
				break;
			}
		}

		void read_element(zone_reader32& reader, FxElemDef* element)
		{
			reader.read_array(element->velSamples, 3, element->velIntervalCount + 1);
			reader.read_array(element->visSamples, 3, element->visStateIntervalCount + 1);

			if (element->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (const auto marks = reader.read_array(element->visuals.markArray, 3, element->visualCount))
				{
					for (auto i = 0; i < element->visualCount; i++)
					{
						for (auto& material : marks[i].materials)
						{
							reader.read_asset(ASSET_TYPE_MATERIAL, material);
						}
					}
				}
			}
			else if (element->visualCount > 1)
			{
				if (const auto array = reader.read_array(element->visuals.array, 3, element->visualCount))
				{
					for (auto i = 0; i < element->visualCount; i++)
					{
						read_visuals(reader, element, array[i]);
					}
				}
			}
			else
			{
				read_visuals(reader, element, element->visuals.instance);
			}

			reader.read_string(element->effectOnImpact.name);
			reader.read_string(element->effectOnDeath.name);
			reader.read_string(element->effectEmitted.name);

			read_extended(reader, element);
		}
	}

	void fx::read(zone_reader32& reader, FxEffectDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto count = asset->elemDefCountEmission + asset->elemDefCountLooping + asset->elemDefCountOneShot;
		if (const auto elements = reader.read_array(asset->elemDefs, 3, count))
		{
			for (auto i = 0; i < count; i++)
			{
				read_element(reader, &elements[i]);
			}
		}

		reader.pop_stream();
	}
}
