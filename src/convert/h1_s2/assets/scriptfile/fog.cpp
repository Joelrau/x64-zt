#include <std_include.hpp>

#include "../../include.hpp"
#include "fog.hpp"

#include <utils/string.hpp>

namespace zonetool::h1::converter::s2::scriptfile
{
	namespace
	{
		// s2 keeps the previous value of a missing atmos fog key, so the blocks get the stock neutral values
		constexpr std::array<std::pair<std::string_view, std::string_view>, 24> s2_atmos_fog_keys
		{{
			{"atmosFogExtinctionDesaturation", "0"},
			{"atmosFogHeightFogMaximumOffset", "0"},
			{"atmosFogHeightFogStrengthOfDepthFog", "0"},
			{"atmosFogEntityScale", "1"},
			{"atmosFogAirLightCapacity", "0"}, // stock uses 2, but s2 air lights haze the room around every bright h1 primary light
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

		// h1 with height fog off uses base height 0 and half plane distance 100000
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
	}

	std::string convert_fog_file(const std::string& source)
	{
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
			if (trimmed == "/*" || trimmed == "*/") // h1 comments out setupfog(), the s2 art script calls it
			{
				continue;
			}

			if (trimmed == "/$" || trimmed == "$/") // gsc compilers only know "/#" dev blocks, the client fog parser skips both
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
		if (!std::regex_search(text, setup_fog)) // custom maps ship no setupfog(), the s2 art script still links it
		{
			text += "\nsetupfog()\n{\n}\n";
		}

		return text;
	}
}
