#include <std_include.hpp>

#include "zonetool/h2/converter/h1/include.hpp"
#include "mapents.hpp"
#include "zonetool/h1/assets/mapents.hpp"

#include "zonetool/h2/functions.hpp"
#include "zonetool/utils/gsc.hpp"

namespace zonetool::h2
{
	namespace converter::h1
	{
		namespace mapents
		{
			std::string replace_mapents_keys(const std::string& data)
			{
				std::string buffer{};
				const auto lines = utils::string::split(data, '\n');

				for (const auto& line : lines)
				{
					const auto _0 = gsl::finally([&]
					{
						buffer.append("\n");
					});

					if (line.starts_with("{") || line.starts_with("}") || line.starts_with("\""))
					{
						buffer.append(line);
						continue;
					}

					const auto first_space = line.find_first_of(' ');
					if (first_space == std::string::npos)
					{
						continue;
					}

					const auto id = static_cast<unsigned int>(std::atoi(line.substr(0, first_space).data()));
					if (id == 0) // 0 "key" "value"
					{
						buffer.append(line);
						continue;
					}

					const auto token = gsc::h2::gsc_ctx->token_name(static_cast<std::uint16_t>(id));
					const auto key = "\"" + token + "\"";

					const auto new_line = key + line.substr(first_space);
					buffer.append(new_line);
				}

				return buffer;
			}

			zonetool::h1::MapEnts* convert(zonetool::h2::MapEnts* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::MapEnts>();

				std::memcpy(new_asset, asset, sizeof(zonetool::h2::MapEnts));
				const auto entity_string = replace_mapents_keys({asset->entityString, static_cast<size_t>(asset->numEntityChars)});
				new_asset->entityString = allocator.duplicate_string(entity_string);
				new_asset->numEntityChars = static_cast<int>(entity_string.size());

				return new_asset;
			}

			void dump(zonetool::h2::MapEnts* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::map_ents::dump(converted_asset);
			}
		}
	}
}

