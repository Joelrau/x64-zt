#include <std_include.hpp>

#include "../../include.hpp"
#include "fog.hpp"

#include <utils/string.hpp>

namespace zonetool::h1::converter::s2::scriptfile
{
	namespace
	{
		constexpr std::array<std::pair<std::string_view, std::string_view>, 24> s2_atmos_fog_keys
		{{
			{"atmosFogExtinctionDesaturation", "0"},
			{"atmosFogHeightFogMaximumOffset", "0"},
			{"atmosFogHeightFogStrengthOfDepthFog", "0"},
			{"atmosFogEntityScale", "1"},
			{"atmosFogAirLightCapacity", "0"},
			{"atmosFogAirLightDensityScalar", "1"},
			{"atmosFogAirLightVertexInnerMutedRadius", "0"},
			{"atmosFogSplineEnabled", "0"},
			{"atmosFogSplineCustomDistance1", "0"},
			{"atmosFogSplineCustomDistance2", "0"},
			{"atmosFogSplineCustomDistance3", "0"},
			{"atmosFogSplineMaxDistance", "0"},
			{"atmosFogSplineCustomVisibility1", "0"},
			{"atmosFogSplineCustomVisibility2", "0"},
			{"atmosFogSplineCustomVisibility3", "0"},
			{"atmosFogSplineMaxDistanceVisibility", "0"},
			{"atmosFogSplineUseMaxDistanceVisibilityOverride", "0"},
			{"atmosFogSplineMaxDistanceVisibilityOverride", "(0, 0, 0)"},
			{"atmosFogSplineCustomTangentsEnabled", "0"},
			{"atmosFogSplineStartingTangentAngle", "0"},
			{"atmosFogSplineCustomTangentAngle1", "0"},
			{"atmosFogSplineCustomTangentAngle2", "0"},
			{"atmosFogSplineCustomTangentAngle3", "0"},
			{"atmosFogSplineMaxDistanceTangentAngle", "0"},
		}};

		constexpr std::array<std::pair<std::string_view, std::string_view>, 5> h1_atmos_height_fog_off
		{{
			{"atmosFogHeightFogEnabled", "1"},
			{"atmosFogHeightFogBaseHeight", "0"},
			{"atmosFogHeightFogHalfPlaneDistance", "100000"},
			{"atmosFogHeightFogMaximumOffset", "0"},
			{"atmosFogHeightFogStrengthOfDepthFog", "1"},
		}};

		std::string fog_key(const std::string_view line)
		{
			if (!line.starts_with("ent."))
			{
				return {};
			}

			const auto end = line.find_first_of(" =", 4);
			return utils::string::to_lower(std::string(line.substr(4, end == std::string_view::npos ? end : end - 4)));
		}

		std::string fog_value(const std::string_view line)
		{
			const auto start = line.find('=');
			if (start == std::string_view::npos)
			{
				return {};
			}

			const auto value = line.substr(start + 1, line.find(';', start) - start - 1);
			const auto first = value.find_first_not_of(" \t");
			return first == std::string_view::npos ? std::string{} : std::string(value.substr(first, value.find_last_not_of(" \t") - first + 1));
		}

		using fog_block = std::unordered_map<std::string, std::string>;

		std::string fog_block_name(const std::string_view line)
		{
			const auto first = line.find('"', line.find("create_vision_set_fog("));
			const auto last = first == std::string_view::npos ? first : line.find('"', first + 1);
			return last == std::string_view::npos ? std::string{} : utils::string::to_lower(std::string(line.substr(first + 1, last - first - 1)));
		}

		std::unordered_map<std::string, fog_block> read_fog_blocks(const std::string& source)
		{
			std::unordered_map<std::string, fog_block> blocks;
			fog_block* block = nullptr;
			for (auto line : utils::string::split(source, '\n'))
			{
				const auto first = line.find_first_not_of(" \t");
				const auto trimmed = first == std::string::npos ? std::string{} : line.substr(first);
				if (trimmed.find("create_vision_set_fog(") != std::string::npos)
				{
					block = &blocks[fog_block_name(trimmed)];
				}
				else if (const auto key = fog_key(trimmed); block && !key.empty())
				{
					(*block)[key] = fog_value(trimmed);
				}
			}
			return blocks;
		}

		float fog_float(const fog_block& block, const std::string& key, const float fallback)
		{
			const auto entry = block.find(key);
			return entry != block.end() && !entry->second.empty() ? std::strtof(entry->second.data(), nullptr) : fallback;
		}

		std::vector<std::string> convert_legacy_fog(const fog_block& block)
		{
			const auto half_plane = std::max(fog_float(block, "halfwaydist", 1.0f), 1.0f) / 0.693147182f;
			const auto start = fog_float(block, "startdist", 0.0f);
			const auto ev = fog_float(block, "hdrcolorintensity", 12.8066f);
			const auto sky_opacity = std::clamp(fog_float(block, "skyfogintensity", 0.0f), 0.0f, 0.999f);
			const auto sky_min = fog_float(block, "skyfogminangle", 0.0f);
			const auto sky_max = fog_float(block, "skyfogmaxangle", 0.0f);
			const auto color = std::format("({}, {}, {})", fog_float(block, "red", 1.0f), fog_float(block, "green", 1.0f), fog_float(block, "blue", 1.0f));

			const auto line = [](const std::string_view key, const auto& value)
			{
				return std::format("\tent.{} = {};", key, value);
			};

			return
			{
				line("atmosFogEnabled", 1),
				line("atmosFogSunFogColor", color),
				line("atmosFogHazeColor", color),
				line("atmosFogHazeStrength", 1),
				line("atmosFogHazeSpread", 0.5f),
				line("atmosFogExtinctionStrength", fog_float(block, "maxopacity", 1.0f)),
				line("atmosFogInScatterStrength", ev - std::log2(0.0795775f)),
				line("atmosFogHalfPlaneDistance", half_plane),
				line("atmosFogStartDistance", start),
				line("atmosFogDistanceScale", 1),
				line("atmosFogSkyDistance", start - half_plane * std::log(1.0f - sky_opacity)),
				line("atmosFogSkyAngularFalloffEnabled", 1),
				line("atmosFogSkyFalloffStartAngle", 90.0f - std::max(sky_min, sky_max)),
				line("atmosFogSkyFalloffAngleRange", std::max(std::abs(sky_max - sky_min), 0.01f)),
				line("atmosFogSunDirection", std::string("(0, 0, 1)")),
				line("atmosFogHeightFogEnabled", 1),
				line("atmosFogHeightFogBaseHeight", 0),
				line("atmosFogHeightFogHalfPlaneDistance", 100000),
			};
		}
	}

	std::string convert_fog_file(const std::string& source)
	{
		const auto blocks = read_fog_blocks(source);
		std::string block_name;
		std::vector<std::string> lines;
		std::unordered_map<std::string, std::size_t> key_lines;
		std::optional<std::size_t> block_end;
		const auto finish_block = [&]
		{
			if (!block_end)
			{
				return;
			}

			std::unordered_map<std::string, std::string_view> values;
			const auto enabled = key_lines.find("atmosfogheightfogenabled");
			if (enabled != key_lines.end() && fog_value(lines[enabled->second]) == "0")
			{
				for (const auto& [key, value] : h1_atmos_height_fog_off)
				{
					values.emplace(utils::string::to_lower(std::string(key)), value);
				}
			}

			std::vector<std::string> missing;
			if (!key_lines.contains("atmosfogenabled") && key_lines.contains("halfwaydist"))
			{
				auto source_block = blocks.find(block_name);
				const auto hdr_key = source_block->second.find("hdroverride");
				if (hdr_key != source_block->second.end())
				{
					std::string hdr_name = utils::string::to_lower(hdr_key->second);
					std::erase(hdr_name, '"');
					if (const auto hdr_block = blocks.find(hdr_name); hdr_block != blocks.end())
					{
						source_block = hdr_block;
					}
				}

				missing = convert_legacy_fog(source_block->second);
				values.emplace("atmosfogextinctiondesaturation", "1");
				values.emplace("atmosfogheightfogstrengthofdepthfog", "1");
			}

			for (const auto& [key, value] : s2_atmos_fog_keys)
			{
				values.try_emplace(utils::string::to_lower(std::string(key)), value);
			}

			for (const auto& [key, value] : h1_atmos_height_fog_off)
			{
				const auto lower = utils::string::to_lower(std::string(key));
				if (const auto line = key_lines.find(lower); line != key_lines.end() && values.contains(lower))
				{
					lines[line->second] = std::format("\tent.{} = {};", key, values.at(lower));
				}
			}

			for (const auto& [key, value] : s2_atmos_fog_keys)
			{
				const auto lower = utils::string::to_lower(std::string(key));
				if (!key_lines.contains(lower))
				{
					missing.emplace_back(std::format("\tent.{} = {};", key, values.at(lower)));
				}
			}

			lines.insert(lines.begin() + static_cast<std::ptrdiff_t>(*block_end), missing.begin(), missing.end());
			key_lines.clear();
			block_end.reset();
		};

		for (auto line : utils::string::split(source, '\n'))
		{
			if (line.ends_with('\r'))
			{
				line.pop_back();
			}

			const auto first = line.find_first_not_of(" \t");
			const auto trimmed = first == std::string::npos ? std::string{} : line.substr(first);
			if (trimmed == "/*" || trimmed == "*/")
			{
				continue;
			}

			if (trimmed == "/$" || trimmed == "$/")
			{
				line.replace(first, 2, trimmed == "/$" ? "/#" : "#/");
			}
			if (trimmed.find("create_vision_set_fog(") != std::string::npos || trimmed == "}")
			{
				finish_block();
			}

			lines.emplace_back(line);
			if (trimmed.find("create_vision_set_fog(") != std::string::npos)
			{
				block_name = fog_block_name(trimmed);
				block_end = lines.size();
			}
			else if (const auto key = fog_key(trimmed); block_end && !key.empty())
			{
				key_lines[key] = lines.size() - 1;
				block_end = lines.size();
			}
		}

		finish_block();

		std::string text;
		for (const auto& line : lines)
		{
			text += line + "\n";
		}

		static const std::regex setup_fog(R"((^|\n)\s*setupfog\s*\()", std::regex::icase);
		if (!std::regex_search(text, setup_fog))
		{
			text += "\nsetupfog()\n{\n}\n";
		}

		return text;
	}
}
