#include <std_include.hpp>

#include "../include.hpp"
#include "rawfile.hpp"
#include "gfximage.hpp"

#include <zlib.h>

namespace zonetool::iw8::converter::iw7::rawfile
{
	namespace
	{
		const std::vector<vision_field> vision_fields // sub_C52C00, defaults of visionDefFields
		{
			{"NQNPKMQRLT", "r_ssaoStrength", "1"},
			{"MSLPPMPKSM", "r_mdaoCullDistance", "1000"},
			{"TQORTRNQ", "r_mdaoFadeoutDistance", "900"},
			{"LOTQRPQLTP", "r_mdaoBoneSizeThreshold", "150"},
			{"LLQOLROKNQ", "r_skyBlendAmount", "0"},
			{"NSMOTTMQOT", "r_skyBlendFeather", "0.001"},
			{"MSTOLQOTKN", "fogDensity", "70000"},
			{"RPNOKKL", "fogStartDist", "0.1"},
			{"NTPTLNNQQL", "fogColorIntensity", "1"},
			{"NMLMMNQLSQ", "fogColor", "1 1 1"},
			{"TRPROKTLS", "fogMaxOpacity", "1"},
			{"MPNPSMMTTT", "fogSunDensity", "70000"},
			{"MLORTTKOLN", "fogSunColorIntensity", "1"},
			{"MRTOMMTTQM", "fogSunColor", "1 1 1"},
			{"NKLTMMOOLP", "fogSunBeginFadeAngle", "0"},
			{"NONQQTPNLT", "fogSunEndFadeAngle", "360"},
			{"RLNQONPKS", "fogSunDir", "1 0 0"},
			{"NORQKLTPTR", "fogSkyIntensity", "0"},
			{"LOROLOTNMP", "fogSkyMinAngle", "0"},
			{"SPQNRNKQR", "fogSkyMaxAngle", "90"},
			{"LNQNRRPNPP", "tonemapGrainStrength", "0"},
			{"LTTOTTRLTS", "tonemapMode", "3"},
			{"NLMSSPLMPK", "tonemapAuto", "0"},
			{"MRKQQMPTKN", "tonemapAutoExposureAdjustCurve", "0"},
			{"LPKQPKSRKQ", "tonemapExposure", "10"},
			{"NOOTTPORTQ", "tonemapAutoExposureAdjust", "0"},
			{"LTNRTLQOKQ", "tonemapMaxExposure", "-10"},
			{"NNRKNLLSQN", "tonemapExposureAdjust", "0"},
			{"MMQMQKSTS", "tonemapAdaptSpeed", "0.02"},
			{"LSLPLPRQNO", "tonemapDarkEv", "2.84"},
			{"NLPRMSTSMM", "tonemapMidEv", "7.823"},
			{"MOPORNQOTQ", "tonemapLightEv", "12.81"},
			{"MTPSRMRRLP", "tonemapDarkExposureAdjust", "-3.17"},
			{"LRPSSPPPPT", "tonemapMidExposureAdjust", "-0.651"},
			{"NMKNKNMKNN", "tonemapLightExposureAdjust", "1.47"},
			{"MOSRNQNQTN", "tonemapMinExposureAdjust", "-3.17"},
			{"NSROSTQOSN", "tonemapMaxExposureAdjust", "2.3"},
			{"LRROOOSOMM", "toneWhitePoint", "1 1 1"},
			{"MLNTKKPNMM", "sunshadowSoftness", "0"},
			{"LPSPPMRPMO", "sunshadowSampleSizeNear", "0.25"},
			{"NKSQTPNOOK", "r_volumeLightScatter", "0"},
			{"LTQQSMPMKN", "r_volumeLightScatterLinearAtten", "1"},
			{"LSNRTORRKM", "r_volumeLightScatterQuadraticAtten", "0"},
			{"NLLMLNPMPM", "r_volumeLightScatterAngularAtten", "0.25"},
			{"MNQPPKSLMR", "r_volumeLightScatterDepthAttenNear", "0"},
			{"MRQKRNTMRK", "r_volumeLightScatterDepthAttenFar", "0"},
			{"LQLPRTOQKP", "r_volumeLightScatterBackgroundDistance", "100000"},
			{"SLLNNNQSP", "r_volumeLightScatterColor", "0.2 0.2 0.2"},
			{"OMOQSPLSST", "r_primaryLightTweakDiffuseStrength", "1"},
			{"NPNRRNMSK", "r_primaryLightTweakSpecularStrength", "1"},
		};

		const std::vector<std::pair<std::string_view, std::string_view>> iw7_vision_defaults // vision text at 0x144324C20
		{
			{"colorizationSetCurveLowMin", "0"},
			{"colorizationSetCurveLowMax", "1"},
			{"colorizationSetCurveHighMin", "1"},
			{"colorizationSetCurveHighMax", "0"},
			{"colorizationSetCurveMidMidpoint", "0.5"},
			{"colorizationSetCurveMidRange", "0"},
			{"colorizationSetCurveMidFalloff", "0.5"},
			{"lmhLowScale", "1 1 1"},
			{"lmhLowBalance", "0 0 0"},
			{"lmhLowSaturation", "0"},
			{"lmhMidScale", "1 1 1"},
			{"lmhMidBalance", "0 0 0"},
			{"lmhMidSaturation", "0"},
			{"lmhHighScale", "1 1 1"},
			{"lmhHighBalance", "0 0 0"},
			{"lmhHighSaturation", "0"},
			{"lmhBalancePreserveLum", "1"},
			{"levelsInMin", "0"},
			{"levelsInGamma", "1"}, // 1.4 in the vision text
			{"levelsInMax", "255"},
			{"levelsOutMin", "0"},
			{"levelsOutMax", "255"},
			{"clut0Image", ""},
			{"r_primaryLightTweakDiffuseStrength", "1"},
			{"r_primaryLightTweakSpecularStrength", "1"},
			{"r_charLightAmbient", "0.3764706 0.3764706 0.3764706"},
			{"r_viewModelPrimaryLightTweakDiffuseStrength", "1"},
			{"r_viewModelPrimaryLightTweakSpecularStrength", "1"},
			{"r_viewModelLightAmbient", "0 0 0"},
			{"r_volumeLightScatter", "0"},
			{"r_volumeLightScatterLinearAtten", "1"},
			{"r_volumeLightScatterQuadraticAtten", "0"},
			{"r_volumeLightScatterAngularAtten", "0.25"},
			{"r_volumeLightScatterDepthAttenNear", "0"},
			{"r_volumeLightScatterDepthAttenFar", "0"},
			{"r_volumeLightScatterBackgroundDistance", "100000"},
			{"r_volumeLightScatterColor", "0.2 0.2 0.2"},
			{"r_ssaoStrength", "1"},
			{"r_mdaoCullDistance", "1000"},
			{"r_mdaoFadeoutDistance", "900"},
			{"r_mdaoBoneSizeThreshold", "150"},
			{"r_glare_normalStart", "0.125"},
			{"r_glare_normalEnd", "0.75"},
			{"r_glare_normalPower", "1"},
			{"r_glare_normalAmount", "0"},
			{"r_glare_mirrorStart", "0.125"},
			{"r_glare_mirrorEnd", "0.75"},
			{"r_glare_mirrorPower", "1"},
			{"r_glare_mirrorAmount", "0"},
			{"r_rimLightScale", "0"},
			{"r_rimLightStartDistance", "100"},
			{"r_rimLightDistance", "300"},
			{"r_rimLightFill", "0"},
			{"r_EVCompBounds", "0"},
			{"r_EVCompGrayReference", "0.18"},
			{"r_hdrSkyIntensity", "4"},
			{"r_hdrSkyColorTint", "1 1 1"},
			{"r_skyBlendAmount", "0"},
			{"r_skyBlendFeather", "0.001"},
			{"fogDensity", "10000"},
			{"fogStartDist", "500"},
			{"fogColorIntensity", "0.553"},
			{"fogColor", "0.254901975 0.196078435 0.196078435"},
			{"fogMaxOpacity", "0.1"},
			{"fogSunDensity", "70000"},
			{"fogSunColorIntensity", "2"},
			{"fogSunColor", "0.345098048 0.298039228 0.3019608"},
			{"fogSunBeginFadeAngle", "0"},
			{"fogSunEndFadeAngle", "150"},
			{"fogSunDir", "-0.339999944 0.03999997 0.05999999"},
			{"fogSkyIntensity", "0.75"},
			{"fogSkyMinAngle", "0"},
			{"fogSkyMaxAngle", "90"},
			{"tonemapMode", "2"},
			{"tonemapAuto", "1"},
			{"tonemapAutoExposureAdjustCurve", "0"},
			{"tonemapExposure", "12.75"},
			{"tonemapAutoExposureAdjust", "0"},
			{"tonemapMaxExposure", "12.75"},
			{"tonemapExposureAdjust", "0"},
			{"tonemapAdaptSpeed", "0.02"},
			{"tonemapBlackPoint", "0"},
			{"tonemapWhitePoint", "2000"},
			{"tonemapCrossover", "0.5"},
			{"tonemapShoulder", "0.995"},
			{"tonemapToe", "0.3"},
			{"tonemapDarkEv", "2.84"},
			{"tonemapMidEv", "7.823"},
			{"tonemapLightEv", "12.81"},
			{"tonemapDarkExposureAdjust", "-0.1"},
			{"tonemapMidExposureAdjust", "0"},
			{"tonemapLightExposureAdjust", "0.1"},
			{"tonemapMinExposureAdjust", "-3.17"},
			{"tonemapMaxExposureAdjust", "2.3"},
			{"tonemapGrainStrength", "0.07"},
			{"chromaRadius", "0"},
			{"chromaStrength", "0"},
			{"chromaAberration", "0"},
			{"r_dustHeading", "0"},
			{"r_dustPitch", "-90"},
			{"r_dustTiling", "0.1"},
			{"r_dustIntensity", "0"},
			{"r_dustPowerCurv", "1"},
			{"r_dustSmoothMin", "0.5"},
			{"r_dustSmoothMax", "1"},
			{"analogRewindAmount", "0"},
			{"analogInterferenceAmount", "0"},
			{"analogCrtEffectAmount", "0"},
			{"analogRollEffectAmount", "0"},
			{"analogChromaSeparationEffectAmount", "0"},
			{"toneWhitePoint", "1 1 1"},
			{"veilStrength", "0.01"},
			{"volumetricAirDensity", "0"},
			{"volumetricAbsorption", "0"},
			{"volumetricAlbedo", "1 1 1"},
			{"volumetricSunBrightness", "0"},
			{"volumetricSpotBrightness", "0"},
			{"volumetricOmniBrightness", "1"},
			{"volumetricAmbientBrightness", "1"},
			{"volumetricSunAnisotropy", "0.15"},
			{"sunshadowTransmissiveTint", "0 0 0"},
			{"sunshadowSoftness", "0"},
			{"sunshadowSampleSizeNear", "1.085"},
			{"cloudShadowScale", "1"},
			{"cloudShadowScrollRate", "0"},
			{"cloudShadowScrollHeading", "0"},
			{"cloudShadowOpacity", "0"},
			{"cloudShadowLowThreshold", "0"},
			{"cloudShadowHighThreshold", "1"},
		};

		std::string read_text(const RawFile* asset)
		{
			std::string text(asset->len, '\0');
			auto size = static_cast<uLongf>(text.size());
			if (uncompress(reinterpret_cast<Bytef*>(text.data()), &size, reinterpret_cast<const Bytef*>(asset->buffer), asset->compressedLen) != Z_OK)
			{
				return {};
			}

			text.resize(size);
			return text;
		}

		std::vector<std::pair<std::string, std::string>> read_vision_values(const std::string& text)
		{
			std::vector<std::pair<std::string, std::string>> values;
			std::istringstream stream(text);
			std::string line;
			while (std::getline(stream, line))
			{
				if (line.ends_with('\r'))
				{
					line.pop_back();
				}

				const auto separator = line.find(' ');
				if (separator != std::string::npos)
				{
					values.emplace_back(line.substr(0, separator), line.substr(separator + 1));
				}
			}

			return values;
		}

		std::optional<std::string> find_vision_value(const std::vector<std::pair<std::string, std::string>>& values, const std::string_view key)
		{
			const auto value = std::ranges::find(values, key, [](const auto& entry) { return std::string_view(entry.first); });
			return value != values.end() ? std::optional(value->second) : std::nullopt;
		}

		std::string convert_vision_value(const std::string_view name, const std::string& value, const std::vector<std::pair<std::string, std::string>>& values)
		{
			if (name == "tonemapMode")
			{
				return "\"2\""; // IW7 supports only filmic
			}

			if (find_vision_value(values, "MKKLQTSKSO") != "\"1\"") // fogAtmosphericScattering
			{
				return value;
			}

			const auto half_plane_distance = find_vision_value(values, "SNNNRLTML"); // fogHalfPlaneDistance
			const auto mie_albedo = find_vision_value(values, "LRKPNSNORL"); // fogMieAlbedo
			const auto color_intensity = find_vision_value(values, "NTPTLNNQQL"); // fogColorIntensity
			if ((name == "fogDensity" || name == "fogSunDensity") && half_plane_distance)
			{
				return *half_plane_distance;
			}

			if (name == "fogColor" && mie_albedo)
			{
				return *mie_albedo;
			}

			if (name == "fogSunColorIntensity" && color_intensity)
			{
				return *color_intensity;
			}

			return value;
		}

		std::optional<std::string> find_iw8_value(const std::vector<std::pair<std::string, std::string>>& values, const std::string_view iw7_name)
		{
			const auto field = std::ranges::find(vision_fields, iw7_name, &vision_field::iw7_name);
			if (field == vision_fields.end())
			{
				return std::nullopt;
			}

			const auto value = find_vision_value(values, field->iw8_name).value_or(std::format("\"{}\"", field->iw8_default));
			if (iw7_name == "tonemapMaxExposure" && value == "\"-10\"")
			{
				return std::nullopt; // IW8 -10 is no limit
			}

			return convert_vision_value(iw7_name, value, values);
		}

		std::optional<std::string> dump_grading_clut(const std::vector<std::pair<std::string, std::string>>& values)
		{
			const auto value = find_vision_value(values, "NONMKTKNNM");
			if (!value || value->size() < 2)
			{
				return {};
			}

			const auto name = value->substr(1, value->size() - 2);
			const auto* clut = static_cast<GfxGradingClut*>(db_find_asset(ASSET_TYPE_GRADING_CLUT, name));
			const auto image = clut ? gfximage::dump_grading_clut(clut) : std::nullopt;
			if (image)
			{
				add_csv_row("image", *image);
			}

			return image;
		}

		std::string convert_vision(const std::string& text)
		{
			const auto values = read_vision_values(text);
			const auto clut = dump_grading_clut(values);

			std::string converted;
			for (const auto& [name, value] : iw7_vision_defaults)
			{
				if (name == "clut0Image")
				{
					if (clut)
					{
						converted += std::format("clut0Image \"{}\"\n", *clut);
					}
					continue;
				}

				converted += std::format("{} {}\n", name, find_iw8_value(values, name).value_or(std::format("\"{}\"", value)));
			}

			return converted;
		}
	}

	void dump(RawFile* asset)
	{
		const std::string_view name = asset->name;
		if (!name.starts_with("vision/") || !name.ends_with(".vision") || !asset->compressedLen)
		{
			return;
		}

		if (!mark_dumped(ASSET_TYPE_RAWFILE, name))
		{
			return;
		}

		filesystem::file file(asset->name);
		file.open("wb");
		file.write(convert_vision(read_text(asset)));
		file.close();

		add_csv_row("rawfile", name);
	}
}
