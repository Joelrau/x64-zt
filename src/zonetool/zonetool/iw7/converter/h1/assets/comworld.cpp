#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "comworld.hpp"

#include "zonetool/h1/assets/comworld.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace comworld
		{
			zonetool::h1::ComWorld* convert(ComWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::ComWorld>();

				REINTERPRET_CAST_SAFE(name);
				COPY_VALUE(isInUse);
				COPY_VALUE(primaryLightCount);

				new_asset->primaryLights = allocator.allocate_array<zonetool::h1::ComPrimaryLight>(asset->primaryLightCount);
				for (unsigned int i = 0; i < asset->primaryLightCount; i++)
				{
					const auto* src = &asset->primaryLights[i];
					auto* dst = &new_asset->primaryLights[i];

					// IW7 lights are already physically based (UNTESTED mapping: colors copied as-is)
					dst->type = static_cast<zonetool::h1::GfxLightType>(src->type);
					dst->canUseShadowMap = src->canUseShadowMap;
					dst->physicallyBased = 1;
					dst->exponent = src->exponent;
					dst->lightingState = 0;
					std::memcpy(dst->color, src->color, sizeof(dst->color));
					std::memcpy(dst->dir, src->dir, sizeof(dst->dir));
					std::memcpy(dst->up, src->up, sizeof(dst->up));
					std::memcpy(dst->origin, src->origin, sizeof(dst->origin));
					dst->fadeOffset[0] = src->fadeOffsetRt[0];
					dst->fadeOffset[1] = src->fadeOffsetRt[1];
					dst->bulbRadius = src->bulbRadius;
					std::memcpy(dst->bulbLength, src->bulbLength, sizeof(dst->bulbLength));
					dst->radius = src->radius;
					dst->cosHalfFovOuter = src->cosHalfFovOuter;
					dst->cosHalfFovInner = src->cosHalfFovInner;
					dst->cosHalfFovExpanded = src->cosHalfFovOuter;
					dst->rotationLimit = src->rotationLimit;
					dst->translationLimit = src->translationLimit;
					dst->cucRotationOffsetRad = 0.0f;
					dst->cucRotationSpeedRad = 0.0f;
					dst->cucScrollVector[0] = dst->cucScrollVector[1] = 0.0f;
					dst->cucScaleVector[0] = dst->cucScaleVector[1] = 1.0f;
					dst->cucTransVector[0] = dst->cucTransVector[1] = 0.0f;
					dst->defName = src->defName;
				}

				// v121 bake: export IW7's primary lights (most are forward+ real-time lights H1 cannot draw) for the offline lightmap baker
				{
					std::string buf;
					const auto put = [&](const void* p, std::size_t n) { buf.append(static_cast<const char*>(p), n); };
					const unsigned int count = asset->primaryLightCount;
					put("LT21", 4);
					put(&count, 4);
					for (unsigned int i = 0; i < count; i++)
					{
						const auto& l = asset->primaryLights[i];
						const unsigned int type = static_cast<unsigned int>(l.type);
						const float ex = static_cast<float>(l.exponent);
						put(&type, 4); put(l.color, 12); put(l.dir, 12); put(l.origin, 12);
						put(&l.radius, 4); put(&l.cosHalfFovOuter, 4); put(&l.cosHalfFovInner, 4); put(&ex, 4);
						put(&l.distanceFalloff, 4); put(&l.uvIntensity, 4); put(&l.irIntensity, 4); put(&l.bulbRadius, 4);
					}
					filesystem::file file("v121_lights.bin");
					file.open("wb");
					file.write(buf);
					file.close();
					ZONETOOL_INFO("v121 bake: %u lights exported", count);
				}

				// v62: IW7 keeps light 0 empty and puts the sun at 1/2, H1 treats light 0 as the sun -> the port had no sun colour at all (cold ambient only look)
				if (asset->primaryLightCount > 1 && new_asset->primaryLights[0].type == static_cast<zonetool::h1::GfxLightType>(0))
				{
					for (unsigned int i = 1; i < asset->primaryLightCount; i++)
					{
						if (static_cast<int>(new_asset->primaryLights[i].type) == 1)
						{
							new_asset->primaryLights[0] = new_asset->primaryLights[i];
							break;
						}
					}
				}

				// v81: Ghosts/H1 suns are non-physical (mp_dig: 10,8.8,6.2). IW7 marks them physicallyBased with ~3.5 peak -> dim, overly orange sun.
				for (unsigned int i = 0; i < asset->primaryLightCount; i++)
				{
					auto& l = new_asset->primaryLights[i];
					if (static_cast<int>(l.type) != 1) continue;
					l.physicallyBased = 0;
					// v102 shadows: v100 forced canUseShadowMap=1 but the working Ghosts ports keep 0 on their sun (mp_dig light 1: type 1,
					//      canUseShadowMap 0, physicallyBased 0); the flag selects shadow-mapped spot/omni variants, not sun cascades. Match them.
					ZONETOOL_INFO("v102 shadows: sun light %u canUseShadowMap was %u -> 0, lightingState %u", i, (unsigned)l.canUseShadowMap, (unsigned)l.lightingState);
					l.canUseShadowMap = 0;
					for (int c = 0; c < 3; c++) l.color[c] *= 3.4f; // v99: 2.5 left sunlit marble ~30% darker than the IW7 reference frame
					// v96: IW7's sun hue is graded by its own tonemapper; raw in H1 it tinted the cream marble brown/orange.
					//      Pull 20% toward neutral at the same luminance (IW7 reference frame shows near-white sunlit stone).
					const float lum = l.color[0] * 0.3f + l.color[1] * 0.59f + l.color[2] * 0.11f;
					for (int c = 0; c < 3; c++) l.color[c] = l.color[c] * 0.8f + lum * 0.2f;
					ZONETOOL_INFO("v96 sun color %g %g %g dir %g %g %g", l.color[0], l.color[1], l.color[2], l.dir[0], l.dir[1], l.dir[2]);
				}

				COPY_VALUE(primaryLightEnvCount);
				REINTERPRET_CAST_SAFE(primaryLightEnvs);
				// v102 shadows: world surfaces use env 1; log which lights it holds (must be the sun for sun shadows)
				for (unsigned int e = 0; e < 3 && e < new_asset->primaryLightEnvCount && new_asset->primaryLightEnvs; e++)
				{
					const auto& env = new_asset->primaryLightEnvs[e];
					const unsigned li = env.numIndices ? env.primaryLightIndices[0] : 0;
					ZONETOOL_INFO("v102 shadows: env %u numIndices %u first light %u type %u", e, (unsigned)env.numIndices, li,
						li < new_asset->primaryLightCount ? (unsigned)new_asset->primaryLights[li].type : 255u);
				}

				return new_asset;
			}

			void dump(ComWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::com_world::dump(converted_asset);
			}
		}
	}
}
