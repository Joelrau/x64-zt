#include <std_include.hpp>

#include "../include.hpp"
#include "rawfile.hpp"

#include <zlib.h>

namespace zonetool::iw7::converter::iw8::rawfile
{
	namespace
	{
		const std::pair<std::string_view, std::string_view> vision_hashes[] // sub_C52C00
		{
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

		std::optional<std::string> convert_vision_value(const std::string_view name, const std::unordered_map<std::string, std::string>& values)
		{
			if (name == "tonemapMode")
			{
				return "\"3\""; // IW8 has no filmic tonemap
			}

			if (name == "fogAtmosphericScattering")
			{
				return "\"0\"";
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
	}

	void dump(RawFile* asset)
	{
		const std::string_view name = asset->name;
		if (!name.starts_with("vision/") || !name.ends_with(".vision") || !asset->len)
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
