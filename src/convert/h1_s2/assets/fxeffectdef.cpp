#include <std_include.hpp>

#include "../include.hpp"
#include "fxeffectdef.hpp"
#include "material.hpp"

namespace zonetool::h1::converter::s2::fxeffectdef
{
	namespace
	{
		constexpr auto stock_unknown_range_base = 0.5f;
		constexpr auto stock_unknown328 = 0.5f;
		constexpr auto stock_unknown344 = 1.0f;

		static_assert(sizeof(FxElemVelStateInFrame) == sizeof(zonetool::s2::FxElemVelStateInFrame));

		int elem_count(const FxEffectDef* asset)
		{
			return asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		}

		int elem_count(const zonetool::s2::FxEffectDef* asset)
		{
			return asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		}

		bool is_supported(const FxEffectDef* asset, const FxElemDef& elem)
		{
			// TODO: S2 has no stock spot light effect to verify FxSpotLightDef
			if (elem.elemType == FX_ELEM_TYPE_SPOT_LIGHT)
			{
				ZONETOOL_WARNING("fx \"%s\": elem type %d has no S2 conversion, the elem is removed", asset->name, elem.elemType);
				return false;
			}

			return true;
		}

		zonetool::s2::FxElemType convert_elem_type(const FxElemType type)
		{
			switch (type)
			{
			case FX_ELEM_TYPE_DECAL:
				return zonetool::s2::FX_ELEM_TYPE_DECAL;
			case FX_ELEM_TYPE_RUNNER:
				return zonetool::s2::FX_ELEM_TYPE_RUNNER;
			case FX_ELEM_TYPE_VECTORFIELD:
				return zonetool::s2::FX_ELEM_TYPE_VECTORFIELD;
			default:
				return static_cast<zonetool::s2::FxElemType>(type);
			}
		}

		const char* effect_name(const FxEffectDefRef& reference)
		{
			return reference.handle ? reference.handle->name : nullptr;
		}

		// s2 draws sprite, tail and line elems from sprite records, only "xl"/"xv" sprite techsets read them
		bool is_sprite(const FxElemType type)
		{
			return type <= FX_ELEM_TYPE_LINE;
		}

		zonetool::s2::FxElemVisuals convert_visual(const FxElemType type, const FxElemVisuals& visual, utils::memory::allocator& allocator)
		{
			zonetool::s2::FxElemVisuals result{};
			switch (type)
			{
			case FX_ELEM_TYPE_MODEL:
				result.model = reinterpret_cast<zonetool::s2::XModel*>(visual.model);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				result.particleSimAnimation = reinterpret_cast<zonetool::s2::FxParticleSimAnimation*>(visual.particleSimAnimation);
				break;
			case FX_ELEM_TYPE_RUNNER:
				result.effectDef.name = effect_name(visual.effectDef);
				break;
			case FX_ELEM_TYPE_SOUND:
				result.soundName = visual.soundName;
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				result.vectorFieldName = visual.vectorFieldName;
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				break;
			default:
				result.material = is_sprite(type) && visual.material
					? make_stub<zonetool::s2::Material>(material::dump_sprite(visual.material), allocator)
					: reinterpret_cast<zonetool::s2::Material*>(visual.material);
				break;
			}

			return result;
		}

		zonetool::s2::FxElemDefVisuals convert_visuals(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			zonetool::s2::FxElemDefVisuals result{};
			if (elem.elemType == FX_ELEM_TYPE_DECAL)
			{
				if (!elem.visuals.markArray)
				{
					return result;
				}

				result.markArray = allocator.allocate_array<zonetool::s2::FxElemMarkVisuals>(elem.visualCount);
				for (auto i = 0; i < elem.visualCount; i++)
				{
					for (auto j = 0u; j < std::size(result.markArray[i].materials); j++)
					{
						result.markArray[i].materials[j] = reinterpret_cast<zonetool::s2::Material*>(elem.visuals.markArray[i].materials[j]);
					}
				}
			}
			else if (elem.visualCount > 1)
			{
				if (!elem.visuals.array)
				{
					return result;
				}

				result.array = allocator.allocate_array<zonetool::s2::FxElemVisuals>(elem.visualCount);
				for (auto i = 0; i < elem.visualCount; i++)
				{
					result.array[i] = convert_visual(elem.elemType, elem.visuals.array[i], allocator);
				}
			}
			else
			{
				result.instance = convert_visual(elem.elemType, elem.visuals.instance, allocator);
			}

			return result;
		}

		zonetool::s2::FxElemVelStateSample* convert_vel_samples(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			if (!elem.velSamples)
			{
				return nullptr;
			}

			const auto count = elem.velIntervalCount + 1;
			const auto samples = allocator.allocate_array<zonetool::s2::FxElemVelStateSample>(count);
			for (auto i = 0; i < count; i++)
			{
				samples[i].vectorFieldScale = 1.0f;
				std::memcpy(&samples[i].local, &elem.velSamples[i].local, sizeof(samples[i].local));
				std::memcpy(&samples[i].world, &elem.velSamples[i].world, sizeof(samples[i].world));
			}

			return samples;
		}

		zonetool::s2::FxElemVisualState convert_visual_state(const FxElemVisualState& state)
		{
			zonetool::s2::FxElemVisualState result{};
			std::memcpy(result.color, state.color, sizeof(result.color));
			std::memcpy(result.emissiveScale, state.emissiveScale, sizeof(result.emissiveScale));
			result.rotationDelta = state.rotationDelta;
			result.rotationTotal = state.rotationTotal;
			std::memcpy(result.size, state.size, sizeof(result.size));
			result.scale = state.scale;
			std::memcpy(result.pivot, state.pivot, sizeof(result.pivot));
			return result;
		}

		// S2 bakes the atlas frame curve into the vis samples, this rule matches every stock mp_aachen_v2 sample
		void set_atlas_frames(const FxElemDef& elem, zonetool::s2::FxElemVisStateSample* samples, const int count)
		{
			if (elem.elemType == FX_ELEM_TYPE_DECAL)
			{
				return;
			}

			const auto& atlas = elem.atlas;
			const auto play_over_life = (atlas.behavior & zonetool::s2::FX_ATLAS_PLAY_OVER_LIFE) || atlas.entryCount == 1;
			const auto frames_per_msec = atlas.fps / 1000.0f;

			for (auto i = 0; i < count; i++)
			{
				const auto life = count > 1 ? static_cast<float>(i) / static_cast<float>(count - 1) : 0.0f;
				if (play_over_life)
				{
					samples[i].base.atlasFrame = -1.0f;
					samples[i].amplitude.atlasFrame = atlas.entryCount > 1 ? atlas.entryCount * life : 0.0f;
				}
				else
				{
					samples[i].base.atlasFrame = frames_per_msec * life;
					samples[i].amplitude.atlasFrame = frames_per_msec;
				}
			}
		}

		zonetool::s2::FxElemVisStateSample* convert_vis_samples(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			if (!elem.visSamples)
			{
				return nullptr;
			}

			const auto count = elem.visStateIntervalCount + 1;
			const auto samples = allocator.allocate_array<zonetool::s2::FxElemVisStateSample>(count);
			for (auto i = 0; i < count; i++)
			{
				samples[i].base = convert_visual_state(elem.visSamples[i].base);
				samples[i].amplitude = convert_visual_state(elem.visSamples[i].amplitude);
			}

			set_atlas_frames(elem, samples, count);
			return samples;
		}

		zonetool::s2::FxTrailDef* convert_trail(const FxTrailDef* trail, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<zonetool::s2::FxTrailDef>();
			result->scrollTimeMsec = trail->scrollTimeMsec;
			result->repeatDist = trail->repeatDist;
			result->invSplitDist = trail->invSplitDist;
			result->invSplitArcDist = trail->invSplitArcDist;
			result->invSplitTime = trail->invSplitTime;
			result->headFadingFactor = trail->headFadingFactor;
			result->tailFadingFactor = trail->tailFadingFactor;
			result->vertCount = trail->vertCount;
			result->verts = retype<zonetool::s2::FxTrailVertex>(trail->verts);
			result->indCount = trail->indCount;
			result->inds = trail->inds;
			return result;
		}

		zonetool::s2::FxOmniLightDef* convert_omni_light(const FxOmniLightDef* light, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<zonetool::s2::FxOmniLightDef>();
			result->bulbRadius = light->bulbRadius;
			result->bulbLength = light->bulbLength;
			result->fadeOffsetRt[0] = light->fadeOffsetRt[0];
			result->fadeOffsetRt[1] = light->fadeOffsetRt[1];
			result->name = "";
			return result;
		}

		zonetool::s2::FxFlareDef* convert_flare(const FxFlareDef* flare, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<zonetool::s2::FxFlareDef>();
			result->position = flare->position;
			result->angularRotCount = flare->angularRotCount;
			result->flags = flare->flags;
			result->depthScaleRange = {flare->depthScaleRange.base, flare->depthScaleRange.amplitude};
			result->depthScaleValue = {flare->depthScaleValue.base, flare->depthScaleValue.amplitude};
			result->radialRot = {flare->radialRot.base, flare->radialRot.amplitude};
			result->radialScaleX = {flare->radialScaleX.base, flare->radialScaleX.amplitude};
			result->radialScaleY = {flare->radialScaleY.base, flare->radialScaleY.amplitude};
			std::memcpy(result->dir, flare->dir, sizeof(result->dir));
			result->intensityXIntervalCount = flare->intensityXIntervalCount;
			result->intensityYIntervalCount = flare->intensityYIntervalCount;
			result->srcCosIntensityIntervalCount = flare->srcCosIntensityIntervalCount;
			result->srcCosScaleIntervalCount = flare->srcCosScaleIntervalCount;
			result->intensityX = flare->intensityX;
			result->intensityY = flare->intensityY;
			result->srcCosIntensity = flare->srcCosIntensity;
			result->srcCosScale = flare->srcCosScale;
			return result;
		}

		zonetool::s2::FxElemExtendedDefPtr convert_extended(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			zonetool::s2::FxElemExtendedDefPtr result{};
			if (!elem.extended.unknownDef)
			{
				return result;
			}

			switch (elem.elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				result.trailDef = convert_trail(elem.extended.trailDef, allocator);
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				result.sparkFountainDef = retype<zonetool::s2::FxSparkFountainDef>(elem.extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				result.omniLightDef = convert_omni_light(elem.extended.omniLightDef, allocator);
				break;
			case FX_ELEM_TYPE_FLARE:
				result.flareDef = convert_flare(elem.extended.flareDef, allocator);
				break;
			default:
				result.unknownBytes = elem.extended.unknownDef;
				break;
			}

			return result;
		}

		zonetool::s2::FxFloatRange convert_range(const FxFloatRange& range)
		{
			return {range.base, range.amplitude};
		}

		void convert_ranges(const FxFloatRange* ranges, zonetool::s2::FxFloatRange* result, const std::size_t count)
		{
			for (auto i = 0u; i < count; i++)
			{
				result[i] = convert_range(ranges[i]);
			}
		}

		void convert_elem(const FxElemDef& elem, zonetool::s2::FxElemDef& result, utils::memory::allocator& allocator)
		{
			result.flags = elem.flags;
			result.flags2 = static_cast<unsigned int>(elem.flags2) & zonetool::s2::FX_ELEM2_H1_MASK;
			result.spawn.looping.intervalMsec = elem.spawn.looping.intervalMsec;
			result.spawn.looping.count = elem.spawn.looping.count;
			result.spawnRange = convert_range(elem.spawnRange);
			result.fadeInRange = convert_range(elem.fadeInRange);
			result.fadeOutRange = convert_range(elem.fadeOutRange);
			result.spawnFrustumCullRadius = elem.spawnFrustumCullRadius;
			result.spawnDelayMsec = {elem.spawnDelayMsec.base, elem.spawnDelayMsec.amplitude};
			result.lifeSpanMsec = {elem.lifeSpanMsec.base, elem.lifeSpanMsec.amplitude};
			convert_ranges(elem.spawnOrigin, result.spawnOrigin, std::size(result.spawnOrigin));
			result.spawnOffsetRadius = convert_range(elem.spawnOffsetRadius);
			result.spawnOffsetHeight = convert_range(elem.spawnOffsetHeight);
			convert_ranges(elem.spawnAngles, result.spawnAngles, std::size(result.spawnAngles));
			convert_ranges(elem.angularVelocity, result.angularVelocity, std::size(result.angularVelocity));
			result.initialRotation = convert_range(elem.initialRotation);
			result.gravity = convert_range(elem.gravity);
			result.reflectionFactor = convert_range(elem.reflectionFactor);
			result.unknownRange.base = stock_unknown_range_base;
			std::memcpy(&result.atlas, &elem.atlas, sizeof(result.atlas));
			result.elemType = convert_elem_type(elem.elemType);
			result.elemLitType = elem.elemLitType;
			result.visualCount = elem.visualCount;
			result.velIntervalCount = elem.velIntervalCount;
			result.visStateIntervalCount = elem.visStateIntervalCount;
			result.velSamples = convert_vel_samples(elem, allocator);
			result.visSamples = convert_vis_samples(elem, allocator);
			result.visuals = convert_visuals(elem, allocator);
			std::memcpy(&result.collBounds, &elem.collBounds, sizeof(result.collBounds));
			result.effectOnImpact.name = effect_name(elem.effectOnImpact);
			result.effectOnDeath.name = effect_name(elem.effectOnDeath);
			result.effectEmitted.name = effect_name(elem.effectEmitted);
			result.emitDist = convert_range(elem.emitDist);
			result.emitDistVariance = convert_range(elem.emitDistVariance);
			result.extended = convert_extended(elem, allocator);
			result.sortOrder = elem.sortOrder;
			result.lightingFrac = elem.lightingFrac;
			result.useItemClip = elem.useItemClip;
			result.fadeInfo = elem.fadeInfo;
			result.fadeOutInfo = elem.fadeOutInfo;
			result.randomSeed = elem.randomSeed;
			result.emissiveScaleScale = elem.emissiveScaleScale;
			result.hdrLightingFrac = elem.hdrLightingFrac;
			result.shadowDensityScale = elem.shadowDensityScale;
			result.scatterRatio = elem.scatterRatio;
			result.volumetricTrailFadeStart = elem.volumetricTrailFadeStart;
			result.unknown328 = stock_unknown328;
			result.unknown344 = stock_unknown344;
			result.name = "";
		}

		int convert_elems(const FxEffectDef* asset, const int first, const int count, zonetool::s2::FxElemDef* result,
			utils::memory::allocator& allocator)
		{
			auto converted = 0;
			for (auto i = first; i < first + count; i++)
			{
				if (is_supported(asset, asset->elemDefs[i]))
				{
					convert_elem(asset->elemDefs[i], result[converted++], allocator);
				}
			}

			return converted;
		}

		std::size_t string_size(const char* str)
		{
			return str ? std::strlen(str) + 1 : 0;
		}

		std::size_t visual_size(const zonetool::s2::FxElemType type, const zonetool::s2::FxElemVisuals& visual)
		{
			switch (type)
			{
			case zonetool::s2::FX_ELEM_TYPE_SOUND:
			case zonetool::s2::FX_ELEM_TYPE_STRING_15:
			case zonetool::s2::FX_ELEM_TYPE_VECTORFIELD:
			case zonetool::s2::FX_ELEM_TYPE_STRING_19:
				return string_size(visual.soundName);
			default:
				return 0;
			}
		}

		std::size_t visuals_size(const zonetool::s2::FxElemDef& elem)
		{
			if (elem.elemType == zonetool::s2::FX_ELEM_TYPE_DECAL)
			{
				return elem.visuals.markArray ? sizeof(zonetool::s2::FxElemMarkVisuals) * elem.visualCount : 0;
			}

			if (elem.visualCount <= 1)
			{
				return visual_size(elem.elemType, elem.visuals.instance);
			}

			if (!elem.visuals.array)
			{
				return 0;
			}

			auto size = sizeof(zonetool::s2::FxElemVisuals) * elem.visualCount;
			for (auto i = 0; i < elem.visualCount; i++)
			{
				size += visual_size(elem.elemType, elem.visuals.array[i]);
			}

			return size;
		}

		std::size_t extended_size(const zonetool::s2::FxElemDef& elem)
		{
			const auto& extended = elem.extended;
			if (!extended.unknownDef)
			{
				return 0;
			}

			switch (elem.elemType)
			{
			case zonetool::s2::FX_ELEM_TYPE_TRAIL:
				return sizeof(zonetool::s2::FxTrailDef) + sizeof(zonetool::s2::FxTrailVertex) * extended.trailDef->vertCount
					+ sizeof(unsigned short) * extended.trailDef->indCount;
			case zonetool::s2::FX_ELEM_TYPE_SPARK_FOUNTAIN:
				return sizeof(zonetool::s2::FxSparkFountainDef);
			case zonetool::s2::FX_ELEM_TYPE_OMNI_LIGHT:
				return sizeof(zonetool::s2::FxOmniLightDef) + string_size(extended.omniLightDef->name);
			case zonetool::s2::FX_ELEM_TYPE_FLARE:
			{
				const auto* flare = extended.flareDef;
				return sizeof(zonetool::s2::FxFlareDef) + sizeof(float) * (flare->intensityXIntervalCount + flare->intensityYIntervalCount
					+ flare->srcCosIntensityIntervalCount + flare->srcCosScaleIntervalCount + 4);
			}
			default:
				return 0;
			}
		}

		// matches the stock values, runner and effect reference names do not count
		int total_size(const zonetool::s2::FxEffectDef* asset)
		{
			auto size = sizeof(zonetool::s2::FxEffectDef) + sizeof(zonetool::s2::FxElemDef) * elem_count(asset);
			for (auto i = 0; i < elem_count(asset); i++)
			{
				const auto& elem = asset->elemDefs[i];
				size += elem.velSamples ? sizeof(zonetool::s2::FxElemVelStateSample) * (elem.velIntervalCount + 1) : 0;
				size += elem.visSamples ? sizeof(zonetool::s2::FxElemVisStateSample) * (elem.visStateIntervalCount + 1) : 0;
				size += visuals_size(elem);
				size += extended_size(elem);
				size += string_size(elem.name);
			}

			return static_cast<int>(size);
		}

		unsigned int elem_effect_flags(const zonetool::s2::FxElemDef& elem)
		{
			auto flags = 0u;
			if (elem.flags & zonetool::s2::FX_ELEM_USE_CAST_SHADOW)
			{
				flags |= zonetool::s2::FX_EFFECT_NEEDS_CAST_SHADOW;
			}

			if (elem.flags & zonetool::s2::FX_ELEM_USE_OCCLUSION_QUERY || elem.flags2 & zonetool::s2::FX_ELEM2_USE_EMISSIVE_DRAW)
			{
				flags |= zonetool::s2::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
			}

			if (elem.flags & zonetool::s2::FX_ELEM_USE_VECTORFIELDS)
			{
				flags |= zonetool::s2::FX_EFFECT_USE_VECTORFIELDS;
			}

			if ((elem.flags & zonetool::s2::FX_ELEM_RUN_MASK) == zonetool::s2::FX_ELEM_RUN_RELATIVE_TO_CAMERA)
			{
				flags |= zonetool::s2::FX_EFFECT_HAS_CAMERA_RELATIVE_ELEM;
			}

			if (elem.flags2 & zonetool::s2::FX_ELEM2_REQUIRES_POST_RESOLVE_DRAW)
			{
				flags |= zonetool::s2::FX_EFFECT_NEEDS_POST_RESOLVE_DRAW;
			}

			if (elem.elemType == zonetool::s2::FX_ELEM_TYPE_SPRITE_ROTATED)
			{
				flags |= zonetool::s2::FX_EFFECT_HAS_ROTATED_SPRITE_ELEM;
			}

			return flags;
		}

		// the elem derived bits match every stock S2 effect, the lighting and sun bits come from H1
		unsigned int convert_effect_flags(const FxEffectDef* asset, const zonetool::s2::FxEffectDef* new_asset)
		{
			constexpr auto kept_flags = zonetool::s2::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_AT_SPAWN
				| zonetool::s2::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_PER_FRAME | zonetool::s2::FX_EFFECT_NEEDS_DYNAMIC_LIGHTING_PER_FRAME
				| zonetool::s2::FX_EFFECT_LINK_TO_SUN;

			auto flags = static_cast<unsigned int>(asset->flags) & kept_flags;
			for (auto i = 0; i < elem_count(new_asset); i++)
			{
				flags |= elem_effect_flags(new_asset->elemDefs[i]);
			}

			return flags;
		}

		zonetool::s2::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::FxEffectDef>();
			new_asset->name = asset->name;
			new_asset->msecLoopingLife = asset->msecLoopingLife;
			new_asset->elemMaxRadius = asset->elemMaxRadius;
			new_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			new_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			new_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			new_asset->occlusionQueryScaleRange = convert_range(asset->occlusionQueryScaleRange);

			const auto elems = allocator.allocate_array<zonetool::s2::FxElemDef>(elem_count(asset));
			const auto one_shot_start = asset->elemDefCountLooping;
			const auto emission_start = one_shot_start + asset->elemDefCountOneShot;
			new_asset->elemDefCountLooping = convert_elems(asset, 0, asset->elemDefCountLooping, elems, allocator);
			new_asset->elemDefCountOneShot = convert_elems(asset, one_shot_start, asset->elemDefCountOneShot,
				elems + new_asset->elemDefCountLooping, allocator);
			new_asset->elemDefCountEmission = convert_elems(asset, emission_start, asset->elemDefCountEmission,
				elems + new_asset->elemDefCountLooping + new_asset->elemDefCountOneShot, allocator);
			new_asset->elemDefs = elem_count(new_asset) ? elems : nullptr;

			new_asset->flags = convert_effect_flags(asset, new_asset);
			new_asset->totalSize = total_size(new_asset);
			return new_asset;
		}
	}

	void dump(FxEffectDef* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		zonetool::s2::fx_effect_def::dump(converted);
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_FX), asset->name);
	}
}
