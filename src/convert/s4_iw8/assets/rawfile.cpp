#include <std_include.hpp>

#include "../include.hpp"
#include "rawfile.hpp"

#include <zlib.h>

namespace zonetool::s4::converter::iw8::rawfile
{
	namespace
	{
		const std::pair<std::string_view, std::string_view> vision_keys[] // iw8 sub_C52C00
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
			{"fogSunDensity", "MPNPSMMTTT"},
			{"fogSunColorIntensity", "MLORTTKOLN"},
			{"fogSunColor", "MRTOMMTTQM"},
			{"fogSunBeginFadeAngle", "NKLTMMOOLP"},
			{"fogSunEndFadeAngle", "NONQQTPNLT"},
			{"fogSunDir", "RLNQONPKS"},
			{"fogSkyIntensity", "NORQKLTPTR"},
			{"fogSkyMinAngle", "LOROLOTNMP"},
			{"fogSkyMaxAngle", "SPQNRNKQR"},
			{"fogAtmosphericScattering", "MKKLQTSKSO"},
			{"fogHalfPlaneDistance", "SNNNRLTML"},
			{"fogMaxDistance", "NRTMTRQTRQ"},
			{"fogSkyDistance", "NLQKSSPMKP"},
			{"fogExtinctionDesaturation", "MKQSQRPPQR"},
			{"fogInscatteringEv", "LNOQTMMLSP"},
			{"fogRayleighAlbedo", "LNMRMNLNTT"},
			{"fogMieAlbedo", "LRKPNSNORL"},
			{"fogMieAnisotropy", "MPRLMQNQO"},
			{"fogMieBlendStrength", "NPMPRRLNPO"},
			{"fogHeightFogBaseHeight", "NOSTLRKPQ"},
			{"fogHeightFogHalfPlaneDistance", "POKSTQPPQ"},
			{"fogSpline", "MLOSSOSRMM"},
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
			{"whiteBalanceMethod", "MNMNSSLLOR"},
			{"whiteBalanceIlluminant", "NLSRLSTPTN"},
			{"whiteBalanceColorTempK", "TRQLLNNLS"},
			{"chromaRadius", "OMQSRPQPL"},
			{"chromaStrength", "LSOTNTMSNS"},
			{"chromaAberration", "MNPOSMSQOS"},
			{"veilStrength", "MMQLTMKTPK"},
			{"volumetricAirDensity", "NSKNLMKQSR"},
			{"volumetricAbsorption", "MQMPKLKK"},
			{"volumetricAlbedo", "NKKRQROOQK"},
			{"volumetricSunBrightness", "LKNNNKTPTM"},
			{"volumetricSpotBrightness", "MQSQOLLMRM"},
			{"volumetricOmniBrightness", "RRKMLNPKP"},
			{"volumetricAmbientBrightness", "MQRSTPRORQ"},
			{"volumetricSunAnisotropy", "MTNSRNMTKN"},
			{"cloudShadowScale", "MKLRNTOLRK"},
			{"cloudShadowScrollRate", "MKLOMPOMMT"},
			{"cloudShadowScrollHeading", "QMSKSTQKR"},
			{"cloudShadowOpacity", "MMTPPOTOLL"},
			{"cloudShadowLowThreshold", "PNSKMLSLN"},
			{"cloudShadowHighThreshold", "OLOQMTPPLT"},
			{"vignetteIntensity", "NKMMLMPOKK"},
		};

		std::uint64_t hash_vision_key(const std::string_view name)
		{
			auto hash = 0xCBF29CE484222325ull;
			for (const auto c : utils::string::to_lower(std::string(name)))
			{
				hash ^= static_cast<std::uint8_t>(c);
				hash *= 0x100000001B3ull;
			}

			return hash;
		}

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

		std::string convert_vision(const RawFile* asset)
		{
			std::unordered_map<std::uint64_t, std::pair<std::string_view, std::string_view>> keys;
			for (const auto& [name, iw8_key] : vision_keys)
			{
				keys.emplace(hash_vision_key(name), std::pair(name, iw8_key));
			}

			std::string converted;
			std::string dropped;
			std::istringstream stream(read_text(asset));
			std::string line;
			while (std::getline(stream, line))
			{
				const auto separator = line.find(' ');
				if (!line.starts_with("#x") || separator == std::string::npos)
				{
					continue;
				}

				const auto key = keys.find(std::strtoull(line.substr(2, separator - 2).data(), nullptr, 16));
				if (key == keys.end())
				{
					dropped += std::format("{}{}", dropped.empty() ? "" : ", ", line.substr(0, separator));
					continue;
				}

				const auto& [name, iw8_key] = key->second;
				const auto value = name == "tonemapMode" ? "\"3\"" : line.substr(separator + 1); // stock iw8 mode
				converted += std::format("{} {}\n", iw8_key, value);
			}

			if (!dropped.empty())
			{
				ZONETOOL_WARNING("vision \"%s\" drops the unknown keys %s", asset->name, dropped.data());
			}

			return converted;
		}
	}

	bool is_vision(const RawFile* asset)
	{
		const std::string_view name = asset->name;
		return name.starts_with("vision/") && name.ends_with(".vision");
	}

	void dump_vision(const RawFile* asset)
	{
		filesystem::file file(asset->name);
		file.open("wb");
		file.write(convert_vision(asset));
		file.close();

		add_csv_row("rawfile", asset->name);
	}
}
