#include <std_include.hpp>

#include "../include.hpp"
#include "gradingclut.hpp"
#include "rawfile.hpp"

#include <zlib.h>

namespace zonetool::iw7::converter::iw8::rawfile
{
	std::string read_text(const RawFile* asset)
	{
		if (!asset->compressedLen)
		{
			return std::string(asset->buffer, asset->len);
		}

		std::string text(asset->len, '\0');
		auto size = static_cast<uLongf>(text.size());
		if (uncompress(reinterpret_cast<Bytef*>(text.data()), &size, reinterpret_cast<const Bytef*>(asset->buffer), asset->compressedLen) != Z_OK)
		{
			return {};
		}

		text.resize(size);
		return text;
	}

	namespace
	{
		const std::pair<std::string_view, std::string_view> vision_hashes[] // sub_C52C00
		{
			{"clut0Image", "NONMKTKNNM"},
			{"r_ssaoStrength", "NQNPKMQRLT"},
			{"r_mdaoCullDistance", "MSLPPMPKSM"},
			{"r_mdaoFadeoutDistance", "TQORTRNQ"},
			{"r_mdaoBoneSizeThreshold", "LOTQRPQLTP"},
			{"r_glare_normalStart", "LOTKNSTQSP"},
			{"r_glare_normalEnd", "MNRLLTRRK"},
			{"r_glare_normalPower", "OTTTLQMLK"},
			{"r_glare_normalAmount", "NONQMPOOSP"},
			{"r_glare_mirrorStart", "MTKLNRROLT"},
			{"r_glare_mirrorEnd", "MQMTPPNLMO"},
			{"r_glare_mirrorPower", "MNRTMKMOTM"},
			{"r_glare_mirrorAmount", "MSLLQRMLNT"},
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
			{"fogSunDensity", "MPNPSMMTTT"},
			{"fogSunColorIntensity", "MLORTTKOLN"},
			{"fogSunColor", "MRTOMMTTQM"},
			{"fogSunBeginFadeAngle", "NKLTMMOOLP"},
			{"fogSunEndFadeAngle", "NONQQTPNLT"},
			{"fogSunDir", "RLNQONPKS"},
			{"fogSkyIntensity", "NORQKLTPTR"},
			{"fogSkyMinAngle", "LOROLOTNMP"},
			{"fogSkyMaxAngle", "SPQNRNKQR"},
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
			{"tonemapMinExposureAdjust", "MOSRNQNQTN"},
			{"tonemapMaxExposureAdjust", "NSROSTQOSN"},
			{"tonemapGrainStrength", "LNQNRRPNPP"},
			{"toneWhitePoint", "LRROOOSOMM"},
			{"veilStrength", "MMQLTMKTPK"},
			{"volumetricAirDensity", "NSKNLMKQSR"},
			{"volumetricAbsorption", "MQMPKLKK"},
			{"volumetricAlbedo", "NKKRQROOQK"},
			{"volumetricSunBrightness", "LKNNNKTPTM"},
			{"volumetricSpotBrightness", "MQSQOLLMRM"},
			{"volumetricAmbientBrightness", "MQRSTPRORQ"},
			{"volumetricSunAnisotropy", "MTNSRNMTKN"},
			{"sunshadowSoftness", "MLNTKKPNMM"},
			{"sunshadowSampleSizeNear", "LPSPPMRPMO"},
			{"cloudShadowScale", "MKLRNTOLRK"},
			{"cloudShadowScrollRate", "MKLOMPOMMT"},
			{"cloudShadowScrollHeading", "QMSKSTQKR"},
			{"cloudShadowOpacity", "MMTPPOTOLL"},
			{"cloudShadowLowThreshold", "PNSKMLSLN"},
			{"cloudShadowHighThreshold", "OLOQMTPPLT"},
			{"fogAtmosphericScattering", "MKKLQTSKSO"},
			{"volumetricOmniBrightness", "RRKMLNPKP"},
		};

		std::unordered_map<std::string, std::string> read_vision_values(const std::string& text)
		{
			std::unordered_map<std::string, std::string> values;
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
					values.emplace(line.substr(0, separator), line.substr(separator + 1));
				}
			}

			return values;
		}

		float read_vision_float(const std::unordered_map<std::string, std::string>& values, const std::string& name, const float fallback = 0.0f)
		{
			const auto value = values.find(name);
			if (value == values.end())
			{
				return fallback;
			}

			auto text = value->second;
			std::erase(text, '"');
			return std::strtof(text.data(), nullptr);
		}

		std::array<float, 3> read_vision_vector(const std::unordered_map<std::string, std::string>& values, const std::string& name,
			const std::array<float, 3>& fallback)
		{
			const auto value = values.find(name);
			if (value == values.end())
			{
				return fallback;
			}

			auto text = value->second;
			std::erase(text, '"');
			auto result = fallback;
			std::istringstream(text) >> result[0] >> result[1] >> result[2];
			return result;
		}

		gradingclut::grading read_grading(const std::unordered_map<std::string, std::string>& values)
		{
			gradingclut::grading result{};
			result.filmic.black_point = read_vision_float(values, "tonemapBlackPoint", result.filmic.black_point);
			result.filmic.white_point = read_vision_float(values, "tonemapWhitePoint", result.filmic.white_point);
			result.filmic.crossover = read_vision_float(values, "tonemapCrossover", result.filmic.crossover);
			result.filmic.shoulder = read_vision_float(values, "tonemapShoulder", result.filmic.shoulder);
			result.filmic.toe = read_vision_float(values, "tonemapToe", result.filmic.toe);
			result.curve_low_min = read_vision_float(values, "colorizationSetCurveLowMin", result.curve_low_min);
			result.curve_low_max = read_vision_float(values, "colorizationSetCurveLowMax", result.curve_low_max);
			result.curve_high_min = read_vision_float(values, "colorizationSetCurveHighMin", result.curve_high_min);
			result.curve_high_max = read_vision_float(values, "colorizationSetCurveHighMax", result.curve_high_max);
			result.curve_mid_midpoint = read_vision_float(values, "colorizationSetCurveMidMidpoint", result.curve_mid_midpoint);
			result.curve_mid_range = read_vision_float(values, "colorizationSetCurveMidRange", result.curve_mid_range);
			result.curve_mid_falloff = read_vision_float(values, "colorizationSetCurveMidFalloff", result.curve_mid_falloff);

			const std::string_view band_names[]{"Low", "Mid", "High"};
			for (auto i = 0u; i < result.bands.size(); i++)
			{
				auto& band = result.bands[i];
				band.scale = read_vision_vector(values, std::format("lmh{}Scale", band_names[i]), band.scale);
				band.balance = read_vision_vector(values, std::format("lmh{}Balance", band_names[i]), band.balance);
				band.saturation = read_vision_float(values, std::format("lmh{}Saturation", band_names[i]), band.saturation);
			}

			result.preserve_luminance = read_vision_float(values, "lmhBalancePreserveLum", result.preserve_luminance);
			result.levels_in_min = read_vision_float(values, "levelsInMin", result.levels_in_min);
			result.levels_in_gamma = read_vision_float(values, "levelsInGamma", result.levels_in_gamma);
			result.levels_in_max = read_vision_float(values, "levelsInMax", result.levels_in_max);
			result.levels_out_min = read_vision_float(values, "levelsOutMin", result.levels_out_min);
			result.levels_out_max = read_vision_float(values, "levelsOutMax", result.levels_out_max);
			return result;
		}

		std::optional<std::string> convert_clut(const std::unordered_map<std::string, std::string>& values)
		{
			const auto value = values.find("clut0Image");
			auto image_name = value != values.end() ? value->second : std::string{};
			std::erase(image_name, '"');
			const auto* image = image_name.empty() ? nullptr : static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, image_name.data()));
			if (!image_name.empty() && !image)
			{
				return {};
			}

			const auto converted = gradingclut::dump(image, read_grading(values));
			return converted ? std::optional(std::format("\"{}\"", *converted)) : std::nullopt;
		}

		float get_volumetric_density_scale(const std::unordered_map<std::string, std::string>& values) // iw8 absorption range is 0 to 10
		{
			const auto brightness = std::max({read_vision_float(values, "volumetricSunBrightness"), read_vision_float(values, "volumetricSpotBrightness"),
				read_vision_float(values, "volumetricOmniBrightness"), read_vision_float(values, "volumetricAmbientBrightness")});
			return std::clamp(read_vision_float(values, "volumetricAbsorption"), std::min(brightness, 1.0f), 1.0f);
		}

		std::optional<std::string> convert_vision_value(const std::string_view name, const std::unordered_map<std::string, std::string>& values)
		{
			if (values.contains(std::string(name)))
			{
				const auto density = read_vision_float(values, "volumetricAirDensity");
				const auto absorption = read_vision_float(values, "volumetricAbsorption");
				const auto scale = get_volumetric_density_scale(values);
				if (name == "volumetricAirDensity")
				{
					return std::format("\"{}\"", density * scale);
				}

				if (name == "volumetricAbsorption")
				{
					return std::format("\"{}\"", std::max(0.01f * density * (absorption - scale), 0.0f));
				}

				if (scale > 0.0f && (name == "volumetricSunBrightness" || name == "volumetricSpotBrightness" || name == "volumetricOmniBrightness"
					|| name == "volumetricAmbientBrightness"))
				{
					return std::format("\"{}\"", read_vision_float(values, std::string(name)) / scale);
				}
			}

			if (name == "tonemapMode")
			{
				return "\"3\""; // IW8 has no filmic tonemap
			}

			if (name == "fogAtmosphericScattering" || name == "r_EVCompBounds")
			{
				return "\"0\"";
			}

			if (name == "r_EVCompGrayReference")
			{
				return "\"1\"";
			}

			if (name == "r_ssaoStrength" && values.contains("r_ssaoStrength"))
			{
				return std::format("\"{}\"", std::clamp(read_vision_float(values, "r_ssaoStrength"), 0.0f, 1.0f)); // TODO: range from the game_test vision field table
			}

			if (name == "clut0Image")
			{
				return convert_clut(values);
			}

			const auto value = values.find(std::string(name));
			return value != values.end() ? std::optional(value->second) : std::nullopt;
		}

		std::string convert_vision(const std::string& text)
		{
			const auto values = read_vision_values(text);

			std::string converted;
			for (const auto& [name, hash] : vision_hashes)
			{
				if (const auto value = convert_vision_value(name, values))
				{
					converted += std::format("{} {}\n", hash, *value);
				}
			}

			return converted;
		}

		bool is_createfx_script(const std::string_view name)
		{
			const auto& map = filesystem::get_fastfile();
			const auto folder = get_map_script_folder(map);
			return name == std::format("{}/{}_fx.gsc", folder, map) || name == std::format("{}/gen/{}_fx.gsc", folder, map)
				|| name == std::format("{}/gen/{}_sound.gsc", folder, map);
		}
	}

	std::string get_map_script_folder(const std::string& map)
	{
		return std::format("scripts/{}/maps/{}", map.starts_with("cp_") ? "cp" : "mp", map); // GScr_MainMP_LoadScripts_shared (0xE94369)
	}

	std::array<float, 3> get_sky_color_scale()
	{
		const auto* asset = static_cast<const RawFile*>(db_find_asset(ASSET_TYPE_RAWFILE, std::format("vision/{}.vision", filesystem::get_fastfile()).data()));
		if (!asset)
		{
			return {1.0f, 1.0f, 1.0f};
		}

		const auto values = read_vision_values(read_text(asset));
		const auto intensity = read_vision_float(values, "r_hdrSkyIntensity", 1.0f);
		const auto tint = read_vision_vector(values, "r_hdrSkyColorTint", {1.0f, 1.0f, 1.0f});
		return {intensity * tint[0], intensity * tint[1], intensity * tint[2]};
	}

	void dump(RawFile* asset)
	{
		const std::string_view name = asset->name;
		const auto is_vision = name.starts_with("vision/") && name.ends_with(".vision");
		if ((!is_vision && !is_createfx_script(name)) || !asset->len)
		{
			return;
		}

		if (!mark_dumped(ASSET_TYPE_RAWFILE, name))
		{
			return;
		}

		filesystem::file file(asset->name);
		file.open("wb");
		file.write(is_vision ? convert_vision(read_text(asset)) : read_text(asset));
		file.close();

		add_csv_row("rawfile", name);
	}
}
