#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "mapents.hpp"

#include "zonetool/h1/assets/mapents.hpp"

#include "zonetool/iw6/functions.hpp"

#include "zonetool/utils/gsc.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace
		{
			std::string convert_mapents_ids(const std::string& source)
			{
				std::string out_buffer;

				const auto lines = utils::string::split(source, '\n');

				for (auto i = 0; i < lines.size(); i++)
				{
					const auto _0 = gsl::finally([&]
					{
						out_buffer.append("\n");
					});

					const auto& line = lines[i];

					std::regex expr(R"~((.+) "(.*)")~");
					std::smatch match{};
					if (!line.starts_with("0 ") && std::regex_search(line, match, expr))
					{
						const auto id = std::atoi(match[1].str().data());
						const auto value = match[2].str();

						std::string key = gsc::iw6::gsc_ctx->token_name(
							static_cast<std::uint16_t>(id));
						if (!key.starts_with("_id_"))
						{
							out_buffer.append(utils::string::va("\"%s\" \"%s\"", key.data(), value.data()));
							continue;
						}
					}
					out_buffer.append(line);
				}

				return out_buffer;
			}
		}

		namespace mapents
		{
			std::int32_t convert_type(std::int32_t flags)
			{
				std::int32_t new_flags = 0;
				auto convert = [&](zonetool::iw6::clientTriggerType_t a, zonetool::h1::clientTriggerType_t b)
				{
					if (flags & a)
						new_flags |= b;
				};

				convert(zonetool::iw6::clientTriggerType_t::CLIENT_TRIGGER_VISIONSET, zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_VISIONSET);
				convert(zonetool::iw6::clientTriggerType_t::CLIENT_TRIGGER_AUDIO, zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_AUDIO);
				convert(zonetool::iw6::clientTriggerType_t::CLIENT_TRIGGER_BLEND_VISION, zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_BLEND_VISION);
				convert(zonetool::iw6::clientTriggerType_t::CLIENT_TRIGGER_BLEND_AUDIO, zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_BLEND_AUDIO);
				convert(zonetool::iw6::clientTriggerType_t::CLIENT_TRIGGER_NPC, zonetool::h1::clientTriggerType_t::CLIENT_TRIGGER_NPC);

				return new_flags;
			}

			zonetool::h1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::h1::MapEnts>();

				REINTERPRET_CAST_SAFE(name);

				const auto str = convert_mapents_ids(
					std::string{asset->entityString, static_cast<size_t>(asset->numEntityChars)});
				new_asset->entityString = allocator.duplicate_string(str);
				new_asset->numEntityChars = static_cast<int>(str.size());

				COPY_VALUE_CAST(trigger);

				COPY_VALUE_CAST(clientTrigger.trigger);
				COPY_VALUE(clientTrigger.numClientTriggerNodes);
				COPY_VALUE_CAST(clientTrigger.clientTriggerAabbTree);
				COPY_VALUE(clientTrigger.triggerStringLength);
				REINTERPRET_CAST_SAFE(clientTrigger.triggerString);
				REINTERPRET_CAST_SAFE(clientTrigger.visionSetTriggers);
				new_asset->clientTrigger.lightSetTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.lightSetTriggers, new_asset->clientTrigger.lightSetTriggers + asset->clientTrigger.trigger.count, -1);
				new_asset->clientTrigger.clutTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.clutTriggers, new_asset->clientTrigger.clutTriggers + asset->clientTrigger.trigger.count, -1);
				
				new_asset->clientTrigger.triggerType = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.triggerType[i] = static_cast<short>(convert_type(asset->clientTrigger.triggerType[i]));
				}

				REINTERPRET_CAST_SAFE(clientTrigger.origins);
				REINTERPRET_CAST_SAFE(clientTrigger.scriptDelay);
				REINTERPRET_CAST_SAFE(clientTrigger.audioTriggers);
				REINTERPRET_CAST_SAFE(clientTrigger.blendLookup);
				new_asset->clientTrigger.unkTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.unkTriggers, new_asset->clientTrigger.unkTriggers + asset->clientTrigger.trigger.count, -1);
				new_asset->clientTrigger.npcTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.npcTriggers, new_asset->clientTrigger.npcTriggers + asset->clientTrigger.trigger.count, -1);
				new_asset->clientTrigger.contextTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.contextTriggers, new_asset->clientTrigger.contextTriggers + asset->clientTrigger.trigger.count, -1);
				new_asset->clientTrigger.waterTriggers = allocator.allocate_array<short>(asset->clientTrigger.trigger.count);
				std::fill(new_asset->clientTrigger.waterTriggers, new_asset->clientTrigger.waterTriggers + asset->clientTrigger.trigger.count, -1);

				for (unsigned int i = 0; i < asset->clientTrigger.trigger.count; i++)
				{
					new_asset->clientTrigger.lightSetTriggers[i] = asset->clientTrigger.visionSetTriggers[i];
				}

				COPY_VALUE(clientTriggerBlend.numClientTriggerBlendNodes);
				REINTERPRET_CAST_SAFE(clientTriggerBlend.blendNodes);

				COPY_VALUE_CAST(spawnList.spawnsCount);
				new_asset->spawnList.spawns = allocator.allocate_array<zonetool::h1::SpawnPointEntityRecord>(asset->spawnList.spawnsCount);
				for (unsigned short i = 0; i < asset->spawnList.spawnsCount; i++)
				{
					COPY_VALUE_CAST(spawnList.spawns[i].index);
					COPY_VALUE_CAST(spawnList.spawns[i].name);
					COPY_VALUE_CAST(spawnList.spawns[i].target);
					COPY_VALUE_CAST(spawnList.spawns[i].script_noteworthy);
					new_asset->spawnList.spawns[i].targetname = static_cast<zonetool::h1::scr_string_t>(SL_AllocString("", 1));
					COPY_ARR(spawnList.spawns[i].origin);
					COPY_ARR(spawnList.spawns[i].angles);
				}

				COPY_VALUE_CAST(splineList.splineCount);
				REINTERPRET_CAST_SAFE(splineList.splines);

				return new_asset;
			}

			void dump_entity_strings(const std::string& name, char* entityString, int numEntityChars)
			{
				const auto path = name + ".ents"s;
				auto file = filesystem::file(path);
				file.open("wb");
				if (file.get_fp())
				{
					const auto str = convert_mapents_ids(
						std::string{ entityString, static_cast<size_t>(numEntityChars) });
					file.write(str.data(), str.size(), 1);
					file.close();
				}
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