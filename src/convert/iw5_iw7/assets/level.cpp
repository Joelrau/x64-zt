#include <std_include.hpp>

#include "../include.hpp"
#include "level.hpp"
#include "particle_system.hpp"

#include <iw7/assets/netconststrings.hpp>
#include <iw7/assets/fxworld.hpp>
#include <iw7/assets/glassworld.hpp>
#include <iw7/assets/scriptfile.hpp>

#include <utils/compression.hpp>
#include <utils/string.hpp>

#include <xsk/gsc/engine/iw7.hpp>

namespace zonetool::iw5::converter::iw7
{
	namespace level
	{
		namespace
		{
			struct vision_key
			{
				const char* key;
				const char* value;
			};

			const vision_key vision_schema[] =
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
				{"levelsInGamma", "1"},
				{"levelsInMax", "255"},
				{"levelsOutMin", "0"},
				{"levelsOutMax", "255"},
				{"r_primaryLightTweakDiffuseStrength", "1"},
				{"r_primaryLightTweakSpecularStrength", "1"},
				{"r_charLightAmbient", "1 1 1"},
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
				{"r_rimLightScale", "8"},
				{"r_rimLightStartDistance", "100"},
				{"r_rimLightDistance", "900"},
				{"r_rimLightFill", "2"},
				{"r_EVCompBounds", "0"},
				{"r_EVCompGrayReference", "0.18"},
				{"r_hdrSkyIntensity", "1"},
				{"r_hdrSkyColorTint", "1 1 1"},
				{"r_skyBlendAmount", "0"},
				{"r_skyBlendFeather", "0.001"},
				{"fogDensity", "50000"},
				{"fogStartDist", "0"},
				{"fogColorIntensity", "1"},
				{"fogColor", "1 1 1"},
				{"fogMaxOpacity", "0"},
				{"fogSunDensity", "50000"},
				{"fogSunColorIntensity", "0"},
				{"fogSunColor", "1 1 1"},
				{"fogSunBeginFadeAngle", "0"},
				{"fogSunEndFadeAngle", "55"},
				{"fogSunDir", "0 0 -1"},
				{"fogSkyIntensity", "0"},
				{"fogSkyMinAngle", "0"},
				{"fogSkyMaxAngle", "90"},
				{"tonemapMode", "2"},
				{"tonemapAuto", "1"},
				{"tonemapAutoExposureAdjustCurve", "0"},
				{"tonemapExposure", "10"},
				{"tonemapAutoExposureAdjust", "0"},
				{"tonemapMaxExposure", "9.75"},
				{"tonemapExposureAdjust", "-0.5"},
				{"tonemapAdaptSpeed", "0.05"},
				{"tonemapBlackPoint", "0"},
				{"tonemapWhitePoint", "512.0"},
				{"tonemapCrossover", "2"},
				{"tonemapShoulder", "0.96"},
				{"tonemapToe", "0"},
				{"tonemapDarkEv", "2.84"},
				{"tonemapMidEv", "7.823"},
				{"tonemapLightEv", "12.81"},
				{"tonemapDarkExposureAdjust", "-3.17"},
				{"tonemapMidExposureAdjust", "-0.651"},
				{"tonemapLightExposureAdjust", "1.47"},
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
				{"veilStrength", "0.06"},
				{"volumetricAirDensity", "0.02"},
				{"volumetricAbsorption", "0"},
				{"volumetricAlbedo", "1 1 1"},
				{"volumetricSunBrightness", "0"},
				{"volumetricSpotBrightness", "0"},
				{"volumetricAmbientBrightness", "0"},
				{"volumetricSunAnisotropy", "0.8"},
				{"sunshadowTransmissiveTint", "0 0 0"},
				{"sunshadowSoftness", "0"},
				{"sunshadowSampleSizeNear", "0.99"},
				{"cloudShadowScale", "1"},
				{"cloudShadowScrollRate", "0"},
				{"cloudShadowScrollHeading", "0"},
				{"cloudShadowOpacity", "0"},
				{"cloudShadowLowThreshold", "0"},
				{"cloudShadowHighThreshold", "1"},
			};

			using vision_values = std::vector<std::pair<std::string, std::string>>;

			vision_values parse_vision(const std::string& text)
			{
				vision_values result;
				std::istringstream stream(text);
				std::string line;
				while (std::getline(stream, line))
				{
					if (!line.empty() && line.back() == '\r')
					{
						line.pop_back();
					}

					const auto key_start = line.find_first_not_of(" \t");
					if (key_start == std::string::npos || line.compare(key_start, 2, "//") == 0)
					{
						continue;
					}

					const auto key_end = line.find_first_of(" \t", key_start);
					if (key_end == std::string::npos)
					{
						continue;
					}

					const auto value_start = line.find_first_not_of(" \t", key_end);
					if (value_start == std::string::npos)
					{
						continue;
					}

					std::string value;
					if (line[value_start] == '"')
					{
						const auto close = line.find('"', value_start + 1);
						value = line.substr(value_start + 1, close == std::string::npos ? std::string::npos : close - value_start - 1);
					}
					else
					{
						const auto value_end = line.find_first_of(" \t", value_start);
						value = line.substr(value_start, value_end == std::string::npos ? std::string::npos : value_end - value_start);
					}

					result.emplace_back(line.substr(key_start, key_end - key_start), value);
				}
				return result;
			}

			std::string* find_vision_key(vision_values& values, const char* key)
			{
				for (auto& [name, value] : values)
				{
					if (_stricmp(name.data(), key) == 0)
					{
						return &value;
					}
				}
				return nullptr;
			}

			std::string strip_script_comments(const std::string& text)
			{
				std::string result;
				for (std::size_t i = 0; i < text.size(); i++)
				{
					if (text[i] == '"')
					{
						const auto begin = i++;
						while (i < text.size() && text[i] != '"')
						{
							i += text[i] == '\\' ? 2 : 1;
						}
						result.append(text, begin, i - begin + 1);
					}
					else if (text.compare(i, 2, "//") == 0)
					{
						i = std::min(text.find('\n', i), text.size()) - 1;
					}
					else if (text.compare(i, 2, "/*") == 0)
					{
						const auto end = text.find("*/", i + 2);
						i = end == std::string::npos ? text.size() : end + 1;
					}
					else
					{
						result.push_back(text[i]);
					}
				}

				return result;
			}

			float vision_float(const std::string& text, const float fallback)
			{
				char* end = nullptr;
				const auto result = std::strtof(text.data(), &end);
				return end == text.data() ? fallback : result;
			}

			void map_film(vision_values& source, vision_values& result)
			{
				const auto* enable = find_vision_key(source, "r_filmEnable");
				if (!enable || vision_float(*enable, 0.0f) == 0.0f)
				{
					return;
				}

				const std::pair<const char*, const char*> tints[] =
				{
					{"r_filmDarkTint", "lmhLowScale"},
					{"r_filmMediumTint", "lmhMidScale"},
					{"r_filmLightTint", "lmhHighScale"},
				};

				for (const auto& [from, to] : tints)
				{
					if (const auto* tint = find_vision_key(source, from))
					{
						*find_vision_key(result, to) = *tint;
					}
				}

				if (const auto* desaturation = find_vision_key(source, "r_filmDesaturation"))
				{
					const auto saturation = std::format("{}", std::clamp(-100.0f * vision_float(*desaturation, 0.0f), -100.0f, 100.0f));
					*find_vision_key(result, "lmhLowSaturation") = saturation;
					*find_vision_key(result, "lmhMidSaturation") = saturation;
					*find_vision_key(result, "lmhHighSaturation") = saturation;
				}
			}
		}

		void dump_net_const_strings()
		{
			namespace target = zonetool::iw7;

			static const char* level_tags[]
			{
				"mdl", "mat", "rmb", "rmg", "veh", "efx", "vfx", "loc", "shk", "tag", "hic", "sic",
				"nps", "mic", "wep", "hnt", "anm", "acl", "lui", "sut", "ges", "tgt", "vsn",
			};

			for (auto type = 0; type <= target::NETCONSTSTRINGTYPE_VISION; type++)
			{
				const auto name = std::format("ncs_{}_level", level_tags[type]);

				target::NetConstStrings asset{};
				asset.name = name.data();
				asset.stringType = static_cast<target::NetConstStringType>(type);
				asset.sourceType = target::NETCONSTSTRINGSOURCE_MAP;
				target::net_const_strings::dump(&asset);
			}
		}

		void dump_empty_glass_world(const char* name)
		{
			namespace target = zonetool::iw7;

			target::G_GlassData data{};
			target::GlassWorld asset{};
			asset.name = name;
			asset.g_glassData = &data;
			target::glass_world::dump(&asset);
		}

		void dump_empty_fx_world(const char* name)
		{
			zonetool::iw7::FxWorld asset{};
			asset.name = name;
			zonetool::iw7::fx_world::dump(&asset);
		}

		void dump_script(const std::string& name, const std::string& source)
		{
			static auto context = []
			{
				auto result = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
				result->init(xsk::gsc::build::prod, {});
				return result;
			}();

			std::vector<std::uint8_t> data(source.begin(), source.end());
			const auto assembly = context->compiler().compile(name + ".gsc", data);
			const auto [bytecode, stack, devmap] = context->assembler().assemble(*assembly);
			const auto buffer = utils::compression::zlib::compress(std::string(reinterpret_cast<const char*>(stack.data), stack.size));

			zonetool::iw7::ScriptFile script{};
			script.name = name.data();
			script.compressedLen = static_cast<int>(buffer.size());
			script.len = static_cast<int>(stack.size);
			script.bytecodeLen = static_cast<int>(bytecode.size);
			script.buffer = buffer.data();
			script.bytecode = reinterpret_cast<char*>(const_cast<std::uint8_t*>(bytecode.data));
			zonetool::iw7::scriptfile::dump(&script);
		}

		std::string port_fx(const std::string& fx_source, const std::string& createfx_source)
		{
			std::string body;
			std::smatch match;

			const auto fx_text = strip_script_comments(fx_source);
			const std::regex load_fx(R"(level\._effect\s*\[\s*"([^"]+)\"\s*\]\s*=\s*loadfx\s*\(\s*"([^"]+)\"\s*\)\s*;)", std::regex_constants::icase);
			for (auto begin = fx_text.cbegin(); std::regex_search(begin, fx_text.cend(), match, load_fx); begin = match[0].second)
			{
				body += std::format("\n    level._effect[\"{}\"] = loadfx( \"{}\" );", match[1].str(), particle_system::asset_name(match[2].str()));
			}

			if (body.empty())
			{
				return body;
			}

			const auto createfx_text = strip_script_comments(createfx_source);
			const std::regex statement(R"(ent\s*=\s*(?:[\w\\]+::)?create(\w+)\s*\(\s*("[^"]+\")?\s*\)\s*;|ent\.v\s*\[\s*("[^"]+\")\s*\]\s*=\s*([^;]+);)", std::regex_constants::icase);
			auto is_effect = false;
			for (auto begin = createfx_text.cbegin(); std::regex_search(begin, createfx_text.cend(), match, statement); begin = match[0].second)
			{
				if (match[1].matched)
				{
					const auto kind = utils::string::to_lower(match[1].str());
					is_effect = match[2].matched && (kind == "oneshoteffect" || kind == "loopeffect");
					if (is_effect)
					{
						body += std::format("\n    ent = scripts\\engine\\utility::create{}( {} );", kind, match[2].str());
					}
				}
				else if (is_effect)
				{
					body += std::format("\n    ent.v[{}] = {};", match[3].str(), match[4].str());
				}
			}

			return body;
		}

		void dump_vision(const std::string& name, const std::string& source_text)
		{
			auto source = parse_vision(source_text);

			vision_values result;
			for (const auto& entry : vision_schema)
			{
				result.emplace_back(entry.key, entry.value);
			}

			map_film(source, result);
			for (const auto& [key, value] : source)
			{
				if (auto* target = find_vision_key(result, key.data()))
				{
					*target = value;
				}
			}

			std::string text;
			for (const auto& [key, value] : result)
			{
				text += key + " \"" + value + "\"\r\n";
			}

			filesystem::file file(name);
			file.open("wb");
			file.write(text);
			file.close();
		}
	}
}
