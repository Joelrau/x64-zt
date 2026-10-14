#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		enum fx_elem_type : std::uint8_t
		{
			FX_ELEM_TYPE_TRAIL = 5,
			FX_ELEM_TYPE_FLARE = 6,
			FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION = 7,
			FX_ELEM_TYPE_SPARK_FOUNTAIN = 10,
			FX_ELEM_TYPE_MODEL = 11,
			FX_ELEM_TYPE_OMNI_LIGHT = 12,
			FX_ELEM_TYPE_SPOT_LIGHT = 13,
			FX_ELEM_TYPE_SOUND = 14,
			FX_ELEM_TYPE_VECTORFIELD = 15,
			FX_ELEM_TYPE_DECAL = 16,
			FX_ELEM_TYPE_RUNNER = 17,
			FX_ELEM_TYPE_STRING_18 = 18,
			FX_ELEM_TYPE_STRING_19 = 19,
			FX_ELEM_TYPE_INLINE_20 = 20,
		};

		void read_visual(zone_reader& reader, const std::uint8_t elem_type, FxElemVisuals* visual)
		{
			switch (elem_type)
			{
			case FX_ELEM_TYPE_MODEL:
				reader.read_asset(ASSET_TYPE_XMODEL, visual->model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				reader.read_string(visual->effectDef.name);
				break;
			case FX_ELEM_TYPE_SOUND:
			case FX_ELEM_TYPE_VECTORFIELD:
			case FX_ELEM_TYPE_STRING_18:
			case FX_ELEM_TYPE_STRING_19:
				reader.read_string(visual->soundName);
				break;
			case FX_ELEM_TYPE_INLINE_20:
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				reader.read_asset(ASSET_TYPE_PARTICLE_SIM_ANIMATION, visual->particleSimAnimation);
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				reader.read_asset(ASSET_TYPE_LIGHT_DEF, visual->lightDef);
				break;
			default:
				reader.read_asset(ASSET_TYPE_MATERIAL, visual->material);
				break;
			}
		}

		void read_visuals(zone_reader& reader, FxElemDef* elem)
		{
			if (elem->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (const auto marks = reader.read_array(elem->visuals.markArray, 7, elem->visualCount))
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						for (auto& material : marks[i].materials)
						{
							reader.read_asset(ASSET_TYPE_MATERIAL, material);
						}
					}
				}
			}
			else if (elem->visualCount > 1)
			{
				if (const auto visuals = reader.read_array(elem->visuals.array, 7, elem->visualCount))
				{
					for (auto i = 0; i < elem->visualCount; i++)
					{
						read_visual(reader, elem->elemType, &visuals[i]);
					}
				}
			}
			else
			{
				read_visual(reader, elem->elemType, &elem->visuals.instance);
			}
		}

		void read_extended(zone_reader& reader, FxElemDef* elem)
		{
			auto& extended = elem->extended;

			switch (elem->elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				if (const auto trail = reader.read_single(extended.trailDef, 7))
				{
					reader.read_array(trail->verts, 3, trail->vertCount);
					reader.read_array(trail->inds, 1, trail->indCount);
				}
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				reader.read_single(extended.sparkFountainDef, 3);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				if (const auto spot = reader.read_single(extended.spotLightDef, 7))
				{
					reader.read_string(spot->name);
				}
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				if (const auto omni = reader.read_single(extended.omniLightDef, 7))
				{
					reader.read_string(omni->name);
				}
				break;
			case FX_ELEM_TYPE_FLARE:
				if (const auto flare = reader.read_single(extended.flareDef, 7))
				{
					reader.read_array(flare->intensityX, 3, flare->intensityXIntervalCount + 1);
					reader.read_array(flare->intensityY, 3, flare->intensityYIntervalCount + 1);
					reader.read_array(flare->srcCosIntensity, 3, flare->srcCosIntensityIntervalCount + 1);
					reader.read_array(flare->srcCosScale, 3, flare->srcCosScaleIntervalCount + 1);
				}
				break;
			default:
				reader.read_single(extended.unknownBytes, 0);
				break;
			}
		}

		void read_elem(zone_reader& reader, FxElemDef* elem)
		{
			reader.read_stream(elem->velSamples, 3, 148, elem->velIntervalCount + 1);
			reader.read_stream(elem->visSamples, 3, 120, elem->visStateIntervalCount + 1);
			read_visuals(reader, elem);
			reader.read_string(elem->effectOnImpact.name);
			reader.read_string(elem->effectOnDeath.name);
			reader.read_string(elem->effectEmitted.name);
			read_extended(reader, elem);
			reader.read_string(elem->name);
		}
	}

	void fx_effect_def::read(zone_reader& reader, FxEffectDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto elem_count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		if (const auto elems = reader.read_array(asset->elemDefs, 7, elem_count))
		{
			for (auto i = 0; i < elem_count; i++)
			{
				read_elem(reader, &elems[i]);
			}
		}

		reader.pop_stream();
	}
}
