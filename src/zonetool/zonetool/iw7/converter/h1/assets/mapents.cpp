#include <std_include.hpp>
#include "zonetool/iw7/converter/h1/include.hpp"
#include "mapents.hpp"

#include "zonetool/h1/assets/mapents.hpp"
#include "zonetool/utils/gsc.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace mapents
		{
			// IW7 entity keys are script token ids ("157 \"worldspawn\""); H1 wants text keys
			static std::string replace_mapents_keys(const std::string& data)
			{
				std::string buffer{};
				const auto lines = utils::string::split(data, '\n');
				for (const auto& line : lines)
				{
					if (line.empty()) { continue; }
					if (line.starts_with("{") || line.starts_with("}") || line.starts_with("\""))
					{
						buffer.append(line).append("\n");
						continue;
					}
					const auto first_space = line.find_first_of(' ');
					if (first_space == std::string::npos) { continue; }
					const auto id = static_cast<unsigned int>(std::atoi(line.substr(0, first_space).data()));
					if (id == 0) { continue; }
					const auto token = gsc::iw7::gsc_ctx->token_name(id);
					if (token.empty() || token.starts_with("_id_")) { continue; }
					buffer.append("\"" + token + "\"" + line.substr(first_space)).append("\n");
				}
				// strip brush-model refs (no submodels emitted in clipmap): script_brushmodel -> script_origin
				std::string out{};
				std::string ent{};
				for (const auto& l : utils::string::split(buffer, '\n'))
				{
					if (l.empty()) { continue; }
					ent.append(l).append("\n");
					if (l.starts_with("}"))
					{
						std::string res{};
						const bool trig = ent.find("\"model\" \"?") != std::string::npos;
						for (const auto& el : utils::string::split(ent, '\n'))
						{
							if (el.empty()) { continue; }
							// v53: brush models (*N) are emitted as clipmap submodels and trigger models (?N) are converted -> keep both refs
							(void)trig;
							res.append(el).append("\n");
						}
						out.append(res);
						ent.clear();
					}
				}
				return out;
			}

			zonetool::h1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::MapEnts>();

				REINTERPRET_CAST_SAFE(name);

				// IW7 entity strings are plain text key/values; H1 uses the same format (classnames/keys may need review)
				const auto entity_string = replace_mapents_keys(std::string{asset->entityString, static_cast<size_t>(asset->numEntityChars)});
				// scriptable instances (palms, props, ...) are not an H1 concept, spawn them as plain script_models
				std::string entity_string_full = entity_string;
				if (asset->scriptableMapEnts.instances)
				{
					for (unsigned int i = 0; i < asset->scriptableMapEnts.totalInstanceCount; i++)
					{
						const auto& ctx = asset->scriptableMapEnts.instances[i].contextHeader.context;
						const char* model_name = nullptr;
						if (ctx.modelData.model && ctx.modelData.model->name)
						{
							model_name = ctx.modelData.model->name;
						}
						else if (ctx.def && ctx.def->numXModels && ctx.def->models && ctx.def->models[0])
						{
							model_name = ctx.def->models[0]->name;
						}
						if (!model_name || !*model_name)
						{
							continue;
						}
						if (ctx.def && ctx.def->name && std::string_view{ctx.def->name}.starts_with("hardpoint"))
						{
							continue;
						}
						const auto* o = ctx.origin;
						const auto* a = ctx.angles;
						entity_string_full += utils::string::va("{\n\"classname\" \"script_model\"\n\"model\" \"%s\"\n\"origin\" \"%g %g %g\"\n\"angles\" \"%g %g %g\"\n}\n",
							model_name, o[0], o[1], o[2], a[0], a[1], a[2]);
					}
				}
				new_asset->entityString = allocator.duplicate_string(entity_string_full);
				new_asset->numEntityChars = static_cast<int>(entity_string_full.size());

				// triggers (IW7 models carry extra physics fields, so copy element-wise)
				new_asset->trigger.count = asset->trigger.count;
				new_asset->trigger.models = allocator.allocate_array<zonetool::h1::TriggerModel>(asset->trigger.count);
				for (unsigned int i = 0; i < asset->trigger.count; i++)
				{
					new_asset->trigger.models[i].contents = asset->trigger.models[i].contents;
					new_asset->trigger.models[i].hullCount = asset->trigger.models[i].hullCount;
					new_asset->trigger.models[i].firstHull = asset->trigger.models[i].firstHull;
				}
				new_asset->trigger.hullCount = asset->trigger.hullCount;
				REINTERPRET_CAST_SAFE(trigger.hulls);
				new_asset->trigger.slabCount = asset->trigger.slabCount;
				REINTERPRET_CAST_SAFE(trigger.slabs);

				// client triggers
				const auto count = asset->clientTrigger.trigger.count;
				new_asset->clientTrigger.trigger.count = count;
				new_asset->clientTrigger.trigger.models = allocator.allocate_array<zonetool::h1::TriggerModel>(count);
				for (unsigned int i = 0; i < count; i++)
				{
					new_asset->clientTrigger.trigger.models[i].contents = asset->clientTrigger.trigger.models[i].contents;
					new_asset->clientTrigger.trigger.models[i].hullCount = asset->clientTrigger.trigger.models[i].hullCount;
					new_asset->clientTrigger.trigger.models[i].firstHull = asset->clientTrigger.trigger.models[i].firstHull;
				}
				new_asset->clientTrigger.trigger.hullCount = asset->clientTrigger.trigger.hullCount;
				new_asset->clientTrigger.trigger.hulls = reinterpret_cast<zonetool::h1::TriggerHull*>(asset->clientTrigger.trigger.hulls);
				new_asset->clientTrigger.trigger.slabCount = asset->clientTrigger.trigger.slabCount;
				new_asset->clientTrigger.trigger.slabs = reinterpret_cast<zonetool::h1::TriggerSlab*>(asset->clientTrigger.trigger.slabs);

				new_asset->clientTrigger.numClientTriggerNodes = 0;
				new_asset->clientTrigger.clientTriggerAabbTree = nullptr;
				new_asset->clientTrigger.triggerStringLength = asset->clientTrigger.triggerStringLength;
				new_asset->clientTrigger.triggerString = asset->clientTrigger.triggerString;

				const auto alloc_short = [&](short fill)
				{
					auto* arr = allocator.allocate_array<short>(count);
					std::fill(arr, arr + count, fill);
					return arr;
				};

				new_asset->clientTrigger.visionSetTriggers = asset->clientTrigger.visionSetTriggers;
				new_asset->clientTrigger.lightSetTriggers = alloc_short(-1);
				new_asset->clientTrigger.clutTriggers = alloc_short(-1);
				new_asset->clientTrigger.triggerType = alloc_short(0);
				for (unsigned int i = 0; i < count; i++)
				{
					new_asset->clientTrigger.triggerType[i] = static_cast<short>(asset->clientTrigger.triggerType[i]);
				}
				new_asset->clientTrigger.origins = reinterpret_cast<decltype(new_asset->clientTrigger.origins)>(asset->clientTrigger.origins);
				new_asset->clientTrigger.scriptDelay = asset->clientTrigger.scriptDelay;
				new_asset->clientTrigger.audioTriggers = asset->clientTrigger.audioTriggers;
				new_asset->clientTrigger.blendLookup = asset->clientTrigger.blendLookup;
				new_asset->clientTrigger.npcTriggers = asset->clientTrigger.npcTriggers;
				new_asset->clientTrigger.contextTriggers = alloc_short(-1);
				new_asset->clientTrigger.waterTriggers = alloc_short(-1);
				new_asset->clientTrigger.unkTriggers = alloc_short(-1);

				new_asset->clientTriggerBlend.numClientTriggerBlendNodes = asset->clientTriggerBlend.numClientTriggerBlendNodes;
				new_asset->clientTriggerBlend.blendNodes = reinterpret_cast<zonetool::h1::ClientTriggerBlendNode*>(asset->clientTriggerBlend.blendNodes);

				// spawns
				new_asset->spawnList.spawnsCount = asset->spawnList.spawnsCount;
				new_asset->spawnList.spawns = allocator.allocate_array<zonetool::h1::SpawnPointEntityRecord>(asset->spawnList.spawnsCount);
				for (unsigned short i = 0; i < asset->spawnList.spawnsCount; i++)
				{
					auto& d = new_asset->spawnList.spawns[i];
					const auto& s = asset->spawnList.spawns[i];
					d.index = s.index;
					d.name = static_cast<zonetool::h1::scr_string_t>(s.name);
					d.target = static_cast<zonetool::h1::scr_string_t>(s.target);
					d.script_noteworthy = static_cast<zonetool::h1::scr_string_t>(s.script_noteworthy);
					d.targetname = static_cast<zonetool::h1::scr_string_t>(0);
					std::memcpy(d.origin, s.origin, sizeof(d.origin));
					std::memcpy(d.angles, s.angles, sizeof(d.angles));
				}

				// splines have a different record layout in IW7, skipped for now
				new_asset->splineList.splineCount = 0;
				new_asset->splineList.splines = nullptr;

				return new_asset;
			}

			void dump(MapEnts* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::map_ents::dump(converted_asset);
			}
		}
	}
}
