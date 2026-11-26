#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"
#include "vision.hpp"

#include <iw8/assets/gradingclut.hpp>

namespace zonetool::t9::converter::iw8::vision
{
	namespace
	{
		const std::pair<std::string_view, std::string_view> vision_hashes[] // sub_C52C00
		{
			{"clut0Image", "NONMKTKNNM"},
			{"r_ssaoStrength", "NQNPKMQRLT"},
			{"r_rimLightScale", "LLPRLLRRRM"},
			{"r_rimLightStartDistance", "MSNNNKMSSR"},
			{"r_rimLightDistance", "MMMPPKPOMN"},
			{"r_rimLightFill", "NKMONTORPL"},
			{"r_EVCompBounds", "NTLPOKSSTL"},
			{"r_EVCompGrayReference", "NMPKSTQSPM"},
			{"r_skyBlendAmount", "LLQOLROKNQ"},
			{"r_skyBlendFeather", "NSMOTTMQOT"},
			{"fogDensity", "MSTOLQOTKN"},
			{"fogStartDist", "RPNOKKL"},
			{"fogColorIntensity", "NTPTLNNQQL"},
			{"fogColor", "NMLMMNQLSQ"},
			{"fogMaxOpacity", "TRPROKTLS"},
			{"fogAtmosphericScattering", "MKKLQTSKSO"},
			{"tonemapMode", "LTTOTTRLTS"},
			{"tonemapAuto", "NLMSSPLMPK"},
			{"tonemapAutoExposureAdjustCurve", "MRKQQMPTKN"},
			{"tonemapExposure", "LPKQPKSRKQ"},
			{"tonemapAutoExposureAdjust", "NOOTTPORTQ"},
			{"tonemapMaxExposure", "LTNRTLQOKQ"},
			{"tonemapExposureAdjust", "NNRKNLLSQN"},
			{"tonemapAdaptSpeed", "MMQMQKSTS"},
			{"tonemapDarkEv", "LSLPLPRQNO"},
			{"tonemapMidEv", "NLPRMSTSMM"},
			{"tonemapLightEv", "MOPORNQOTQ"},
			{"tonemapDarkExposureAdjust", "MTPSRMRRLP"},
			{"tonemapMidExposureAdjust", "LRPSSPPPPT"},
			{"tonemapLightExposureAdjust", "NMKNKNMKNN"},
			{"tonemapGrainStrength", "LNQNRRPNPP"},
			{"veilStrength", "MMQLTMKTPK"},
			{"volumetricAirDensity", "NSKNLMKQSR"},
		};

		const std::pair<std::string_view, std::string_view> default_values[] // TODO: stock mp_rust values, T9 lighting and postfx are not converted
		{
			{"r_ssaoStrength", "1"},
			{"r_rimLightScale", "0"},
			{"r_rimLightStartDistance", "0"},
			{"r_rimLightDistance", "128"},
			{"r_rimLightFill", "0.25"},
			{"r_EVCompBounds", "0"},
			{"r_EVCompGrayReference", "1"},
			{"r_skyBlendAmount", "0"},
			{"r_skyBlendFeather", "0.001"},
			{"fogDensity", "45000"},
			{"fogStartDist", "512"},
			{"fogColorIntensity", "0"},
			{"fogColor", "1 1 1"},
			{"fogMaxOpacity", "0"},
			{"fogAtmosphericScattering", "0"},
			{"tonemapMode", "3"},
			{"tonemapAuto", "1"},
			{"tonemapAutoExposureAdjustCurve", "1"},
			{"tonemapExposure", "10"},
			{"tonemapAutoExposureAdjust", "0"},
			{"tonemapMaxExposure", "-10"},
			{"tonemapExposureAdjust", "0"},
			{"tonemapAdaptSpeed", "0.06"},
			{"tonemapDarkEv", "7.53"},
			{"tonemapMidEv", "10.3"},
			{"tonemapLightEv", "14"},
			{"tonemapDarkExposureAdjust", "-3"},
			{"tonemapMidExposureAdjust", "-1"},
			{"tonemapLightExposureAdjust", "1.3"},
			{"tonemapGrainStrength", "0.05"},
			{"veilStrength", "0.09"},
			{"volumetricAirDensity", "0"},
		};

		std::string dump_grading_clut(const std::string& map)
		{
			const auto name = map + "_clut";
			const auto size = 32u; // TODO: clutSize from stock mp_rust_clut only
			auto* image = gfximage::dump_identity_grading_clut("gradingclut" + name, size);
			if (!image)
			{
				return {};
			}

			zonetool::iw8::GfxGradingClut grading_clut{};
			grading_clut.name = name.data();
			grading_clut.inputColorimetry = zonetool::iw8::HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT2020_PQ;
			grading_clut.outputColorimetry = zonetool::iw8::HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_BT2020_PQ;
			grading_clut.clutSize = size;
			grading_clut.hdrClutImage = image;
			zonetool::iw8::grading_clut::dump(&grading_clut);
			add_csv_row("gradingclut", name);
			return name;
		}

		std::optional<std::string_view> find_value(const std::string_view name)
		{
			const auto entry = std::ranges::find(default_values, name, &std::pair<std::string_view, std::string_view>::first);
			return entry != std::end(default_values) ? std::optional(entry->second) : std::nullopt;
		}
	}

	void dump(const std::string& map)
	{
		const auto clut = dump_grading_clut(map);

		std::string text;
		std::string unsourced;
		for (const auto& [name, hash] : vision_hashes)
		{
			if (name == "clut0Image")
			{
				if (!clut.empty())
				{
					text += std::format("{} \"{}\"\n", hash, clut);
				}

				continue;
			}

			if (const auto value = find_value(name))
			{
				text += std::format("{} \"{}\"\n", hash, *value);
				unsourced += std::format("{}{}", unsourced.empty() ? "" : ", ", name);
			}
		}

		ZONETOOL_WARNING("vision \"%s\": these values have no proven T9 source and use stock mp_rust values: %s", map.data(), unsourced.data());
		ZONETOOL_WARNING("vision \"%s\": the grading clut is an identity clut, the T9 color grading is not converted", map.data());

		const auto file_name = std::format("vision/{}.vision", map);
		filesystem::file file(file_name);
		file.open("wb");
		file.write(text);
		file.close();
		add_csv_row("rawfile", file_name);
	}
}
