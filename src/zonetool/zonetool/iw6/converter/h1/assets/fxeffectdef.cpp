#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "fxeffectdef.hpp"

#include "zonetool/h1/assets/fxeffectdef.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxeffectdef
		{
			zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::FxEffectDef>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(flags);
				COPY_VALUE(totalSize);
				COPY_VALUE(msecLoopingLife);
				COPY_VALUE(elemDefCountLooping);
				COPY_VALUE(elemDefCountOneShot);
				COPY_VALUE(elemDefCountEmission);
				COPY_VALUE(elemMaxRadius);
				COPY_VALUE(occlusionQueryDepthBias);
				COPY_VALUE(occlusionQueryFadeIn);
				COPY_VALUE(occlusionQueryFadeOut);
				COPY_VALUE_CAST(occlusionQueryScaleRange);
				
				auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
				new_asset->elemDefs = mem->Alloc<zonetool::h1::FxElemDef>(count);
				for (auto i = 0; i < count; i++)
				{
					auto* elem = &asset->elemDefs[i];
					auto* new_elem = &new_asset->elemDefs[i];

					COPY_VALUE_CAST(elemDefs[i].flags); // convert?
					COPY_VALUE_CAST(elemDefs[i].flags2);
					COPY_VALUE_CAST(elemDefs[i].spawn);
					COPY_VALUE_CAST(elemDefs[i].spawnRange);
					COPY_VALUE_CAST(elemDefs[i].fadeInRange);
					COPY_VALUE_CAST(elemDefs[i].fadeOutRange);
					COPY_VALUE_CAST(elemDefs[i].spawnFrustumCullRadius);
					COPY_VALUE_CAST(elemDefs[i].spawnDelayMsec);
					COPY_VALUE_CAST(elemDefs[i].lifeSpanMsec);
					COPY_ARR(elemDefs[i].spawnOrigin);
					COPY_VALUE_CAST(elemDefs[i].spawnOffsetRadius);
					COPY_VALUE_CAST(elemDefs[i].spawnOffsetHeight);
					COPY_ARR(elemDefs[i].spawnAngles);
					COPY_ARR(elemDefs[i].angularVelocity);
					COPY_VALUE_CAST(elemDefs[i].initialRotation);
					COPY_VALUE_CAST(elemDefs[i].gravity);
					COPY_VALUE_CAST(elemDefs[i].reflectionFactor);
					COPY_VALUE_CAST(elemDefs[i].atlas);
					COPY_VALUE_CAST(elemDefs[i].elemType); // convert?
					COPY_VALUE_CAST(elemDefs[i].elemLitType); // convert?
					COPY_VALUE_CAST(elemDefs[i].visualCount);
					COPY_VALUE_CAST(elemDefs[i].velIntervalCount);
					COPY_VALUE_CAST(elemDefs[i].visStateIntervalCount);
					REINTERPRET_CAST_SAFE(elemDefs[i].velSamples);

					if (elem->visSamples)
					{
						new_elem->visSamples = mem->Alloc<zonetool::h1::FxElemVisStateSample>(elem->visStateIntervalCount + 1);
						for (int ii = 0; ii < elem->visStateIntervalCount + 1; ii++)
						{
							// base
							new_elem->visSamples[ii].base.color[0] = elem->visSamples[ii].base.color[0];
							new_elem->visSamples[ii].base.color[1] = elem->visSamples[ii].base.color[1];
							new_elem->visSamples[ii].base.color[2] = elem->visSamples[ii].base.color[2];
							new_elem->visSamples[ii].base.color[3] = elem->visSamples[ii].base.color[3];
							new_elem->visSamples[ii].base.pad1[0] = 1.0f;
							new_elem->visSamples[ii].base.pad1[1] = 1.0f;
							new_elem->visSamples[ii].base.pad1[2] = 1.0f;
							new_elem->visSamples[ii].base.rotationDelta = elem->visSamples[ii].base.rotationDelta;
							new_elem->visSamples[ii].base.rotationTotal = elem->visSamples[ii].base.rotationTotal;
							memcpy(&new_elem->visSamples[ii].base.size, &elem->visSamples[ii].base.size, sizeof(float[2]));
							new_elem->visSamples[ii].base.scale = elem->visSamples[ii].base.scale;
							new_elem->visSamples[ii].base.pad2[1] = 0.0f;
							new_elem->visSamples[ii].base.pad2[1] = 0.0f;

							// amplitude
							new_elem->visSamples[ii].amplitude.color[0] = elem->visSamples[ii].amplitude.color[0];
							new_elem->visSamples[ii].amplitude.color[1] = elem->visSamples[ii].amplitude.color[1];
							new_elem->visSamples[ii].amplitude.color[2] = elem->visSamples[ii].amplitude.color[2];
							new_elem->visSamples[ii].amplitude.color[3] = elem->visSamples[ii].amplitude.color[3];
							new_elem->visSamples[ii].amplitude.pad1[0] = 1.0f;
							new_elem->visSamples[ii].amplitude.pad1[1] = 1.0f;
							new_elem->visSamples[ii].amplitude.pad1[2] = 1.0f;
							new_elem->visSamples[ii].amplitude.rotationDelta = elem->visSamples[ii].amplitude.rotationDelta;
							new_elem->visSamples[ii].amplitude.rotationTotal = elem->visSamples[ii].amplitude.rotationTotal;
							memcpy(&new_elem->visSamples[ii].amplitude.size, &elem->visSamples[ii].amplitude.size, sizeof(float[2]));
							new_elem->visSamples[ii].amplitude.scale = elem->visSamples[ii].amplitude.scale;
							new_elem->visSamples[ii].amplitude.pad2[1] = 0.0f;
							new_elem->visSamples[ii].amplitude.pad2[1] = 0.0f;
						}
					}

					// visuals
					if (elem->elemType == FX_ELEM_TYPE_DECAL)
					{
						if (elem->visuals.markArray)
						{
							new_elem->visuals.markArray = mem->Alloc<zonetool::h1::FxElemMarkVisuals>(elem->visualCount);

							for (unsigned char j = 0; j < elem->visualCount; j++)
							{
								new_elem->visuals.markArray[j].materials[0] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[0]); // mc
								new_elem->visuals.markArray[j].materials[1] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc
								new_elem->visuals.markArray[j].materials[2] = reinterpret_cast<zonetool::h1::Material*>(elem->visuals.markArray[j].materials[1]); // wc displacement
							}
						}
					}
					else if (elem->visualCount > 1)
					{
						new_elem->visuals.array = mem->Alloc<zonetool::h1::FxElemVisuals>(elem->visualCount);
						for (int j = 0; j < elem->visualCount; j++)
						{
							// this is fine
							new_elem->visuals.array[j].anonymous = elem->visuals.array[j].anonymous;
						}
					}
					else if (elem->visualCount)
					{
						// this is fine
						new_elem->visuals.instance.anonymous = elem->visuals.instance.anonymous;
					}

					COPY_VALUE_CAST(elemDefs[i].collBounds);

					new_elem->effectOnImpact.name = elem->effectOnImpact.name;
					new_elem->effectOnDeath.name = elem->effectOnDeath.name;
					new_elem->effectEmitted.name = elem->effectEmitted.name;

					COPY_VALUE_CAST(elemDefs[i].emitDist);
					COPY_VALUE_CAST(elemDefs[i].emitDistVariance);

					switch (elem->elemType)
					{
					case FX_ELEM_TYPE_TRAIL:
						new_elem->extended.trailDef = mem->Alloc<zonetool::h1::FxTrailDef>();

						// check
						new_elem->extended.trailDef->scrollTimeMsec = elem->extended.trailDef->scrollTimeMsec;
						new_elem->extended.trailDef->repeatDist = elem->extended.trailDef->repeatDist;
						new_elem->extended.trailDef->invSplitDist = elem->extended.trailDef->invSplitDist;
						new_elem->extended.trailDef->invSplitArcDist = elem->extended.trailDef->invSplitArcDist;
						new_elem->extended.trailDef->invSplitTime = elem->extended.trailDef->invSplitTime;
						// pad

						new_elem->extended.trailDef->vertCount = elem->extended.trailDef->vertCount;
						new_elem->extended.trailDef->verts = mem->Alloc<zonetool::h1::FxTrailVertex>(new_elem->extended.trailDef->vertCount);
						for (int j = 0; j < new_elem->extended.trailDef->vertCount; j++)
						{
							// check
							memcpy(&new_elem->extended.trailDef->verts[j].pos, &elem->extended.trailDef->verts[j].pos, sizeof(float[2]));
							memcpy(&new_elem->extended.trailDef->verts[j].normal, &elem->extended.trailDef->verts[j].normal, sizeof(float[2]));
							memcpy(&new_elem->extended.trailDef->verts[j].texCoord, &elem->extended.trailDef->verts[j].texCoord, sizeof(float[2]));
							// pad
						}

						new_elem->extended.trailDef->indCount = elem->extended.trailDef->indCount;
						new_elem->extended.trailDef->inds = elem->extended.trailDef->inds;
						break;
					case FX_ELEM_TYPE_SPARK_FOUNTAIN:
						// should be fine
						new_elem->extended.sparkFountainDef = reinterpret_cast<zonetool::h1::FxSparkFountainDef*>(elem->extended.sparkFountainDef);
						break;
					case FX_ELEM_TYPE_SPOT_LIGHT:
						// check
						new_elem->extended.spotLightDef = mem->Alloc<zonetool::h1::FxSpotLightDef>();
						if (elem->extended.spotLightDef)
						{
							new_elem->extended.spotLightDef->fovInnerFraction = elem->extended.spotLightDef->fovInnerFraction;
							new_elem->extended.spotLightDef->startRadius = elem->extended.spotLightDef->startRadius;
							new_elem->extended.spotLightDef->endRadius = elem->extended.spotLightDef->endRadius;
							new_elem->extended.spotLightDef->brightness = elem->extended.spotLightDef->brightness;
							new_elem->extended.spotLightDef->maxLength = elem->extended.spotLightDef->maxLength;
							new_elem->extended.spotLightDef->exponent = elem->extended.spotLightDef->exponent;
							// pad
						}
						break;
					case FX_ELEM_TYPE_OMNI_LIGHT:
						new_elem->extended.omniLightDef = mem->Alloc<zonetool::h1::FxOmniLightDef>();
						// todo?
						break;
					case FX_ELEM_TYPE_FLARE:
						// should be fine
						new_elem->extended.flareDef = reinterpret_cast<zonetool::h1::FxFlareDef*>(elem->extended.flareDef);
						break;
					default:
						new_elem->extended.unknownDef = elem->extended.unknownDef;
						break;
					}

					COPY_VALUE(elemDefs[i].sortOrder);
					COPY_VALUE(elemDefs[i].lightingFrac);
					COPY_VALUE(elemDefs[i].useItemClip);
					COPY_VALUE(elemDefs[i].fadeInfo);
					COPY_VALUE(elemDefs[i].randomSeed);
					
					// unknown stuff in h1
					memcpy(&new_elem->__pad0, &elem->litMaxColorChangePerSec, sizeof(float[4]));
				}

				return new_asset;
			}

			void dump(FxEffectDef* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IFxEffectDef::dump(converted_asset);
			}
		}
	}
}