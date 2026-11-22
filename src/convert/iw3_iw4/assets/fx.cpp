#include <std_include.hpp>

#include "../include.hpp"
#include "fx.hpp"

namespace zonetool::iw3::converter::iw4
{
	namespace fx
	{
		namespace
		{
			zonetool::iw4::Material* material_reference(const zonetool::iw3::Material* material, x86::allocator& allocator)
			{
				if (!material)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw4::Material>();
				reference->name.value = material->info.name.value;
				return reference;
			}

			zonetool::iw4::XModel* model_reference(const zonetool::iw3::XModel* model, x86::allocator& allocator)
			{
				if (!model)
				{
					return nullptr;
				}

				const auto reference = allocator.allocate<zonetool::iw4::XModel>();
				reference->name.value = model->name.value;
				return reference;
			}

			void convert_visuals(const zonetool::iw3::FxElemDef* element, const zonetool::iw3::FxElemVisuals& source,
				zonetool::iw4::FxElemVisuals& destination, x86::allocator& allocator)
			{
				switch (element->elemType)
				{
				case zonetool::iw3::FX_ELEM_TYPE_SPRITE_BILLBOARD:
				case zonetool::iw3::FX_ELEM_TYPE_SPRITE_ORIENTED:
				case zonetool::iw3::FX_ELEM_TYPE_TAIL:
				case zonetool::iw3::FX_ELEM_TYPE_TRAIL:
				case zonetool::iw3::FX_ELEM_TYPE_CLOUD:
					destination.material = material_reference(source.material.get(), allocator);
					break;
				case zonetool::iw3::FX_ELEM_TYPE_MODEL:
					destination.xmodel = model_reference(source.model.get(), allocator);
					break;
				default:
					destination.anonymous.value = source.anonymous.value; // runner effect and sound names stay raw offsets
					break;
				}
			}

			void convert_element(const zonetool::iw3::FxElemDef* element, zonetool::iw4::FxElemDef* result, x86::allocator& allocator)
			{
				constexpr auto prefix = offsetof(zonetool::iw3::FxElemDef, collMins);
				static_assert(offsetof(zonetool::iw3::FxElemDef, collMins) == offsetof(zonetool::iw4::FxElemDef, collBounds));
				std::memcpy(result, element, prefix);

				if (result->elemType >= 5) // IW4 inserted spark cloud and spark fountain before model
				{
					result->elemType += 2;
				}

				for (auto i = 0; i < 3; i++)
				{
					result->collBounds.midPoint[i] = (element->collMins[i] + element->collMaxs[i]) * 0.5f;
					result->collBounds.halfSize[i] = (element->collMaxs[i] - element->collMins[i]) * 0.5f;
				}

				result->effectOnImpact.name.value = element->effectOnImpact.name.value;
				result->effectOnDeath.name.value = element->effectOnDeath.name.value;
				result->effectEmitted.name.value = element->effectEmitted.name.value;
				std::memcpy(&result->emitDist, &element->emitDist, sizeof(result->emitDist));
				std::memcpy(&result->emitDistVariance, &element->emitDistVariance, sizeof(result->emitDistVariance));
				result->sortOrder = element->sortOrder;
				result->lightingFrac = element->lightingFrac;
				result->useItemClip = element->useItemClip;

				if (const auto velocity = element->velSamples.get())
				{
					const auto count = element->velIntervalCount + 1;
					const auto copy = allocator.allocate<zonetool::iw4::FxElemVelStateSample>(count);
					static_assert(sizeof(zonetool::iw4::FxElemVelStateSample) == sizeof(zonetool::iw3::FxElemVelStateSample));
					std::memcpy(copy, velocity, sizeof(zonetool::iw4::FxElemVelStateSample) * count);
					result->velSamples = copy;
				}

				if (const auto visual = element->visSamples.get())
				{
					const auto count = element->visStateIntervalCount + 1;
					const auto copy = allocator.allocate<zonetool::iw4::FxElemVisStateSample>(count);
					static_assert(sizeof(zonetool::iw4::FxElemVisStateSample) == sizeof(zonetool::iw3::FxElemVisStateSample));
					std::memcpy(copy, visual, sizeof(zonetool::iw4::FxElemVisStateSample) * count);

					if (element->elemType == zonetool::iw3::FX_ELEM_TYPE_OMNI_LIGHT || element->elemType == zonetool::iw3::FX_ELEM_TYPE_SPOT_LIGHT)
					{
						for (auto i = 0; i < count; i++)
						{
							copy[i].base.scale = 1.0f; // IW4 lights read their scale
							copy[i].amplitude.scale = 1.0f;
						}
					}

					result->visSamples = copy;
				}

				if (element->elemType == zonetool::iw3::FX_ELEM_TYPE_DECAL)
				{
					if (const auto marks = element->visuals.markArray.get())
					{
						const auto copy = allocator.allocate<zonetool::iw4::FxElemMarkVisuals>(element->visualCount);
						for (auto i = 0; i < element->visualCount; i++)
						{
							copy[i].materials[0] = material_reference(marks[i].materials[0].get(), allocator);
							copy[i].materials[1] = material_reference(marks[i].materials[1].get(), allocator);
						}
						result->visuals.markArray = copy;
					}
				}
				else if (element->visualCount > 1)
				{
					if (const auto array = element->visuals.array.get())
					{
						const auto copy = allocator.allocate<zonetool::iw4::FxElemVisuals>(element->visualCount);
						for (auto i = 0; i < element->visualCount; i++)
						{
							convert_visuals(element, array[i], copy[i], allocator);
						}
						result->visuals.array = copy;
					}
				}
				else
				{
					convert_visuals(element, element->visuals.instance, result->visuals.instance, allocator);
				}

				if (element->elemType == zonetool::iw3::FX_ELEM_TYPE_TRAIL)
				{
					if (const auto trail = element->trailDef.get())
					{
						const auto copy = allocator.allocate<zonetool::iw4::FxTrailDef>();
						copy->scrollTimeMsec = trail->scrollTimeMsec;
						copy->repeatDist = trail->repeatDist;
						copy->invSplitDist = trail->splitDist ? 1.0f / static_cast<float>(trail->splitDist) : 0.0f; // arc and time factors are not stored in IW3 (left 0)
						copy->vertCount = trail->vertCount;
						copy->indCount = trail->indCount;

						const auto vertices = allocator.allocate<zonetool::iw4::FxTrailVertex>(trail->vertCount);
						std::memcpy(vertices, trail->verts.get(), sizeof(zonetool::iw4::FxTrailVertex) * trail->vertCount);
						copy->verts = vertices;

						const auto indices = allocator.allocate<unsigned short>(trail->indCount);
						std::memcpy(indices, trail->inds.get(), sizeof(unsigned short) * trail->indCount);
						copy->inds = indices;

						result->extended.trailDef = copy;
					}
				}
			}
		}

		zonetool::iw4::FxEffectDef* convert(zonetool::iw3::FxEffectDef* asset, x86::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw4::FxEffectDef>();

			COPY_POINTER_RAW(name);
			COPY_VALUE(flags);
			COPY_VALUE(totalSize);
			COPY_VALUE(msecLoopingLife);
			COPY_VALUE(elemDefCountLooping);
			COPY_VALUE(elemDefCountOneShot);
			COPY_VALUE(elemDefCountEmission);

			const auto count = asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;
			const auto elements = allocator.allocate<zonetool::iw4::FxElemDef>(count);
			for (auto i = 0; i < count; i++)
			{
				convert_element(&asset->elemDefs.get()[i], &elements[i], allocator);
			}
			new_asset->elemDefs = elements;

			return new_asset;
		}
	}
}
