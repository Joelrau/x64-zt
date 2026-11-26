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

		const std::pair<std::string_view, std::string_view> white_balance_keys[]
		{
			{"whiteBalanceMethod", "MNMNSSLLOR"},
			{"whiteBalanceIlluminant", "NLSRLSTPTN"},
			{"whiteBalanceColorTempK", "TRQLLNNLS"},
			{"whiteBalanceTint", "OPSMSQNKQ"},
		};

		using matrix3 = std::array<double, 9>;
		using vector3 = std::array<double, 3>;
		using chromaticity = std::array<double, 2>;

		const matrix3 white_balance_cats[] // iw8 0x23E6950, s4 0x7BD3420
		{
			{3.2404542, -1.53713846, -0.498531401, -0.969265997, 1.87601078, 0.0415560007, 0.0556433983, -0.204025894, 1.05722523},
			{0.732800007, 0.4296, -0.162400007, -0.703599989, 1.69749999, 0.00609999988, 0.00300000003, 0.0136000002, 0.983399987},
			{1.2694, -0.0988000035, -0.170599997, -0.836399972, 1.80060005, 0.0357000008, 0.0296999998, -0.0315000005, 1.00179994},
			{0.895099998, 0.266400009, -0.161400005, -0.750199974, 1.71350002, 0.0366999991, 0.0388999991, -0.0684999973, 1.02960002},
			{0.400240004, 0.707599998, -0.080810003, -0.226300001, 1.16532004, 0.0456999987, 0, 0, 0.918219984},
			{1, 0, 0, 0, 1, 0, 0, 0, 1},
		};

		const matrix3 xyz_to_srgb{3.2404542, -1.53713846, -0.498531401, -0.969265997, 1.87601078, 0.0415560007, 0.0556433983, -0.204025894, 1.05722523};
		const matrix3 s4_srgb_to_xyz{0.412456393, 0.357576102, 0.180437505, 0.212672904, 0.715152204, 0.0721750036, 0.0193339009, 0.119191997, 0.950304091}; // D65, s4 0x7BD3520
		const matrix3 iw8_srgb_to_xyz{0.436074704, 0.3850649, 0.143080398, 0.222504497, 0.716878593, 0.0606168993, 0.0139322001, 0.0971044973, 0.714173317}; // D50, iw8 0x23E6A50

		const std::array<double, 3> white_balance_illuminants[] // x, y, temperature; iw8 0x411C190, s4 0x97352E0
		{
			{0.34567, 0.3585, 5003}, {0.34567, 0.3585, 5003}, {0.34567, 0.3585, 5003}, {0.44757, 0.40745, 2856},
			{0.34842, 0.35161, 4874}, {0.31006, 0.31616, 6774}, {0.34567, 0.3585, 5003}, {0.33242, 0.34743, 5503},
			{0.31271, 0.32902, 6504}, {0.29902, 0.31485, 7504}, {0.3131, 0.33727, 6430}, {0.37208, 0.37529, 4230},
			{0.4091, 0.3943, 3450}, {0.44018, 0.40329, 2940}, {0.31379, 0.34531, 6350}, {0.3779, 0.38835, 4150},
			{0.31292, 0.32933, 6500}, {0.34588, 0.35875, 5000}, {0.37417, 0.37281, 4150}, {0.34609, 0.35986, 5000},
			{0.38052, 0.37713, 4000}, {0.43695, 0.40441, 3000},
		};

		matrix3 multiply(const matrix3& a, const matrix3& b)
		{
			matrix3 result{};
			for (auto row = 0; row < 3; row++)
			{
				for (auto column = 0; column < 3; column++)
				{
					for (auto i = 0; i < 3; i++)
					{
						result[row * 3 + column] += a[row * 3 + i] * b[i * 3 + column];
					}
				}
			}

			return result;
		}

		vector3 multiply(const matrix3& m, const vector3& v)
		{
			return {
				m[0] * v[0] + m[1] * v[1] + m[2] * v[2],
				m[3] * v[0] + m[4] * v[1] + m[5] * v[2],
				m[6] * v[0] + m[7] * v[1] + m[8] * v[2],
			};
		}

		matrix3 inverse(const matrix3& m)
		{
			const matrix3 adjugate
			{
				m[4] * m[8] - m[5] * m[7], m[2] * m[7] - m[1] * m[8], m[1] * m[5] - m[2] * m[4],
				m[5] * m[6] - m[3] * m[8], m[0] * m[8] - m[2] * m[6], m[2] * m[3] - m[0] * m[5],
				m[3] * m[7] - m[4] * m[6], m[1] * m[6] - m[0] * m[7], m[0] * m[4] - m[1] * m[3],
			};

			const auto determinant = m[0] * adjugate[0] + m[1] * adjugate[3] + m[2] * adjugate[6];
			matrix3 result{};
			for (auto i = 0; i < 9; i++)
			{
				result[i] = adjugate[i] / determinant;
			}

			return result;
		}

		chromaticity daylight_chromaticity(const double temperature) // iw8 sub_EF7FE0
		{
			const auto t = 1.0 / temperature;
			const auto x = temperature > 7000.0
				? 0.23704 + 247.48 * t + 1901800.0 * t * t - 2006400000.0 * t * t * t
				: 0.244063 + 99.11 * t + 2967800.0 * t * t - 4607000064.0 * t * t * t;
			return {x, (2.87 - 3.0 * x) * x - 0.275};
		}

		chromaticity blackbody_chromaticity(const double temperature) // iw8 0xEF883D
		{
			const auto t = 1.0 / temperature;
			const auto t2 = t * t;
			const auto t3 = t2 * t;
			const auto t4 = t2 * t2;
			const auto t5 = t4 * t;
			const auto x = 0.243185 + 164.352 * t + 2888860.0 * t2 - 6615549952.0 * t3 + 5842660229120.0 * t4 - 1873809942511616.0 * t5;
			const auto y = 2.87806 * x - 0.291251 - x * (0.70878 + 479.01544 * t + 8419785.0 * t2 - 19281483776.0 * t3 + 17028842782720.0 * t4 - 5461350490832896.0 * t5);
			return {x, y};
		}

		chromaticity apply_tint(const chromaticity& white, const double temperature, const double tint, const bool daylight) // iw8 sub_EF8090
		{
			const auto to_uv = [](const chromaticity& c) -> chromaticity
			{
				const auto d = 6.0 * c[1] - c[0] + 1.5;
				return {2.0 * c[0] / d, 3.0 * c[1] / d};
			};

			const auto uv = to_uv(white);
			const auto next = to_uv(daylight ? daylight_chromaticity(temperature + 1.0) : blackbody_chromaticity(temperature + 1.0));
			auto du = next[0] - uv[0];
			auto dv = next[1] - uv[1];
			auto length = std::sqrt(du * du + dv * dv);
			if (!(length > 0.0))
			{
				length = 1.0;
			}

			du /= length;
			dv /= length;

			const auto scale = tint / 3000.0;
			const auto u = uv[0] + dv * scale;
			const auto v = uv[1] - du * scale;
			const auto d = u - 4.0 * v + 2.0;
			auto x = std::clamp(1.5 * u / d, 0.000001, 0.999999);
			auto y = std::clamp(v / d, 0.000001, 0.999999);
			const auto sum = x + y;
			if (sum > 0.999999)
			{
				x *= 0.999999 / sum;
				y *= 0.999999 / sum;
			}

			return {x, y};
		}

		std::optional<chromaticity> white_balance_chromaticity(const int illuminant, const double temperature, const double tint)
		{
			if (illuminant == 1)
			{
				return apply_tint(daylight_chromaticity(temperature), temperature, tint, true);
			}

			if (illuminant == 2)
			{
				return apply_tint(blackbody_chromaticity(temperature), temperature, tint, false);
			}

			if (illuminant >= 3 && illuminant < static_cast<int>(std::size(white_balance_illuminants)))
			{
				const auto& [x, y, illuminant_temperature] = white_balance_illuminants[illuminant];
				return apply_tint({x, y}, illuminant_temperature, tint, false);
			}

			return {};
		}

		matrix3 white_balance_matrix(const int method, const chromaticity& white, const matrix3& srgb_to_xyz) // iw8 sub_EF8500, s4 sub_2AD4150
		{
			const auto& cat = white_balance_cats[method];
			const auto target = multiply(cat, vector3{0.95047, 1.0, 1.0888});
			const auto source = multiply(cat, vector3{white[0] / white[1], 1.0, (1.0 - white[0] - white[1]) / white[1]});
			const matrix3 gain{target[0] / source[0], 0, 0, 0, target[1] / source[1], 0, 0, 0, target[2] / source[2]};
			return multiply(multiply(multiply(multiply(xyz_to_srgb, inverse(cat)), gain), cat), srgb_to_xyz);
		}

		std::string convert_white_balance(const RawFile* asset, const std::unordered_map<std::string_view, std::string>& values)
		{
			const auto value = [&](const std::string_view name, const double fallback)
			{
				const auto entry = values.find(name);
				if (entry == values.end())
				{
					return fallback;
				}

				auto text = entry->second;
				std::erase(text, '"');
				return std::strtod(text.data(), nullptr);
			};

			const auto method = static_cast<int>(value("whiteBalanceMethod", 0.0));
			const auto illuminant = static_cast<int>(value("whiteBalanceIlluminant", 0.0));
			const auto temperature = value("whiteBalanceColorTempK", 5000.0);
			const auto tint = value("whiteBalanceTint", 0.0);

			const auto copy_values = [&]
			{
				std::string converted;
				for (const auto& [name, iw8_key] : white_balance_keys)
				{
					const auto entry = values.find(name);
					if (entry != values.end())
					{
						converted += std::format("{} {}\n", iw8_key, entry->second);
					}
				}

				return converted;
			};

			const auto white = white_balance_chromaticity(illuminant, temperature, tint);
			if (method < 0 || method >= static_cast<int>(std::size(white_balance_cats)) || (illuminant == 0 && method == 0))
			{
				return copy_values();
			}

			if (!white)
			{
				ZONETOOL_WARNING("vision \"%s\" white balance method %d with illuminant %d is not converted", asset->name, method, illuminant);
				return copy_values();
			}

			const auto s4_matrix = white_balance_matrix(method, *white, s4_srgb_to_xyz);
			auto best_error = std::numeric_limits<double>::max();
			auto best_method = 0;
			auto best_temperature = 0;
			for (auto candidate_method = 0; candidate_method < static_cast<int>(std::size(white_balance_cats)); candidate_method++)
			{
				for (auto candidate_temperature = 2000; candidate_temperature <= 40000; candidate_temperature += 10)
				{
					const auto iw8_matrix = white_balance_matrix(candidate_method, daylight_chromaticity(candidate_temperature), iw8_srgb_to_xyz);
					auto error = 0.0;
					for (auto i = 0; i < 9; i++)
					{
						error = std::max(error, std::abs(iw8_matrix[i] - s4_matrix[i]));
					}

					if (error < best_error)
					{
						best_error = error;
						best_method = candidate_method;
						best_temperature = candidate_temperature;
					}
				}
			}

			return std::format("{} \"{}\"\n{} \"1\"\n{} \"{}\"\n", white_balance_keys[0].second, best_method, white_balance_keys[1].second, white_balance_keys[2].second, best_temperature);
		}

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
			std::unordered_map<std::string_view, std::string> white_balance;
			std::istringstream stream(read_text(asset));
			std::string line;
			while (std::getline(stream, line))
			{
				const auto separator = line.find(' ');
				if (!line.starts_with("#x") || separator == std::string::npos)
				{
					continue;
				}

				const auto hash = std::strtoull(line.substr(2, separator - 2).data(), nullptr, 16);
				const auto value = line.substr(separator + 1);
				if (hash == 0xDA61A1BF5CFCE364) // s4 white balance tint
				{
					white_balance["whiteBalanceTint"] = value;
					continue;
				}

				const auto white_balance_key = std::ranges::find_if(white_balance_keys, [&](const auto& entry) { return hash_vision_key(entry.first) == hash; });
				if (white_balance_key != std::end(white_balance_keys))
				{
					white_balance[white_balance_key->first] = value;
					continue;
				}

				const auto key = keys.find(hash);
				if (key == keys.end())
				{
					dropped += std::format("{}{}", dropped.empty() ? "" : ", ", line.substr(0, separator));
					continue;
				}

				const auto& [name, iw8_key] = key->second;
				converted += std::format("{} {}\n", iw8_key, name == "tonemapMode" ? "\"3\"" : value); // stock iw8 mode
			}

			if (!dropped.empty())
			{
				ZONETOOL_WARNING("vision \"%s\" drops the unknown keys %s", asset->name, dropped.data());
			}

			return converted + convert_white_balance(asset, white_balance);
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
