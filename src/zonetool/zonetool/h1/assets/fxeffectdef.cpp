#include <std_include.hpp>
#include "fxeffectdef.hpp"

//#define DUMP_JSON

namespace zonetool::h1
{
	namespace fx::json
	{
		const std::string FX_ELEM_TYPE_s[] =
		{
			"FX_ELEM_TYPE_SPRITE_BILLBOARD",
			"FX_ELEM_TYPE_SPRITE_ORIENTED",
			"FX_ELEM_TYPE_SPRITE_ROTATED",
			"FX_ELEM_TYPE_TAIL",
			"FX_ELEM_TYPE_LINE",
			"FX_ELEM_TYPE_TRAIL",
			"FX_ELEM_TYPE_FLARE",
			"FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION",
			"FX_ELEM_TYPE_CLOUD",
			"FX_ELEM_TYPE_SPARK_CLOUD",
			"FX_ELEM_TYPE_SPARK_FOUNTAIN",
			"FX_ELEM_TYPE_MODEL",
			"FX_ELEM_TYPE_OMNI_LIGHT",
			"FX_ELEM_TYPE_SPOT_LIGHT",
			"FX_ELEM_TYPE_SOUND",
			"FX_ELEM_TYPE_DECAL",
			"FX_ELEM_TYPE_RUNNER",
			"FX_ELEM_TYPE_VECTORFIELD",
		};

		const std::string FX_ELEM_LIT_TYPE_s[] =
		{
			"FX_ELEM_LIT_TYPE_NONE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_SPAWN_SINGLE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SINGLE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_SPRITE",
			"FX_ELEM_LIT_TYPE_LIGHTGRID_FRAME_VERTEX",
		};

		enum elem_type_e
		{
			elem_type_looping,
			elem_type_oneshot,
			elem_type_emission,
			elem_type_error,
		};

		const std::string elem_type_s[] =
		{
			"looping",
			"oneshot",
			"emission"
		};

		template<typename T> T get_value_index(const std::string& value, const std::string* lookup_table, size_t len, T default_value = 0)
		{
			if (value.empty())
				return default_value;

			for (size_t i = 0; i < len; i++)
			{
				if (lookup_table[i] == value)
					return static_cast<T>(i);
			}

			return default_value;
		}

#define MODVAL(__field__, __size__) \
		if (__size__) assert(__size__ == static_cast<decltype(__size__)>(data[#__field__].size())); \
		__size__ = static_cast<decltype(__size__)>(data[#__field__].size());

#define PARSE_FIELD_ALLOC_ARR(__field__, __size__) \
	if(!data[#__field__].is_null()) \
	{ \
		asset->__field__ = mem->allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(__size__); \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
		} \
	} \
	else \
	{ \
		asset->__field__ = nullptr; \
	}
#define PARSE_FIELD_ALLOC_ARR_MODVAL(__field__, __size__) \
	MODVAL(__field__, __size__) \
	PARSE_FIELD_ALLOC_ARR(__field__, __size__)

#define PARSE_STRING(__field__) \
		static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
		!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty() ? asset->__field__ = mem->duplicate_string(data[#__field__].get<std::string>()) : asset->__field__ = nullptr;

#define PARSE_FIELD(__field__) \
		if (!data[#__field__].is_null()) asset->__field__ = data[#__field__].get<decltype(asset->__field__)>();

#define PARSE_FIELD_ARR(__field__, __size__) \
		if(!data[#__field__].is_null()) \
		{ \
			for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
			{ \
				asset->__field__[idx##__field__] = data[#__field__][idx##__field__].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__])>::type>(); \
			} \
		}

#define PARSE_ASSET(__field__) \
		if (!data[#__field__].is_null() && !data[#__field__].get<std::string>().empty()) \
		{ \
			asset->__field__ = mem->manual_allocate<typename std::remove_reference<decltype(*asset->__field__)>::type>(sizeof(const char*)); \
			asset->__field__->name = mem->duplicate_string(data[#__field__].get<std::string>()); \
		} \
		else \
		{ \
			asset->__field__ = nullptr; \
		}

#define PARSE_RANGE(__field__) \
		asset->__field__.base = data[#__field__]["base"].get<typename std::remove_reference<decltype(asset->__field__.base)>::type>(); \
		asset->__field__.amplitude = data[#__field__]["amplitude"].get<typename std::remove_reference<decltype(asset->__field__.amplitude)>::type>();

#define PARSE_RANGE_ARR(__field__, __count__) \
		for(auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__count__; idx##__field__++) \
		{ \
			asset->__field__[idx##__field__].base = data[#__field__][idx##__field__]["base"].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__].base)>::type>(); \
			asset->__field__[idx##__field__].amplitude = data[#__field__][idx##__field__]["amplitude"].get<typename std::remove_reference<decltype(asset->__field__[idx##__field__].amplitude)>::type>(); \
		}

#define PARSE_ELEM_TYPE() \
		asset->elemType = get_value_index<FxElemType>(data["elemType"].get<std::string>(), FX_ELEM_TYPE_s, std::extent_v<decltype(FX_ELEM_TYPE_s)>, FX_ELEM_TYPE_SPRITE_BILLBOARD);

#define PARSE_ELEM_LIT_TYPE() \
		asset->elemLitType = get_value_index<FxElemLitType>(data["elemLitType"].get<std::string>(), FX_ELEM_LIT_TYPE_s, std::extent_v<decltype(FX_ELEM_LIT_TYPE_s)>, FX_ELEM_LIT_TYPE_NONE);

#define PARSE_BOUNDS(__field__) \
		for(auto i = 0; i < 3; i++) asset->__field__.midPoint[i] = data[#__field__]["midPoint"][i].get<float>(); \
		for(auto i = 0; i < 3; i++) asset->__field__.halfSize[i] = data[#__field__]["halfSize"][i].get<float>();

		void parse_visuals(FxElemVisuals* asset, FxElemDef* def, ordered_json& data, zone_memory* mem)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				PARSE_ASSET(model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				PARSE_ASSET(effectDef.handle);
				break;
			case FX_ELEM_TYPE_SOUND:
				PARSE_STRING(soundName);
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				PARSE_STRING(vectorFieldName);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				PARSE_ASSET(particleSimAnimation);
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						PARSE_ASSET(lightDef);
					}
				}
				else
				{
					PARSE_ASSET(material);
				}
				break;
			}
		}

		void parse_vec3_range(FxElemVec3Range* asset, ordered_json& data)
		{
			for (auto i = 0; i < 3; i++) asset->base[i] = data["base"][i];
			for (auto i = 0; i < 3; i++) asset->amplitude[i] = data["amplitude"][i];
		}

		void parse_vel_state_sample(FxElemVelStateSample* asset, ordered_json& data)
		{
			parse_vec3_range(&asset->local.velocity, data["local"]["velocity"]);
			parse_vec3_range(&asset->local.totalDelta, data["local"]["totalDelta"]);
			parse_vec3_range(&asset->world.velocity, data["world"]["velocity"]);
			parse_vec3_range(&asset->world.totalDelta, data["world"]["totalDelta"]);
		}

		void parse_visual_state(FxElemVisualState* asset, ordered_json& data)
		{
			PARSE_FIELD_ARR(color, 4);
			PARSE_FIELD_ARR(emissiveScale, 3);
			PARSE_FIELD(rotationDelta);
			PARSE_FIELD(rotationTotal);
			PARSE_FIELD_ARR(size, 2);
			PARSE_FIELD(scale);
			PARSE_FIELD_ARR(pivot, 2);
		}

		void parse_vis_sample_state(FxElemVisStateSample* asset, ordered_json& data)
		{
			parse_visual_state(&asset->base, data["base"]);
			parse_visual_state(&asset->amplitude, data["amplitude"]);
		}

		void parse_trail_vertex(FxTrailVertex* asset, ordered_json& data)
		{
			PARSE_FIELD_ARR(pos, 2);
			PARSE_FIELD_ARR(normal, 2);
			PARSE_FIELD_ARR(texCoord, 2);
			PARSE_FIELD_ARR(radialNormal, 2);
		}

		void parse_trail(FxTrailDef* asset, ordered_json& data, zone_memory* mem)
		{
			PARSE_FIELD(scrollTimeMsec);
			PARSE_FIELD(repeatDist);
			PARSE_FIELD(invSplitDist);
			PARSE_FIELD(invSplitArcDist);
			PARSE_FIELD(invSplitTime);
			PARSE_FIELD(headFadingFactor);
			PARSE_FIELD(tailFadingFactor);

			// vertCount
			asset->vertCount = static_cast<int>(data["verts"].size());
			asset->verts = mem->allocate<FxTrailVertex>(asset->vertCount);
			for (auto i = 0; i < asset->vertCount; i++)
			{
				parse_trail_vertex(&asset->verts[i], data["verts"][i]);
			}

			// indCount
			PARSE_FIELD_ALLOC_ARR_MODVAL(inds, asset->indCount); // alloc
		}

		void parse_spark_fountain(FxSparkFountainDef* asset, ordered_json& data)
		{
			PARSE_FIELD(gravity);
			PARSE_FIELD(bounceFrac);
			PARSE_FIELD(bounceRand);
			PARSE_FIELD(sparkSpacing);
			PARSE_FIELD(sparkLength);
			PARSE_FIELD(sparkCount);
			PARSE_FIELD(loopTime);
			PARSE_FIELD(velMin);
			PARSE_FIELD(velMax);
			PARSE_FIELD(velConeFrac);
			PARSE_FIELD(restSpeed);
			PARSE_FIELD(boostTime);
			PARSE_FIELD(boostFactor);
		}

		void parse_spot_light(FxSpotLightDef* asset, ordered_json& data)
		{
			PARSE_FIELD(halfFovOuter);
			PARSE_FIELD(halfFovInner);
			PARSE_FIELD(radius);
			PARSE_FIELD(brightness);
			PARSE_FIELD(maxLength);
			PARSE_FIELD(exponent);
			PARSE_FIELD(nearClip);
			PARSE_FIELD(bulbRadius);
			PARSE_FIELD(bulbLength);
			PARSE_FIELD_ARR(fadeOffsetRt, 2);
			PARSE_FIELD(unk1);
			PARSE_FIELD(opl);
			PARSE_FIELD(unk2);
			PARSE_FIELD(unused);
		}

		void parse_omni_light(FxOmniLightDef* asset, ordered_json& data)
		{
			PARSE_FIELD(bulbRadius);
			PARSE_FIELD(bulbLength);
			PARSE_FIELD_ARR(fadeOffsetRt, 2);
		}

		void parse_flare(FxFlareDef* asset, ordered_json& data, zone_memory* mem)
		{
			PARSE_FIELD(position);
			PARSE_FIELD(angularRotCount);
			PARSE_FIELD(flags);
			PARSE_RANGE(depthScaleRange);
			PARSE_RANGE(depthScaleValue);
			PARSE_RANGE(radialRot);
			PARSE_RANGE(radialScaleX);
			PARSE_RANGE(radialScaleY);
			PARSE_FIELD_ARR(dir, 3);
			// intensityXIntervalCount
			// intensityYIntervalCount
			// srcCosIntensityIntervalCount
			// srcCosScaleIntervalCount
			PARSE_FIELD_ALLOC_ARR_MODVAL(intensityX, asset->intensityXIntervalCount); // alloc
			PARSE_FIELD_ALLOC_ARR_MODVAL(intensityY, asset->intensityYIntervalCount); // alloc
			PARSE_FIELD_ALLOC_ARR_MODVAL(srcCosIntensity, asset->srcCosIntensityIntervalCount); // alloc
			PARSE_FIELD_ALLOC_ARR_MODVAL(srcCosScale, asset->srcCosScaleIntervalCount); // alloc
		}

		void parse_elem(FxElemDef* asset, ordered_json& data, const elem_type_e type, zone_memory* mem)
		{
			PARSE_FIELD(flags);
			PARSE_FIELD(flags2);

			switch (type)
			{
			case elem_type_looping:
				PARSE_FIELD(spawn.looping.intervalMsec);
				PARSE_FIELD(spawn.looping.count);
				break;
			case elem_type_oneshot:
				PARSE_RANGE(spawn.oneShot.count);
				break;
			}

			PARSE_RANGE(spawnRange);
			PARSE_RANGE(fadeInRange);
			PARSE_RANGE(fadeOutRange);
			PARSE_FIELD(spawnFrustumCullRadius);
			PARSE_RANGE(spawnDelayMsec);
			PARSE_RANGE(lifeSpanMsec);
			PARSE_RANGE_ARR(spawnOrigin, 3);
			PARSE_RANGE(spawnOffsetRadius);
			PARSE_RANGE(spawnOffsetHeight);
			PARSE_RANGE_ARR(spawnAngles, 3);
			PARSE_RANGE_ARR(angularVelocity, 3);
			PARSE_RANGE(initialRotation);
			PARSE_RANGE(gravity);
			PARSE_RANGE(reflectionFactor);
			PARSE_FIELD(atlas.behavior);
			PARSE_FIELD(atlas.index);
			PARSE_FIELD(atlas.fps);
			PARSE_FIELD(atlas.loopCount);
			PARSE_FIELD(atlas.colIndexBits);
			PARSE_FIELD(atlas.rowIndexBits);
			PARSE_FIELD(atlas.entryCount);

			PARSE_ELEM_TYPE();
			PARSE_ELEM_LIT_TYPE();

			asset->visualCount = static_cast<unsigned char>(data["visuals"].size()); // visualCount
			if (!data["velSamples"].is_null())
			{
				asset->velIntervalCount = static_cast<unsigned char>(data["velSamples"].size()) - 1; // visStateIntervalCount
			}
			else
			{
				asset->velIntervalCount = 0;
			}
			if (!data["visSamples"].is_null())
			{
				asset->visStateIntervalCount = static_cast<unsigned char>(data["visSamples"].size()) - 1; // visStateIntervalCount
			}
			else
			{
				asset->visStateIntervalCount = 0;
			}

			if (!data["velSamples"].is_null())
			{
				asset->velSamples = mem->allocate<FxElemVelStateSample>(asset->velIntervalCount + 1);
				for (auto i = 0; i < asset->velIntervalCount + 1; i++)
				{
					parse_vel_state_sample(&asset->velSamples[i], data["velSamples"][i]);
				}
			}

			if (!data["visSamples"].is_null())
			{
				asset->visSamples = mem->allocate<FxElemVisStateSample>(asset->visStateIntervalCount + 1);
				for (auto i = 0; i < asset->visStateIntervalCount + 1; i++)
				{
					parse_vis_sample_state(&asset->visSamples[i], data["visSamples"][i]);
				}
			}

			if (asset->elemType == FX_ELEM_TYPE_DECAL)
			{
				asset->visuals.markArray = mem->allocate<FxElemMarkVisuals>(asset->visualCount);
				if (asset->visuals.markArray)
				{
					for (unsigned char a = 0; a < asset->visualCount; a++)
					{
						data["visuals"][a]["materials"][0] = nullptr;
						data["visuals"][a]["materials"][1] = nullptr;
						data["visuals"][a]["materials"][2] = nullptr;

						if (!data["visuals"][a]["materials"][0].is_null())
						{
							asset->visuals.markArray[a].materials[0] = mem->allocate<Material>();
							asset->visuals.markArray[a].materials[0]->name = mem->duplicate_string(data["visuals"][a]["materials"][0].get<std::string>());
						}
						if (!data["visuals"][a]["materials"][1].is_null())
						{
							asset->visuals.markArray[a].materials[1] = mem->allocate<Material>();
							asset->visuals.markArray[a].materials[1]->name = mem->duplicate_string(data["visuals"][a]["materials"][1].get<std::string>());
						}
						if (!data["visuals"][a]["materials"][2].is_null())
						{
							asset->visuals.markArray[a].materials[2] = mem->allocate<Material>();
							asset->visuals.markArray[a].materials[2]->name = mem->duplicate_string(data["visuals"][a]["materials"][2].get<std::string>());
						}
					}
				}
			}
			else if (asset->visualCount > 1)
			{
				asset->visuals.array = mem->allocate<FxElemVisuals>(asset->visualCount);
				if (asset->visuals.array)
				{
					for (unsigned char vis = 0; vis < asset->visualCount; vis++)
					{
						parse_visuals(&asset->visuals.array[vis], asset, data["visuals"][vis], mem);
					}
				}
			}
			else
			{
				parse_visuals(&asset->visuals.instance, asset, data["visuals"][0], mem);
			}

			PARSE_BOUNDS(collBounds);

			// dump reference FX defs
			PARSE_ASSET(effectOnImpact.handle);
			PARSE_ASSET(effectOnDeath.handle);
			PARSE_ASSET(effectEmitted.handle);

			PARSE_RANGE(emitDist);
			PARSE_RANGE(emitDistVariance);

			// dump extended FX data
			asset->extended.unknownDef = nullptr;
			if (asset->extended.unknownDef)
			{
				if (asset->elemType == FX_ELEM_TYPE_TRAIL)
				{
					parse_trail(asset->extended.trailDef, data["extended"]["trail"], mem);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
				{
					parse_spark_fountain(asset->extended.sparkFountainDef, data["extended"]["sparkFountain"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					parse_spot_light(asset->extended.spotLightDef, data["extended"]["spotLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
				{
					parse_omni_light(asset->extended.omniLightDef, data["extended"]["omniLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_FLARE)
				{
					parse_flare(asset->extended.flareDef, data["extended"]["flare"], mem);
				}
				else
				{
					__debugbreak();
				}
			}

			PARSE_FIELD(sortOrder);
			PARSE_FIELD(lightingFrac);
			PARSE_FIELD(useItemClip);
			PARSE_FIELD(fadeInfo);
			PARSE_FIELD(fadeOutInfo);
			PARSE_FIELD(randomSeed);
			PARSE_FIELD(emissiveScaleScale);
			PARSE_FIELD(hdrLightingFrac);
			PARSE_FIELD(shadowDensityScale);
			PARSE_FIELD(scatterRatio);
			PARSE_FIELD(volumetricTrailFadeStart);
		}

		void sort_and_count_elems(ordered_json& data, int& loop_count, int& one_shot_count, int& emission_count)
		{
			if (data.empty())
			{
				return;
			}

			ordered_json loop_data;
			ordered_json one_shot_data;
			ordered_json emission_data;

			for (const auto& item : data)
			{
				auto type = get_value_index<elem_type_e>(item["type"].get<std::string>(), elem_type_s, std::extent_v<decltype(elem_type_s)>, elem_type_error);
				assert(type != elem_type_error);

				switch (type)
				{
				case elem_type_looping:
					loop_data.push_back(item);
					break;
				case elem_type_oneshot:
					one_shot_data.push_back(item);
					break;
				case elem_type_emission:
					emission_data.push_back(item);
					break;
				}
			}

			// Clear and rebuild the data structure
			data.clear();
			data = ::json::array();
			data.insert(data.end(), loop_data.begin(), loop_data.end());
			data.insert(data.end(), one_shot_data.begin(), one_shot_data.end());
			data.insert(data.end(), emission_data.begin(), emission_data.end());

			// Update the counts
			loop_count = static_cast<int>(loop_data.size());
			one_shot_count = static_cast<int>(one_shot_data.size());
			emission_count = static_cast<int>(emission_data.size());
		}

		FxEffectDef* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = "effects\\"s + name + ".json"s;

			auto file = filesystem::file(path);
			if (!file.exists())
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing fx \"%s\"...", name.data());

			// parse json file
			file.open("rb");
			ordered_json data = ::json::parse(file.read_bytes(file.size()));
			file.close();

			// allocate asset
			auto asset = mem->allocate<FxEffectDef>();

			PARSE_STRING(name);
			PARSE_FIELD(flags);
			PARSE_FIELD(totalSize);
			PARSE_FIELD(msecLoopingLife);
			//PARSE_FIELD(elemDefCountLooping);
			//PARSE_FIELD(elemDefCountOneShot);
			//PARSE_FIELD(elemDefCountEmission);
			PARSE_FIELD(elemMaxRadius);
			PARSE_FIELD(occlusionQueryDepthBias);
			PARSE_FIELD(occlusionQueryFadeIn);
			PARSE_FIELD(occlusionQueryFadeOut);
			PARSE_RANGE(occlusionQueryScaleRange);

			sort_and_count_elems(data["elems"], asset->elemDefCountLooping, asset->elemDefCountOneShot, asset->elemDefCountEmission);

			asset->elemDefs = mem->allocate<FxElemDef>(asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				const auto is_looping = i < asset->elemDefCountLooping;
				const auto is_oneshot = !is_looping && i < asset->elemDefCountLooping + asset->elemDefCountOneShot;
				const auto is_emission = !is_oneshot && i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;

				elem_type_e type = is_looping ? elem_type_looping : is_oneshot ? elem_type_oneshot : is_emission ? elem_type_emission : elem_type_error;
				assert(type != elem_type_error);

				assert(data["elems"].is_array());
				parse_elem(&asset->elemDefs[i], data["elems"][i], type, mem);
			}

			return asset;
		}

#define DUMP_STRING(__field__) \
		static_assert(std::is_same_v<decltype(asset->__field__), const char*>, "Field is not of type const char*"); \
		asset->__field__ ? data[#__field__] = asset->__field__ : data[#__field__] = "";

#define DUMP_FIELD(__field__) \
		data[#__field__] = asset->__field__;

#define DUMP_FIELD_ARR(__field__, __size__) \
		for (auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__size__; idx##__field__++) \
		{ \
			data[#__field__][idx##__field__] = asset->__field__[idx##__field__]; \
		}

#define DUMP_ASSET(__field__) \
		if (asset->__field__) \
		{ \
			data[#__field__] = asset->__field__->name; \
		} \
		else \
		{ \
			data[#__field__] = ""; \
		}

#define DUMP_RANGE(__field__) \
		data[#__field__]["base"] = asset->__field__.base; \
		data[#__field__]["amplitude"] = asset->__field__.amplitude;

#define DUMP_RANGE_ARR(__field__, __count__) \
		for(auto idx##__field__ = 0u; idx##__field__ < (unsigned int)__count__; idx##__field__++) \
		{ \
			data[#__field__][idx##__field__]["base"] = asset->__field__[idx##__field__].base; \
			data[#__field__][idx##__field__]["amplitude"] = asset->__field__[idx##__field__].amplitude; \
		}

#define DUMP_ELEM_TYPE() \
		data["elemType"] = FX_ELEM_TYPE_s[asset->elemType];

#define DUMP_ELEM_LIT_TYPE() \
		data["elemLitType"] = FX_ELEM_LIT_TYPE_s[asset->elemLitType];

#define DUMP_BOUNDS(__field__) \
		for(auto i = 0; i < 3; i++) data[#__field__]["midPoint"][i] = asset->__field__.midPoint[i]; \
		for(auto i = 0; i < 3; i++) data[#__field__]["halfSize"][i] = asset->__field__.halfSize[i];

		void dump_visuals(FxElemVisuals* asset, FxElemDef* def, ordered_json& data)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				DUMP_ASSET(model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				DUMP_ASSET(effectDef.handle);
				break;
			case FX_ELEM_TYPE_SOUND:
				DUMP_STRING(soundName);
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				DUMP_STRING(vectorFieldName);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				DUMP_ASSET(particleSimAnimation);
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						DUMP_ASSET(lightDef);
					}
				}
				else
				{
					DUMP_ASSET(material);
				}
				break;
			}
		}

		void dump_vec3_range(FxElemVec3Range* asset, ordered_json& data)
		{
			for (auto i = 0; i < 3; i++) data["base"][i] = asset->base[i];
			for (auto i = 0; i < 3; i++) data["amplitude"][i] = asset->amplitude[i];
		}

		void dump_vel_state_sample(FxElemVelStateSample* asset, ordered_json& data)
		{
			dump_vec3_range(&asset->local.velocity, data["local"]["velocity"]);
			dump_vec3_range(&asset->local.totalDelta, data["local"]["totalDelta"]);
			dump_vec3_range(&asset->world.velocity, data["world"]["velocity"]);
			dump_vec3_range(&asset->world.totalDelta, data["world"]["totalDelta"]);
		}

		void dump_visual_state(FxElemVisualState* asset, ordered_json& data)
		{
			DUMP_FIELD_ARR(color, 4);
			DUMP_FIELD_ARR(emissiveScale, 3);
			DUMP_FIELD(rotationDelta);
			DUMP_FIELD(rotationTotal);
			DUMP_FIELD_ARR(size, 2);
			DUMP_FIELD(scale);
			DUMP_FIELD_ARR(pivot, 2);
		}

		void dump_vis_sample_state(FxElemVisStateSample* asset, ordered_json& data)
		{
			dump_visual_state(&asset->base, data["base"]);
			dump_visual_state(&asset->amplitude, data["amplitude"]);
		}

		void dump_trail_vertex(FxTrailVertex* asset, ordered_json& data)
		{
			DUMP_FIELD_ARR(pos, 2);
			DUMP_FIELD_ARR(normal, 2);
			DUMP_FIELD_ARR(texCoord, 2);
			DUMP_FIELD_ARR(radialNormal, 2);
		}

		void dump_trail(FxTrailDef* asset, ordered_json& data)
		{
			DUMP_FIELD(scrollTimeMsec);
			DUMP_FIELD(repeatDist);
			DUMP_FIELD(invSplitDist);
			DUMP_FIELD(invSplitArcDist);
			DUMP_FIELD(invSplitTime);
			DUMP_FIELD(headFadingFactor);
			DUMP_FIELD(tailFadingFactor);

			// vertCount
			for (auto i = 0; i < asset->vertCount; i++)
			{
				dump_trail_vertex(&asset->verts[i], data["verts"][i]);
			}

			DUMP_FIELD_ARR(inds, asset->indCount); // alloc
		}

		void dump_spark_fountain(FxSparkFountainDef* asset, ordered_json& data)
		{
			DUMP_FIELD(gravity);
			DUMP_FIELD(bounceFrac);
			DUMP_FIELD(bounceRand);
			DUMP_FIELD(sparkSpacing);
			DUMP_FIELD(sparkLength);
			DUMP_FIELD(sparkCount);
			DUMP_FIELD(loopTime);
			DUMP_FIELD(velMin);
			DUMP_FIELD(velMax);
			DUMP_FIELD(velConeFrac);
			DUMP_FIELD(restSpeed);
			DUMP_FIELD(boostTime);
			DUMP_FIELD(boostFactor);
		}

		void dump_spot_light(FxSpotLightDef* asset, ordered_json& data)
		{
			DUMP_FIELD(halfFovOuter);
			DUMP_FIELD(halfFovInner);
			DUMP_FIELD(radius);
			DUMP_FIELD(brightness);
			DUMP_FIELD(maxLength);
			DUMP_FIELD(exponent);
			DUMP_FIELD(nearClip);
			DUMP_FIELD(bulbRadius);
			DUMP_FIELD(bulbLength);
			DUMP_FIELD_ARR(fadeOffsetRt, 2);
			DUMP_FIELD(unk1);
			DUMP_FIELD(opl);
			DUMP_FIELD(unk2);
			DUMP_FIELD(unused);
		}

		void dump_omni_light(FxOmniLightDef* asset, ordered_json& data)
		{
			DUMP_FIELD(bulbRadius);
			DUMP_FIELD(bulbLength);
			DUMP_FIELD_ARR(fadeOffsetRt, 2);
		}

		void dump_flare(FxFlareDef* asset, ordered_json& data)
		{
			DUMP_FIELD(position);
			DUMP_FIELD(angularRotCount);
			DUMP_FIELD(flags);
			DUMP_RANGE(depthScaleRange);
			DUMP_RANGE(depthScaleValue);
			DUMP_RANGE(radialRot);
			DUMP_RANGE(radialScaleX);
			DUMP_RANGE(radialScaleY);
			DUMP_FIELD_ARR(dir, 3);
			// intensityXIntervalCount
			// intensityYIntervalCount
			// srcCosIntensityIntervalCount
			// srcCosScaleIntervalCount
			DUMP_FIELD_ARR(intensityX, asset->intensityXIntervalCount); // alloc
			DUMP_FIELD_ARR(intensityY, asset->intensityYIntervalCount); // alloc
			DUMP_FIELD_ARR(srcCosIntensity, asset->srcCosIntensityIntervalCount); // alloc
			DUMP_FIELD_ARR(srcCosScale, asset->srcCosScaleIntervalCount); // alloc
		}

		void dump_elem(FxElemDef* asset, ordered_json& data, const elem_type_e type)
		{
			data["type"] = elem_type_s[type];

			DUMP_FIELD(flags);
			DUMP_FIELD(flags2);

			switch (type)
			{
			case elem_type_looping:
				DUMP_FIELD(spawn.looping.intervalMsec);
				DUMP_FIELD(spawn.looping.count);
				break;
			case elem_type_oneshot:
				DUMP_RANGE(spawn.oneShot.count);
				break;
			}

			DUMP_RANGE(spawnRange);
			DUMP_RANGE(fadeInRange);
			DUMP_RANGE(fadeOutRange);
			DUMP_FIELD(spawnFrustumCullRadius);
			DUMP_RANGE(spawnDelayMsec);
			DUMP_RANGE(lifeSpanMsec);
			DUMP_RANGE_ARR(spawnOrigin, 3);
			DUMP_RANGE(spawnOffsetRadius);
			DUMP_RANGE(spawnOffsetHeight);
			DUMP_RANGE_ARR(spawnAngles, 3);
			DUMP_RANGE_ARR(angularVelocity, 3);
			DUMP_RANGE(initialRotation);
			DUMP_RANGE(gravity);
			DUMP_RANGE(reflectionFactor);
			DUMP_FIELD(atlas.behavior);
			DUMP_FIELD(atlas.index);
			DUMP_FIELD(atlas.fps);
			DUMP_FIELD(atlas.loopCount);
			DUMP_FIELD(atlas.colIndexBits);
			DUMP_FIELD(atlas.rowIndexBits);
			DUMP_FIELD(atlas.entryCount);

			DUMP_ELEM_TYPE();
			DUMP_ELEM_LIT_TYPE();

			// visualCount
			// velIntervalCount;
			// visStateIntervalCount

			if (asset->velSamples)
			{
				for (auto i = 0; i < asset->velIntervalCount + 1; i++)
				{
					dump_vel_state_sample(&asset->velSamples[i], data["velSamples"][i]);
				}
			}
			else
			{
				data["velSamples"] = nullptr;
			}

			if (asset->visSamples)
			{
				for (auto i = 0; i < asset->visStateIntervalCount + 1; i++)
				{
					dump_vis_sample_state(&asset->visSamples[i], data["visSamples"][i]);
				}
			}
			else
			{
				data["visSamples"] = nullptr;
			}

			data["visuals"] = ::json::array();
			if (asset->elemType == FX_ELEM_TYPE_DECAL)
			{
				if (asset->visuals.markArray)
				{
					for (unsigned char a = 0; a < asset->visualCount; a++)
					{
						data["visuals"][a]["materials"][0] = nullptr;
						data["visuals"][a]["materials"][1] = nullptr;
						data["visuals"][a]["materials"][2] = nullptr;

						if (asset->visuals.markArray[a].materials[0])
						{
							data["visuals"][a]["materials"][0] = asset->visuals.markArray[a].materials[0]->name;
						}
						if (asset->visuals.markArray[a].materials[1])
						{
							data["visuals"][a]["materials"][1] = asset->visuals.markArray[a].materials[1]->name;
						}
						if (asset->visuals.markArray[a].materials[2])
						{
							data["visuals"][a]["materials"][2] = asset->visuals.markArray[a].materials[2]->name;
						}
					}
				}
			}
			else if (asset->visualCount > 1)
			{
				if (asset->visuals.array)
				{
					for (unsigned char vis = 0; vis < asset->visualCount; vis++)
					{
						dump_visuals(&asset->visuals.array[vis], asset, data["visuals"][vis]);
					}
				}
			}
			else
			{
				dump_visuals(&asset->visuals.instance, asset, data["visuals"][0]);
			}

			DUMP_BOUNDS(collBounds);

			// dump reference FX defs
			DUMP_ASSET(effectOnImpact.handle);
			DUMP_ASSET(effectOnDeath.handle);
			DUMP_ASSET(effectEmitted.handle);

			DUMP_RANGE(emitDist);
			DUMP_RANGE(emitDistVariance);

			// dump extended FX data
			data["extended"] = ::json::object();
			if (asset->extended.unknownDef)
			{
				// todo...
				if (asset->elemType == FX_ELEM_TYPE_TRAIL)
				{
					dump_trail(asset->extended.trailDef, data["extended"]["trail"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
				{
					dump_spark_fountain(asset->extended.sparkFountainDef, data["extended"]["sparkFountain"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dump_spot_light(asset->extended.spotLightDef, data["extended"]["spotLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
				{
					dump_omni_light(asset->extended.omniLightDef, data["extended"]["omniLight"]);
				}
				else if (asset->elemType == FX_ELEM_TYPE_FLARE)
				{
					dump_flare(asset->extended.flareDef, data["extended"]["flare"]);
				}
				else
				{
					__debugbreak();
				}
			}

			DUMP_FIELD(sortOrder);
			DUMP_FIELD(lightingFrac);
			DUMP_FIELD(useItemClip);
			DUMP_FIELD(fadeInfo);
			DUMP_FIELD(fadeOutInfo);
			DUMP_FIELD(randomSeed);
			DUMP_FIELD(emissiveScaleScale);
			DUMP_FIELD(hdrLightingFrac);
			DUMP_FIELD(shadowDensityScale);
			DUMP_FIELD(scatterRatio);
			DUMP_FIELD(volumetricTrailFadeStart);
		}

		void dump(FxEffectDef* asset)
		{
			const auto path = "effects\\"s + asset->name + ".json"s;

			ordered_json data;

			DUMP_STRING(name);
			DUMP_FIELD(flags);
			DUMP_FIELD(totalSize);
			DUMP_FIELD(msecLoopingLife);
			//DUMP_FIELD(elemDefCountLooping);
			//DUMP_FIELD(elemDefCountOneShot);
			//DUMP_FIELD(elemDefCountEmission);
			DUMP_FIELD(elemMaxRadius);
			DUMP_FIELD(occlusionQueryDepthBias);
			DUMP_FIELD(occlusionQueryFadeIn);
			DUMP_FIELD(occlusionQueryFadeOut);
			DUMP_RANGE(occlusionQueryScaleRange);

			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				const auto is_looping = i < asset->elemDefCountLooping;
				const auto is_oneshot = !is_looping && i < asset->elemDefCountLooping + asset->elemDefCountOneShot;
				const auto is_emission = !is_oneshot && i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission;

				elem_type_e type = is_looping ? elem_type_looping : is_oneshot ? elem_type_oneshot : is_emission ? elem_type_emission : elem_type_error;
				assert(type != elem_type_error);

				dump_elem(&asset->elemDefs[i], data["elems"][i], type);
			}

			std::string json_str = data.dump(4);
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(json_str.data(), json_str.size(), 1);
			file.close();
		}
	}

	namespace fx::binary
	{
		void parse_visuals(assetmanager::reader* read, FxElemDef* def, FxElemVisuals* vis)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				vis->model = read->read_asset<XModel>();
				break;
			case FX_ELEM_TYPE_RUNNER:
				vis->effectDef.handle = read->read_asset<FxEffectDef>();
				break;
			case FX_ELEM_TYPE_SOUND:
				vis->soundName = read->read_string();
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				vis->vectorFieldName = read->read_string();
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				vis->particleSimAnimation = read->read_asset<FxParticleSimAnimation>();
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						vis->lightDef = read->read_asset<GfxLightDef>();
					}
				}
				else
				{
					vis->material = read->read_asset<Material>();
				}
				break;
			}
		}

		FxEffectDef* parse(const std::string& name, zone_memory* mem)
		{
			assetmanager::reader read(mem);

			const auto path = "effects\\"s + name + ".fxe"s;
			if (!read.open(path))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing fx \"%s\"...", name.data());

			const auto asset = read.read_single<FxEffectDef>();
			asset->name = read.read_string();
			asset->elemDefs = read.read_array<FxElemDef>();

			for (int i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				auto def = &asset->elemDefs[i];

				def->velSamples = read.read_array<FxElemVelStateSample>();
				def->visSamples = read.read_array<FxElemVisStateSample>();

				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (def->visuals.markArray)
					{
						def->visuals.markArray = read.read_array<FxElemMarkVisuals>();

						for (unsigned char j = 0; j < def->visualCount; j++)
						{
							if (def->visuals.markArray[j].materials[0])
							{
								def->visuals.markArray[j].materials[0] = read.read_asset<Material>();
							}
							if (def->visuals.markArray[j].materials[1])
							{
								def->visuals.markArray[j].materials[1] = read.read_asset<Material>();
							}
							if (def->visuals.markArray[j].materials[2])
							{
								def->visuals.markArray[j].materials[2] = read.read_asset<Material>();
							}
						}
					}
				}
				else if (def->visualCount > 1)
				{
					def->visuals.array = read.read_array<FxElemVisuals>();

					for (unsigned char vis = 0; vis < def->visualCount; vis++)
					{
						parse_visuals(&read, def, &def->visuals.array[vis]);
					}
				}
				else
				{
					parse_visuals(&read, def, &def->visuals.instance);
				}

				def->effectOnImpact.handle = read.read_asset<FxEffectDef>();
				def->effectOnDeath.handle = read.read_asset<FxEffectDef>();
				def->effectEmitted.handle = read.read_asset<FxEffectDef>();

				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						def->extended.trailDef = read.read_single<FxTrailDef>();

						if (def->extended.trailDef->verts)
						{
							def->extended.trailDef->verts = read.read_array<FxTrailVertex>();
						}

						if (def->extended.trailDef->inds)
						{
							def->extended.trailDef->inds = read.read_array<unsigned short>();
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
					{
						def->extended.sparkFountainDef = read.read_single<FxSparkFountainDef>();
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						def->extended.spotLightDef = read.read_single<FxSpotLightDef>();
					}
					else if (def->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
					{
						def->extended.omniLightDef = read.read_single<FxOmniLightDef>();
					}
					else if (def->elemType == FX_ELEM_TYPE_FLARE)
					{
						def->extended.flareDef = read.read_single<FxFlareDef>();

						if (def->extended.flareDef->intensityX)
						{
							def->extended.flareDef->intensityX = read.read_array<float>();
						}

						if (def->extended.flareDef->intensityY)
						{
							def->extended.flareDef->intensityY = read.read_array<float>();
						}

						if (def->extended.flareDef->srcCosIntensity)
						{
							def->extended.flareDef->srcCosIntensity = read.read_array<float>();
						}

						if (def->extended.flareDef->srcCosScale)
						{
							def->extended.flareDef->srcCosScale = read.read_array<float>();
						}
					}
					else
					{
						def->extended.unknownDef = read.read_single<char>();
					}
				}
			}

			read.close();

			return asset;
		}

		void dump_visuals(assetmanager::dumper* dump, FxElemDef* def, FxElemVisuals* vis)
		{
			switch (def->elemType)
			{
			case FX_ELEM_TYPE_MODEL:
				dump->dump_asset(vis->model);
				break;
			case FX_ELEM_TYPE_RUNNER:
				dump->dump_asset(vis->effectDef.handle);
				break;
			case FX_ELEM_TYPE_SOUND:
				dump->dump_string(vis->soundName);
				break;
			case FX_ELEM_TYPE_VECTORFIELD:
				dump->dump_string(vis->vectorFieldName);
				break;
			case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
				dump->dump_asset(vis->particleSimAnimation);
				break;
			default:
				if (def->elemType - 12 <= 1u)
				{
					if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump->dump_asset(vis->lightDef);
					}
				}
				else
				{
					dump->dump_asset(vis->material);
				}
				break;
			}
		}

		void dump(FxEffectDef* asset)
		{
			assetmanager::dumper dump;

			const auto path = "effects\\"s + asset->name + ".fxe"s;
			if (!dump.open(path))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_array(asset->elemDefs,
				asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission);

			// dump elemDefs
			for (auto i = 0; i < asset->elemDefCountLooping + asset->elemDefCountOneShot + asset->elemDefCountEmission; i++)
			{
				auto def = &asset->elemDefs[i];

				// dump elem samples
				dump.dump_array(def->velSamples, def->velIntervalCount + 1);
				dump.dump_array(def->visSamples, def->visStateIntervalCount + 1);

				// dump visuals
				if (def->elemType == FX_ELEM_TYPE_DECAL)
				{
					if (def->visuals.markArray)
					{
						dump.dump_array(def->visuals.markArray, def->visualCount);

						for (unsigned char a = 0; a < def->visualCount; a++)
						{
							if (def->visuals.markArray[a].materials[0])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[0]);
							}
							if (def->visuals.markArray[a].materials[1])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[1]);
							}
							if (def->visuals.markArray[a].materials[2])
							{
								dump.dump_asset(def->visuals.markArray[a].materials[2]);
							}
						}
					}
				}
				else if (def->visualCount > 1)
				{
					if (def->visuals.markArray)
					{
						dump.dump_array(def->visuals.array, def->visualCount);
						for (unsigned char vis = 0; vis < def->visualCount; vis++)
						{
							dump_visuals(&dump, def, &def->visuals.array[vis]);
						}
					}
				}
				else
				{
					dump_visuals(&dump, def, &def->visuals.instance);
				}

				// dump reference FX defs
				dump.dump_asset(def->effectOnImpact.handle);
				dump.dump_asset(def->effectOnDeath.handle);
				dump.dump_asset(def->effectEmitted.handle);

				// dump extended FX data
				if (def->extended.trailDef)
				{
					if (def->elemType == FX_ELEM_TYPE_TRAIL)
					{
						dump.dump_single(def->extended.trailDef);

						if (def->extended.trailDef->verts)
						{
							dump.dump_array(def->extended.trailDef->verts, def->extended.trailDef->vertCount);
						}

						if (def->extended.trailDef->inds)
						{
							dump.dump_array(def->extended.trailDef->inds, def->extended.trailDef->indCount);
						}
					}
					else if (def->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
					{
						dump.dump_single(def->extended.sparkFountainDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
					{
						dump.dump_single(def->extended.spotLightDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
					{
						dump.dump_single(def->extended.omniLightDef);
					}
					else if (def->elemType == FX_ELEM_TYPE_FLARE)
					{
						dump.dump_single(def->extended.flareDef);

						if (def->extended.flareDef->intensityX)
						{
							dump.dump_array(def->extended.flareDef->intensityX, def->extended.flareDef->intensityXIntervalCount + 1);
						}

						if (def->extended.flareDef->intensityY)
						{
							dump.dump_array(def->extended.flareDef->intensityY, def->extended.flareDef->intensityYIntervalCount + 1);
						}

						if (def->extended.flareDef->srcCosIntensity)
						{
							dump.dump_array(def->extended.flareDef->srcCosIntensity, def->extended.flareDef->srcCosIntensityIntervalCount + 1);
						}

						if (def->extended.flareDef->srcCosScale)
						{
							dump.dump_array(def->extended.flareDef->srcCosScale, def->extended.flareDef->srcCosScaleIntervalCount + 1);
						}
					}
					else
					{
						dump.dump_single(def->extended.unknownDef);
					}
				}
			}

			dump.close();
		}
	}

	

	FxEffectDef* fx_effect_def::parse(const std::string& name, zone_memory* mem)
	{
		FxEffectDef* asset = fx::json::parse(name, mem);
		if (asset)
		{
			return asset;
		}
		return fx::binary::parse(name, mem);
	}

	void fx_effect_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).fx;
		}
	}

	void fx_effect_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void fx_effect_def::load_depending(zone_base* zone)
	{
		auto data = this->asset_;

		auto load_FxElemVisuals = [zone](FxElemDef* def, FxElemDefVisuals* vis)
		{
			auto load_visuals = [zone](FxElemDef* def, FxElemVisuals* vis)
			{
				if (!vis->anonymous)
				{
					return;
				}

				switch (def->elemType)
				{
				case FX_ELEM_TYPE_MODEL:
					zone->add_asset_of_type(ASSET_TYPE_XMODEL, vis->model->name);
					break;
				case FX_ELEM_TYPE_RUNNER:
					zone->add_asset_of_type(ASSET_TYPE_FX, vis->effectDef.handle->name);
					break;
				case FX_ELEM_TYPE_SOUND:
					zone->add_asset_of_type(ASSET_TYPE_SOUND, vis->soundName);
					break;
				case FX_ELEM_TYPE_VECTORFIELD:
					zone->add_asset_of_type(ASSET_TYPE_VECTORFIELD, vis->vectorFieldName);
					break;
				case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
					zone->add_asset_of_type(ASSET_TYPE_PARTICLE_SIM_ANIMATION, vis->particleSimAnimation->name);
					break;
				default:
					if (def->elemType - 12 <= 1u)
					{
						if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
						{
							zone->add_asset_of_type(ASSET_TYPE_LIGHT_DEF, vis->lightDef->name);
						}
					}
					else
					{
						zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->material->name);
					}
					break;
				}
			};

			if (def->elemType == FX_ELEM_TYPE_DECAL)
			{
				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					if (vis->markArray[i].materials)
					{
						if (vis->markArray[i].materials[0])
							zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->markArray[i].materials[0]->name);
						if (vis->markArray[i].materials[1])
							zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->markArray[i].materials[1]->name);
						if (vis->markArray[i].materials[2])
							zone->add_asset_of_type(ASSET_TYPE_MATERIAL, vis->markArray[i].materials[2]->name);
					}
				}
			}
			else if (def->visualCount > 1)
			{
				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					load_visuals(def, &vis->array[i]);
				}
			}
			else
			{
				load_visuals(def, &vis->instance);
			}
		};

		// Loop through frames
		for (int i = 0; i < data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission; i++)
		{
			auto& def = data->elemDefs[i];

			// Sub-FX effects
			if (def.effectEmitted.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectEmitted.handle->name);
			if (def.effectOnDeath.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectOnDeath.handle->name);
			if (def.effectOnImpact.handle)
				zone->add_asset_of_type(ASSET_TYPE_FX, def.effectOnImpact.handle->name);

			// Visuals
			load_FxElemVisuals(&def, &def.visuals);
		}
	}

	std::string fx_effect_def::name()
	{
		return this->name_;
	}

	std::int32_t fx_effect_def::type()
	{
		return ASSET_TYPE_FX;
	}

	void write_fx_elem_visuals(zone_base* zone, zone_buffer* buf, FxElemDef* def, FxElemVisuals* dest)
	{
		auto data = dest;

		if (!data->anonymous)
		{
			return;
		}

		switch (def->elemType)
		{
		case FX_ELEM_TYPE_MODEL:
			dest->model = reinterpret_cast<XModel*>(zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->model->name));
			break;
		case FX_ELEM_TYPE_RUNNER:
			dest->effectDef.name = buf->write_str(data->effectDef.handle->name);
			break;
		case FX_ELEM_TYPE_SOUND:
			dest->soundName = buf->write_str(data->soundName);
			break;
		case FX_ELEM_TYPE_VECTORFIELD:
			dest->vectorFieldName = buf->write_str(data->vectorFieldName);
			break;
		case FX_ELEM_TYPE_PARTICLE_SIM_ANIMATION:
			dest->particleSimAnimation = reinterpret_cast<FxParticleSimAnimation*>(zone->get_asset_pointer(ASSET_TYPE_PARTICLE_SIM_ANIMATION, data->particleSimAnimation->name));
			break;
		default:
			if (def->elemType - 12 <= 1u)
			{
				if (def->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
				{
					dest->lightDef = reinterpret_cast<GfxLightDef*>(zone->get_asset_pointer(ASSET_TYPE_LIGHT_DEF, data->lightDef->name));
				}
			}
			else
			{
				dest->material = reinterpret_cast<Material*>(zone->get_asset_pointer(ASSET_TYPE_MATERIAL, data->material->name));
			}
			break;
		}
	}

	void write_fx_elem_def_visuals(zone_base* zone, zone_buffer* buf, FxElemDef* def, FxElemDefVisuals* dest)
	{
		auto data = dest;

		if (def->elemType == FX_ELEM_TYPE_DECAL)
		{
			if (data->markArray)
			{
				auto destvisuals = buf->write(data->markArray, def->visualCount);

				for (unsigned char i = 0; i < def->visualCount; i++)
				{
					destvisuals[i].materials[0] = (data->markArray[i].materials[0])
						? reinterpret_cast<Material*>(zone->get_asset_pointer(
							ASSET_TYPE_MATERIAL, data->markArray[i].materials[0]->name))
						: nullptr;
					destvisuals[i].materials[1] = (data->markArray[i].materials[1])
						? reinterpret_cast<Material*>(zone->get_asset_pointer(
							ASSET_TYPE_MATERIAL, data->markArray[i].materials[1]->name))
						: nullptr;
					destvisuals[i].materials[2] = (data->markArray[i].materials[2])
						? reinterpret_cast<Material*>(zone->get_asset_pointer(
							ASSET_TYPE_MATERIAL, data->markArray[i].materials[2]->name))
						: nullptr;
				}
			}
		}
		else if (def->visualCount > 1)
		{
			auto vis = buf->write(data->array, def->visualCount);

			for (unsigned char i = 0; i < def->visualCount; i++)
			{
				write_fx_elem_visuals(zone, buf, def, &vis[i]);
			}
		}
		else
		{
			write_fx_elem_visuals(zone, buf, def, &dest->instance);
		}
	}

	void write_fx_elem_def(zone_base* zone, zone_buffer* buf, FxElemDef* dest)
	{
		auto data = dest;

		if (data->velSamples)
		{
			buf->align(3);
			buf->write(data->velSamples, data->velIntervalCount + 1);
			buf->clear_pointer(&dest->velSamples);
		}

		if (data->visSamples)
		{
			buf->align(3);
			buf->write(data->visSamples, data->visStateIntervalCount + 1);
			buf->clear_pointer(&dest->velSamples);
		}

		write_fx_elem_def_visuals(zone, buf, data, &dest->visuals);

		if (data->effectOnImpact.handle)
		{
			buf->write_str(data->effectOnImpact.handle->name);
			buf->clear_pointer(&dest->effectOnImpact);
		}

		if (data->effectOnDeath.handle)
		{
			buf->write_str(data->effectOnDeath.handle->name);
			buf->clear_pointer(&dest->effectOnDeath);
		}

		if (data->effectEmitted.handle)
		{
			buf->write_str(data->effectEmitted.handle->name);
			buf->clear_pointer(&dest->effectEmitted);
		}

		if (data->extended.trailDef)
		{
			if (data->elemType == FX_ELEM_TYPE_TRAIL)
			{
				if (data->extended.trailDef)
				{
					buf->align(3);
					buf->write(data->extended.trailDef);

					if (data->extended.trailDef->verts)
					{
						buf->align(3);
						buf->write(data->extended.trailDef->verts, data->extended.trailDef->vertCount);
					}

					if (data->extended.trailDef->inds)
					{
						buf->align(1);
						buf->write(data->extended.trailDef->inds, data->extended.trailDef->indCount);
					}

					buf->clear_pointer(&dest->extended.trailDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_SPARK_FOUNTAIN)
			{
				if (data->extended.sparkFountainDef)
				{
					buf->align(3);
					buf->write(data->extended.sparkFountainDef);
					buf->clear_pointer(&dest->extended.sparkFountainDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_SPOT_LIGHT)
			{
				if (data->extended.spotLightDef)
				{
					buf->align(3);
					buf->write(data->extended.spotLightDef);
					buf->clear_pointer(&dest->extended.spotLightDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_OMNI_LIGHT)
			{
				if (data->extended.omniLightDef)
				{
					buf->align(3);
					buf->write(data->extended.omniLightDef);
					buf->clear_pointer(&dest->extended.omniLightDef);
				}
			}
			else if (data->elemType == FX_ELEM_TYPE_FLARE)
			{
				buf->align(3);
				buf->write(data->extended.flareDef);

				if (data->extended.flareDef->intensityX)
				{
					buf->write(data->extended.flareDef->intensityX, data->extended.flareDef->intensityXIntervalCount + 1);
				}

				if (data->extended.flareDef->intensityY)
				{
					buf->write(data->extended.flareDef->intensityY, data->extended.flareDef->intensityYIntervalCount + 1);
				}

				if (data->extended.flareDef->srcCosIntensity)
				{
					buf->write(data->extended.flareDef->srcCosIntensity, data->extended.flareDef->srcCosIntensityIntervalCount + 1);
				}

				if (data->extended.flareDef->srcCosScale)
				{
					buf->write(data->extended.flareDef->srcCosScale, data->extended.flareDef->srcCosScaleIntervalCount + 1);
				}

				buf->clear_pointer(&dest->extended.flareDef);
			}
			else
			{
				buf->align(0);
				buf->write_stream(data->extended.unknownDef, 1);
				buf->clear_pointer(&dest->extended.unknownDef);
			}
		}
	}

	void fx_effect_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->elemDefs)
		{
			buf->align(3);
			auto destdef = buf->write(data->elemDefs,
				data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission);

			for (int i = 0; i < (data->elemDefCountLooping + data->elemDefCountOneShot + data->elemDefCountEmission); i++)
			{
				write_fx_elem_def(zone, buf, &destdef[i]);
			}

			buf->clear_pointer(&dest->elemDefs);
		}

		buf->pop_stream();
	}

	void fx_effect_def::dump(FxEffectDef* asset)
	{
#ifdef DUMP_JSON
		fx::json::dump(asset);
		return;
#else
		fx::binary::dump(asset);
#endif
	}
}