#include <std_include.hpp>

#include "../include.hpp"
#include "fxeffectdef.hpp"
#include "material.hpp"
#include "sound.hpp"
#include "vectorfield.hpp"
#include "xmodel.hpp"

#include <h1/assets/fxeffectdef.hpp>

namespace zonetool::s2::converter::h1::fxeffectdef
{
	namespace
	{
		std::unordered_set<std::string> dumped_effects;

		static_assert(sizeof(FxElemVelStateInFrame) == sizeof(zonetool::h1::FxElemVelStateInFrame));
		static_assert(sizeof(FxTrailDef) == sizeof(zonetool::h1::FxTrailDef));
		static_assert(sizeof(FxSparkFountainDef) == sizeof(zonetool::h1::FxSparkFountainDef));
		static_assert(sizeof(FxFlareDef) == sizeof(zonetool::h1::FxFlareDef));

		template <typename T>
		T* make_stub(const char* name, utils::memory::allocator& allocator)
		{
			const auto stub = allocator.allocate<T>();
			stub->name = name;
			return stub;
		}

		int elem_count(const FxEffectDef* asset)
		{
			return asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
		}

		bool is_dropped(const FxElemType type)
		{
			switch (type)
			{
			case FX_ELEM_TYPE_STRING_15:
			case FX_ELEM_TYPE_STRING_19:
			case FX_ELEM_TYPE_UNK_20:
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				return true;
			default:
				return false;
			}
		}

		zonetool::h1::FxElemType convert_elem_type(const FxElemType type)
		{
			switch (type)
			{
			case FX_ELEM_TYPE_DECAL:
				return zonetool::h1::FX_ELEM_TYPE_DECAL;
			case FX_ELEM_TYPE_RUNNER:
				return zonetool::h1::FX_ELEM_TYPE_RUNNER;
			case FX_ELEM_TYPE_VECTORFIELD:
				return zonetool::h1::FX_ELEM_TYPE_VECTORFIELD;
			default:
				return static_cast<zonetool::h1::FxElemType>(type);
			}
		}

		bool is_sprite(const FxElemType type)
		{
			return type <= FX_ELEM_TYPE_LINE || type == FX_ELEM_TYPE_TRAIL || type == FX_ELEM_TYPE_CLOUD || type == FX_ELEM_TYPE_SPARK_CLOUD
				|| type == FX_ELEM_TYPE_SPARK_FOUNTAIN;
		}

		zonetool::h1::Material* reference_material(Material* material, utils::memory::allocator& allocator)
		{
			if (!material)
			{
				return nullptr;
			}

			material::dump(material);
			return make_stub<zonetool::h1::Material>(material->name, allocator);
		}

		zonetool::h1::FxEffectDef* reference_effect(const FxEffectDefRef& reference, utils::memory::allocator& allocator)
		{
			if (!reference.name || !*reference.name)
			{
				return nullptr;
			}

			if (const auto effect = db_find_x_asset_header_safe(ASSET_TYPE_FX, reference.name).fx)
			{
				dump(effect);
			}
			else
			{
				ZONETOOL_WARNING("Effect \"%s\" is not loaded", reference.name);
			}

			return make_stub<zonetool::h1::FxEffectDef>(reference.name, allocator);
		}

		zonetool::h1::FxElemVisuals convert_visual(const FxElemType type, const FxElemVisuals& visual, utils::memory::allocator& allocator)
		{
			zonetool::h1::FxElemVisuals result{};
			switch (type)
			{
			case FX_ELEM_TYPE_MODEL:
				if (visual.model)
				{
					xmodel::reference(visual.model);
					result.model = make_stub<zonetool::h1::XModel>(visual.model->name, allocator);
				}
				break;
			case FX_ELEM_TYPE_RUNNER:
				result.effectDef.handle = reference_effect(visual.effectDef, allocator);
				break;
			case FX_ELEM_TYPE_SOUND:
				if (visual.soundName && sound::dump_used(visual.soundName))
				{
					result.soundName = visual.soundName;
				}
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				result.vectorFieldName = visual.vectorFieldName;
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
			case FX_ELEM_TYPE_SPOT_LIGHT:
				break;
			default:
				result.material = is_sprite(type) ? reference_material(visual.material, allocator) : nullptr;
				break;
			}

			return result;
		}

		zonetool::h1::FxElemDefVisuals convert_visuals(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			zonetool::h1::FxElemDefVisuals result{};
			if (elem.elemType == FX_ELEM_TYPE_DECAL)
			{
				if (!elem.visuals.markArray)
				{
					return result;
				}

				result.markArray = allocator.allocate_array<zonetool::h1::FxElemMarkVisuals>(elem.visualCount);
				for (auto i = 0; i < elem.visualCount; i++)
				{
					for (auto j = 0u; j < std::size(result.markArray[i].materials); j++)
					{
						result.markArray[i].materials[j] = reference_material(elem.visuals.markArray[i].materials[j], allocator);
					}
				}
			}
			else if (elem.visualCount > 1)
			{
				if (!elem.visuals.array)
				{
					return result;
				}

				result.array = allocator.allocate_array<zonetool::h1::FxElemVisuals>(elem.visualCount);
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

		zonetool::h1::FxElemVelStateSample* convert_vel_samples(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			if (!elem.velSamples)
			{
				return nullptr;
			}

			const auto count = elem.velIntervalCount + 1;
			const auto samples = allocator.allocate_array<zonetool::h1::FxElemVelStateSample>(count);
			for (auto i = 0; i < count; i++)
			{
				std::memcpy(&samples[i].local, &elem.velSamples[i].local, sizeof(samples[i].local));
				std::memcpy(&samples[i].world, &elem.velSamples[i].world, sizeof(samples[i].world));
			}

			return samples;
		}

		zonetool::h1::FxElemVisualState convert_visual_state(const FxElemVisualState& state)
		{
			zonetool::h1::FxElemVisualState result{};
			std::memcpy(result.color, state.color, sizeof(result.color));
			std::memcpy(result.emissiveScale, state.emissiveScale, sizeof(result.emissiveScale));
			result.rotationDelta = state.rotationDelta;
			result.rotationTotal = state.rotationTotal;
			std::memcpy(result.size, state.size, sizeof(result.size));
			result.scale = state.scale;
			std::memcpy(result.pivot, state.pivot, sizeof(result.pivot));
			return result;
		}

		zonetool::h1::FxElemVisStateSample* convert_vis_samples(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			if (!elem.visSamples)
			{
				return nullptr;
			}

			const auto count = elem.visStateIntervalCount + 1;
			const auto samples = allocator.allocate_array<zonetool::h1::FxElemVisStateSample>(count);
			for (auto i = 0; i < count; i++)
			{
				samples[i].base = convert_visual_state(elem.visSamples[i].base);
				samples[i].amplitude = convert_visual_state(elem.visSamples[i].amplitude);
			}

			return samples;
		}

		zonetool::h1::FxSpotLightDef* convert_spot_light(const FxSpotLightDef* light, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<zonetool::h1::FxSpotLightDef>();
			result->halfFovOuter = light->halfFovOuter;
			result->halfFovInner = light->halfFovInner;
			result->radius = light->radius;
			result->brightness = light->brightness;
			result->maxLength = light->maxLength;
			result->exponent = light->exponent;
			result->nearClip = light->nearClip;
			result->bulbRadius = light->bulbRadius;
			result->bulbLength = light->bulbLength;
			result->fadeOffsetRt[0] = light->fadeOffsetRt[0];
			result->fadeOffsetRt[1] = light->fadeOffsetRt[1];
			return result;
		}

		zonetool::h1::FxOmniLightDef* convert_omni_light(const FxOmniLightDef* light, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<zonetool::h1::FxOmniLightDef>();
			result->bulbRadius = light->bulbRadius;
			result->bulbLength = light->bulbLength;
			result->fadeOffsetRt[0] = light->fadeOffsetRt[0];
			result->fadeOffsetRt[1] = light->fadeOffsetRt[1];
			return result;
		}

		zonetool::h1::FxElemExtendedDefPtr convert_extended(const FxElemDef& elem, utils::memory::allocator& allocator)
		{
			zonetool::h1::FxElemExtendedDefPtr result{};
			if (!elem.extended.unknownDef)
			{
				return result;
			}

			switch (elem.elemType)
			{
			case FX_ELEM_TYPE_TRAIL:
				result.trailDef = reinterpret_cast<zonetool::h1::FxTrailDef*>(elem.extended.trailDef);
				break;
			case FX_ELEM_TYPE_SPARK_FOUNTAIN:
				result.sparkFountainDef = reinterpret_cast<zonetool::h1::FxSparkFountainDef*>(elem.extended.sparkFountainDef);
				break;
			case FX_ELEM_TYPE_SPOT_LIGHT:
				result.spotLightDef = convert_spot_light(elem.extended.spotLightDef, allocator);
				break;
			case FX_ELEM_TYPE_OMNI_LIGHT:
				result.omniLightDef = convert_omni_light(elem.extended.omniLightDef, allocator);
				break;
			case FX_ELEM_TYPE_FLARE:
				result.flareDef = reinterpret_cast<zonetool::h1::FxFlareDef*>(elem.extended.flareDef);
				break;
			default:
				break;
			}

			return result;
		}

		zonetool::h1::FxFloatRange convert_range(const FxFloatRange& range)
		{
			return {range.base, range.amplitude};
		}

		void convert_ranges(const FxFloatRange* ranges, zonetool::h1::FxFloatRange* result, const std::size_t count)
		{
			for (auto i = 0u; i < count; i++)
			{
				result[i] = convert_range(ranges[i]);
			}
		}

		void convert_elem(const FxElemDef& elem, zonetool::h1::FxElemDef& result, utils::memory::allocator& allocator)
		{
			result.flags = static_cast<zonetool::h1::FxElemDefFlags>(elem.flags);
			result.flags2 = static_cast<int>(elem.flags2 & FX_ELEM2_H1_MASK);
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
			std::memcpy(&result.atlas, &elem.atlas, sizeof(result.atlas));
			result.elemType = convert_elem_type(elem.elemType);
			result.elemLitType = static_cast<zonetool::h1::FxElemLitType>(elem.elemLitType);
			result.visualCount = elem.visualCount;
			result.velIntervalCount = elem.velIntervalCount;
			result.visStateIntervalCount = elem.visStateIntervalCount;
			result.velSamples = convert_vel_samples(elem, allocator);
			result.visSamples = convert_vis_samples(elem, allocator);
			result.visuals = convert_visuals(elem, allocator);
			std::memcpy(&result.collBounds, &elem.collBounds, sizeof(result.collBounds));
			result.effectOnImpact.handle = reference_effect(elem.effectOnImpact, allocator);
			result.effectOnDeath.handle = reference_effect(elem.effectOnDeath, allocator);
			result.effectEmitted.handle = reference_effect(elem.effectEmitted, allocator);
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
		}

		bool has_vector_field(const FxElemDef& elem)
		{
			if (elem.visualCount > 1)
			{
				return std::all_of(elem.visuals.array, elem.visuals.array + elem.visualCount, [](const FxElemVisuals& visual)
				{
					return visual.vectorFieldName && vectorfield::dump_used(visual.vectorFieldName);
				});
			}

			return elem.visuals.instance.vectorFieldName && vectorfield::dump_used(elem.visuals.instance.vectorFieldName);
		}

		int convert_elems(const FxEffectDef* asset, const int first, const int count, std::vector<zonetool::h1::FxElemDef>& result,
			utils::memory::allocator& allocator)
		{
			auto converted = 0;
			for (auto i = 0; i < count; i++)
			{
				const auto& elem = asset->elemDefs[first + i];
				if (is_dropped(elem.elemType))
				{
					continue;
				}

				if (elem.elemType == FX_ELEM_TYPE_VECTORFIELD && !has_vector_field(elem))
				{
					continue;
				}

				convert_elem(elem, result.emplace_back(), allocator);
				converted++;
			}

			return converted;
		}

		std::size_t string_size(const char* str)
		{
			return str ? std::strlen(str) + 1 : 0;
		}

		std::size_t visuals_size(const zonetool::h1::FxElemDef& elem)
		{
			if (elem.elemType == zonetool::h1::FX_ELEM_TYPE_DECAL)
			{
				return elem.visuals.markArray ? sizeof(zonetool::h1::FxElemMarkVisuals) * elem.visualCount : 0;
			}

			return elem.visualCount > 1 && elem.visuals.array ? sizeof(zonetool::h1::FxElemVisuals) * elem.visualCount : 0;
		}

		std::size_t extended_size(const zonetool::h1::FxElemDef& elem)
		{
			const auto& extended = elem.extended;
			if (!extended.unknownDef)
			{
				return 0;
			}

			switch (elem.elemType)
			{
			case zonetool::h1::FX_ELEM_TYPE_TRAIL:
				return sizeof(zonetool::h1::FxTrailDef)
					+ (extended.trailDef->verts ? sizeof(zonetool::h1::FxTrailVertex) * extended.trailDef->vertCount : 0)
					+ (extended.trailDef->inds ? sizeof(unsigned short) * extended.trailDef->indCount : 0);
			case zonetool::h1::FX_ELEM_TYPE_SPARK_FOUNTAIN:
				return sizeof(zonetool::h1::FxSparkFountainDef);
			case zonetool::h1::FX_ELEM_TYPE_SPOT_LIGHT:
				return sizeof(zonetool::h1::FxSpotLightDef);
			case zonetool::h1::FX_ELEM_TYPE_OMNI_LIGHT:
				return sizeof(zonetool::h1::FxOmniLightDef);
			case zonetool::h1::FX_ELEM_TYPE_FLARE:
			{
				const auto* flare = extended.flareDef;
				return sizeof(zonetool::h1::FxFlareDef) + sizeof(float) * (flare->intensityXIntervalCount + flare->intensityYIntervalCount
					+ flare->srcCosIntensityIntervalCount + flare->srcCosScaleIntervalCount + 4);
			}
			default:
				return 0;
			}
		}

		int total_size(const zonetool::h1::FxEffectDef* asset, const int count)
		{
			auto size = sizeof(zonetool::h1::FxEffectDef) + sizeof(zonetool::h1::FxElemDef) * count + string_size(asset->name);
			for (auto i = 0; i < count; i++)
			{
				const auto& elem = asset->elemDefs[i];
				size += elem.velSamples ? sizeof(zonetool::h1::FxElemVelStateSample) * (elem.velIntervalCount + 1) : 0;
				size += elem.visSamples ? sizeof(zonetool::h1::FxElemVisStateSample) * (elem.visStateIntervalCount + 1) : 0;
				size += visuals_size(elem) + extended_size(elem);
			}

			return static_cast<int>(size);
		}

		unsigned int elem_effect_flags(const zonetool::h1::FxElemDef& elem)
		{
			auto flags = 0u;
			if ((elem.flags & zonetool::h1::FX_ELEM_RUN_MASK) == zonetool::h1::FX_ELEM_RUN_RELATIVE_TO_CAMERA)
			{
				flags |= zonetool::h1::FX_EFFECT_HAS_CAMERA_RELATIVE_ELEM;
			}

			if (elem.flags & zonetool::h1::FX_ELEM_USE_VECTORFIELDS)
			{
				flags |= zonetool::h1::FX_EFFECT_USE_VECTORFIELDS;
			}

			if (elem.flags & zonetool::h1::FX_ELEM_USE_CAST_SHADOW)
			{
				flags |= zonetool::h1::FX_EFFECT_NEEDS_CAST_SHADOW;
			}

			if (elem.flags & zonetool::h1::FX_ELEM_USE_OCCLUSION_QUERY || elem.flags2 & zonetool::h1::FX_ELEM2_USE_EMISSIVE_DRAW)
			{
				flags |= zonetool::h1::FX_EFFECT_NEEDS_EMISSIVE_DRAW;
			}

			return flags;
		}

		zonetool::h1::FxEffectDef* convert(FxEffectDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::h1::FxEffectDef>();
			new_asset->name = asset->name;
			new_asset->msecLoopingLife = asset->msecLoopingLife;
			new_asset->elemMaxRadius = asset->elemMaxRadius;
			new_asset->occlusionQueryDepthBias = asset->occlusionQueryDepthBias;
			new_asset->occlusionQueryFadeIn = asset->occlusionQueryFadeIn;
			new_asset->occlusionQueryFadeOut = asset->occlusionQueryFadeOut;
			new_asset->occlusionQueryScaleRange = convert_range(asset->occlusionQueryScaleRange);

			std::vector<zonetool::h1::FxElemDef> elems;
			elems.reserve(elem_count(asset));
			const auto one_shot_start = asset->elemDefCountLooping;
			const auto emission_start = one_shot_start + asset->elemDefCountOneShot;
			new_asset->elemDefCountLooping = convert_elems(asset, 0, asset->elemDefCountLooping, elems, allocator);
			new_asset->elemDefCountOneShot = convert_elems(asset, one_shot_start, asset->elemDefCountOneShot, elems, allocator);
			new_asset->elemDefCountEmission = convert_elems(asset, emission_start, asset->elemDefCountEmission, elems, allocator);

			const auto count = static_cast<int>(elems.size());
			new_asset->elemDefs = count ? allocator.allocate_array<zonetool::h1::FxElemDef>(count) : nullptr;
			std::ranges::copy(elems, new_asset->elemDefs);

			auto flags = asset->flags & (zonetool::h1::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_AT_SPAWN | zonetool::h1::FX_EFFECT_NEEDS_LIGHT_GRID_LIGHTING_PER_FRAME
				| zonetool::h1::FX_EFFECT_NEEDS_DYNAMIC_LIGHTING_PER_FRAME | zonetool::h1::FX_EFFECT_LINK_TO_SUN);
			for (auto i = 0; i < count; i++)
			{
				flags |= elem_effect_flags(new_asset->elemDefs[i]);
			}

			new_asset->flags = static_cast<int>(flags);
			new_asset->totalSize = total_size(new_asset, count);

			if (const auto dropped = elem_count(asset) - count)
			{
				ZONETOOL_INFO("Effect \"%s\": dropped %d elems that h1 has no type for", asset->name, dropped);
			}

			return new_asset;
		}
	}

	void dump(FxEffectDef* asset)
	{
		if (!dumped_effects.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		zonetool::h1::fx_effect_def::dump(converted);
		add_csv_row("fx", asset->name);
	}
}
