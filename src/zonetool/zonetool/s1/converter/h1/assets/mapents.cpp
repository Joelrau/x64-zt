#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "mapents.hpp"

#include "zonetool/h1/assets/mapents.hpp"

#include "zonetool/s1/functions.hpp"

#include "zonetool/utils/gsc.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace
		{
			std::string convert_mapents(const std::string& source)
			{
				std::string out_buffer{};

				const auto lines = utils::string::split(source, '\n');
				auto in_map_ent = false;
				auto empty = false;
				auto in_comment = false;

				for (auto i = 0; i < lines.size(); i++)
				{
					auto line = lines[i];
					if (line.ends_with('\r'))
					{
						line.pop_back();
					}

					if (line.starts_with("/*") || line.ends_with("/*"))
					{
						in_comment = true;
						continue;
					}

					if (line.starts_with("*/") || line.ends_with("*/"))
					{
						in_comment = false;
						continue;
					}

					if (in_comment || line.starts_with("//") || line.empty())
					{
						continue;
					}

					if (line[0] == '{' && !in_map_ent)
					{
						in_map_ent = true;
						out_buffer.append("{\n");
						continue;
					}

					if (line[0] == '{' && in_map_ent)
					{
						ZONETOOL_FATAL("Unexpected '{' on line %i", i);
					}

					if (line[0] == '}' && in_map_ent)
					{
						if (empty)
						{
							out_buffer.append("\n}\n");
						}
						else if (i < static_cast<int>(lines.size()) - 1)
						{
							out_buffer.append("}\n");
						}
						else
						{
							out_buffer.append("}\0");
						}

						in_map_ent = false;
						continue;
					}

					if (line[0] == '}' && !in_map_ent)
					{
						ZONETOOL_FATAL("Unexpected '}' on line %i", i);
					}

					if (line.starts_with("0 \""))
					{
						out_buffer.append(line);
						out_buffer.append("\n");
						continue;
					}

					std::regex expr(R"~((.+) "(.*)")~");
					std::smatch match{};
					if (!std::regex_search(line, match, expr))
					{
						ZONETOOL_WARNING("Failed to parse line %i (%s)", i, line.data());
						continue;
					}

					auto key = utils::string::to_lower(match[1].str());
					const auto value = match[2].str();

					if (key.size() <= 0)
					{
						ZONETOOL_WARNING("Invalid key ('%s') on line %i (%s)", key.data(), i, line.data());
						continue;
					}

					if (value.size() <= 0)
					{
						continue;
					}

					empty = false;

					if (utils::string::is_numeric(key) || key.size() < 3 || !key.starts_with("\"") || !key.ends_with("\""))
					{
						out_buffer.append(line);
						out_buffer.append("\n");
						continue;
					}

					const auto key_ = key.substr(1, key.size() - 2);
					const auto id = gsc::s1::gsc_ctx->token_id(key_);
					if (id == 0)
					{
						ZONETOOL_WARNING("Key '%s' not found, on line %i (%s)", key_.data(), i, line.data());
						continue;
					}

					out_buffer.append(utils::string::va("%i \"%s\"\n", id, value.data()));
				}

				return out_buffer;
			}

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

						std::string key = gsc::s1::gsc_ctx->token_name(
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
			zonetool::h1::MapEnts* convert(MapEnts* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::MapEnts>();

				REINTERPRET_CAST_SAFE(name);

				REINTERPRET_CAST_SAFE(entityString);
				COPY_VALUE(numEntityChars);

				COPY_VALUE_CAST(trigger);

				COPY_VALUE_CAST(clientTrigger.trigger);
				COPY_VALUE(clientTrigger.numClientTriggerNodes);
				COPY_VALUE_CAST(clientTrigger.clientTriggerAabbTree);
				COPY_VALUE(clientTrigger.triggerStringLength);
				REINTERPRET_CAST_SAFE(clientTrigger.triggerString);
				REINTERPRET_CAST_SAFE(clientTrigger.visionSetTriggers);
				REINTERPRET_CAST_SAFE(clientTrigger.blendLookup);
				new_asset->clientTrigger.unk1 = asset->clientTrigger.unk1;
				REINTERPRET_CAST_SAFE(clientTrigger.triggerType); // convert?
				REINTERPRET_CAST_SAFE(clientTrigger.origins);
				REINTERPRET_CAST_SAFE(clientTrigger.scriptDelay);
				REINTERPRET_CAST_SAFE(clientTrigger.audioTriggers);
				new_asset->clientTrigger.unk2 = asset->clientTrigger.unk2;
				new_asset->clientTrigger.unk3 = asset->clientTrigger.unk3;
				new_asset->clientTrigger.unk4 = asset->clientTrigger.unk4;
				new_asset->clientTrigger.unk5 = asset->clientTrigger.unk5;
				new_asset->clientTrigger.unk6 = mem->Alloc<short>(asset->clientTrigger.trigger.count);

				COPY_VALUE_CAST(clientTriggerBlend);
				COPY_VALUE_CAST(spawnList);
				COPY_VALUE_CAST(splineList);

				return new_asset;
			}

			void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars)
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

			void dump(MapEnts* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IMapEnts::dump(converted_asset);

				// dump the converted mapent strings
				dump_entityStrings(asset->name, asset->entityString, asset->numEntityChars);
			}
		}
	}
}