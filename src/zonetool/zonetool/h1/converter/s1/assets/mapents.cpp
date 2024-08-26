#include <std_include.hpp>
#include "zonetool/h1/converter/s1/include.hpp"
#include "mapents.hpp"

#include "zonetool/s1/assets/mapents.hpp"

#include "zonetool/utils/gsc.hpp"

namespace zonetool::h1
{
	namespace converter::s1
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
						const auto value = match[2].str();

						const auto id = std::atoi(match[1].str().data());
						if (id == 0) // invalid id
						{
							out_buffer.append(line.data());
							continue;
						}

						std::string key = gsc::h1::gsc_ctx->token_name(
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
			zonetool::s1::MapEnts* convert(MapEnts* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::s1::MapEnts>();

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
				
				new_asset->clientTrigger.unk1 = asset->clientTrigger.lightSetTriggers;
				new_asset->clientTrigger.unk2 = asset->clientTrigger.colorGradingTriggers;
				REINTERPRET_CAST_SAFE(clientTrigger.triggerType); // convert?
				REINTERPRET_CAST_SAFE(clientTrigger.origins);
				REINTERPRET_CAST_SAFE(clientTrigger.scriptDelay);
				REINTERPRET_CAST_SAFE(clientTrigger.audioTriggers);
				REINTERPRET_CAST_SAFE(clientTrigger.blendLookup);
				new_asset->clientTrigger.unk3 = asset->clientTrigger.unk3;
				new_asset->clientTrigger.unk4 = asset->clientTrigger.unk4;
				new_asset->clientTrigger.unk5 = asset->clientTrigger.unk5;

				COPY_VALUE_CAST(clientTriggerBlend);
				COPY_VALUE_CAST(spawnList);
				COPY_VALUE_CAST(splineList);

				return new_asset;
			}

			void dump(MapEnts* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::s1::map_ents::dump(converted_asset);
			}
		}
	}
}
