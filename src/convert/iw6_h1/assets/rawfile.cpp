#include <std_include.hpp>
#include "../include.hpp"
#include "rawfile.hpp"
#include "tonemapping.hpp"

#include <h1/assets/rawfile.hpp>


namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace rawfile
		{
			namespace
			{
				std::string read_rawfile(const RawFile* asset)
				{
					if (asset->compressedLen > 0)
					{
						std::string uncompressed;
						uncompressed.resize(asset->len);

						auto size = static_cast<uLongf>(uncompressed.size());
						if (uncompress(reinterpret_cast<Bytef*>(uncompressed.data()), &size,
							reinterpret_cast<const Bytef*>(asset->buffer), asset->compressedLen) != Z_OK)
						{
							ZONETOOL_WARNING("rawfile \"%s\" could not be decompressed", asset->name);
							return "";
						}

						uncompressed.resize(size);
						return uncompressed;
					}
					else if (asset->len > 0)
					{
						return std::string(asset->buffer, static_cast<std::size_t>(asset->len));
					}

					return "";
				}

				using color_t = std::array<float, 3>;

				std::string tint_to_string(const color_t& tint)
				{
					return utils::string::va("%g %g %g", tint[0], tint[1], tint[2]);
				}
			}

			std::vector<float> parse_vector(const std::string& value)
			{
				std::vector<float> values;

				const auto tokens = utils::string::split(value, ' ');
				for (const auto& token : tokens)
				{
					values.emplace_back(std::atof(token.data()));
				}

				return values;
			}

			std::unordered_map<std::string, std::string> read_vision_values(const std::string& data)
			{
				static const std::regex regex("(.+?)\\s+\"(.+)\"");

				std::unordered_map<std::string, std::string> values;
				for (const auto& line : utils::string::split(data, '\n'))
				{
					std::smatch match;
					if (std::regex_search(line, match, regex))
					{
						values.insert(std::make_pair(match[1], match[2]));
					}
				}

				return values;
			}

			std::unordered_map<std::string, std::string> read_vision(const std::string& name)
			{
				const auto* vision = db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, std::format("vision/{}.vision", name)).rawfile;
				if (!vision)
				{
					return {};
				}

				return read_vision_values(read_rawfile(vision));
			}

			std::array<float, 4> get_sky_intensity()
			{
				auto values = read_vision(filesystem::get_fastfile());
				const auto intensity = parse_vector(values["r_hdrSkyIntensity"]);
				const auto tint = parse_vector(values["r_hdrSkyColorTint"]);
				if (intensity.size() != 1 || tint.size() != 3)
				{
					return {1.0f, 1.0f, 1.0f, 1.0f};
				}

				const auto value = intensity[0];
				return {value * tint[0], value * tint[1], value * tint[2], value};
			}

			std::string convert_vision_file(const std::string& name, const std::string& data, utils::memory::allocator&)
			{
				auto values = read_vision_values(data);

				std::string buffer;

				const auto write_value = [&](const std::string& name, const std::string& default_value, bool override_ = false)
				{
					const auto new_gen = name + "_NG";
					auto iter = values.find(new_gen);

					if (iter == values.end())
					{
						iter = values.find(name);
					}

					if (override_ || iter == values.end())
					{
						buffer.append(utils::string::va("%s \"%s\"", name.data(), default_value.data()));
						if (iter != values.end())
						{
							buffer.append(utils::string::va(" // \"%s\"\r\n", iter->second.data()));
						}
						else
						{
							buffer.append("\r\n");
						}
					}
					else
					{
						buffer.append(utils::string::va("%s \"%s\"\r\n", name.data(), iter->second.data()));
					}
				};

				const auto separator = [&]()
				{
					buffer.append("\r\n");
				};

				ZONETOOL_INFO("Converting vision file %s...", name.data());

				write_value("r_glow", "1");
				write_value("r_glowUseAltCutoff", "0");
				write_value("r_glowRadius0", "0");
				write_value("r_glowBloomPinch", "0");
				write_value("r_glowBloomCutoff", "0");
				write_value("r_glowBloomDesaturation", "0");
				write_value("r_glowBloomIntensity0", "0");
				write_value("r_glowAltCutoff", "0");
				write_value("r_glowAltColorScaleR", "0");
				write_value("r_glowAltColorScaleG", "0");
				write_value("r_glowAltColorScaleB", "0");
				separator();
				const auto colorization_name = values.find("colorizationSet");
				const auto* colorization_set = colorization_name != values.end()
					? db_find_x_asset_header_safe(ASSET_TYPE_COLORIZATIONSET, colorization_name->second).colorizationSet : nullptr;

				const auto film = tonemapping::get_film_values(name, colorization_set);

				write_value("r_filmEnable", "1");
				write_value("r_filmContrast", "1", colorization_set != nullptr);
				write_value("r_filmBrightness", "0", colorization_set != nullptr);
				write_value("r_filmDesaturation", utils::string::va("%g", film.desaturation), colorization_set != nullptr);
				write_value("r_filmDesaturationDark", utils::string::va("%g", film.desaturation), colorization_set != nullptr);
				write_value("r_filmInvert", "0");
				write_value("r_filmLightTint", tint_to_string(film.light), true);
				write_value("r_filmMediumTint", tint_to_string(film.medium), true);
				write_value("r_filmDarkTint", tint_to_string(film.dark), true);
				separator();
				write_value("r_primaryLightUseTweaks", "0");
				write_value("r_primaryLightTweakDiffuseStrength", "0");
				write_value("r_primaryLightTweakSpecularStrength", "0");
				write_value("r_charLightAmbient", "0");
				separator();
				write_value("r_viewModelPrimaryLightUseTweaks", "0");
				write_value("r_viewModelPrimaryLightTweakDiffuseStrength", "0");
				write_value("r_viewModelPrimaryLightTweakSpecularStrength", "0");
				write_value("r_viewModelLightAmbient", "0");
				separator();
				write_value("r_volumeLightScatter", "0");
				write_value("r_volumeLightScatterColor", "0 0 0");
				write_value("r_volumeLightScatterLinearAtten", "0");
				write_value("r_volumeLightScatterQuadraticAtten", "0");
				write_value("r_volumeLightScatterAngularAtten", "0");
				write_value("r_volumeLightScatterDepthAttenNear", "0");
				write_value("r_volumeLightScatterDepthAttenFar", "0");
				write_value("r_volumeLightScatterBackgroundDistance", "0");
				write_value("r_volumeLightScatterEv", "0");
				separator();
				write_value("r_rimLightUseTweaks", "1");
				write_value("r_rimLight0Pitch", "0");
				write_value("r_rimLight0Heading", "0");
				write_value("r_rimLightDiffuseIntensity", "1", true);
				write_value("r_rimLightSpecIntensity", "1", true);
				write_value("r_rimLightBias", "0");
				write_value("r_rimLightPower", "1", true);
				write_value("r_rimLight0Color", "0 0 0", true);
				write_value("r_rimLightFalloffMaxDistance", "700");
				write_value("r_rimLightFalloffMinDistance", "100");
				write_value("r_rimLightFalloffMinIntensity", "0.25");
				separator();
				write_value("r_unlitSurfaceHDRScalar", "1");
				write_value("r_litSurfaceHDRScalar", "1");
				write_value("r_hdrSkyIntensity", "1");
				write_value("r_hdrSkyColorTint", "1 1 1");
				separator();
				write_value("r_chromaticAberrationMode", "");
				write_value("r_chromaticSeparation", " ");
				write_value("r_chromaticAberrationAlpha", "");

				return buffer;
			}

			std::string convert_fog_file(const std::string& name, const std::string& data, utils::memory::allocator& allocator)
			{
				const auto lines = utils::string::split(data, '\n');

				static const std::regex name_regex("create_vision_set_fog\\(\\s*\"([^\"]+)\"");
				static const std::regex color_regex("ent\\.HDRColorIntensity\\s*=\\s*([0-9.]+)");
				static const std::regex sun_regex("ent\\.HDRSunColorIntensity\\s*=\\s*([0-9.]+)");
				static const std::regex override_regex("ent\\.HDROverride\\s*=\\s*\"([^\"]+)\"");
				static const std::regex sky_min_regex("(ent\\.skyFogMinAngle\\s*=\\s*)(-?[0-9.]+)");
				static const std::regex sky_max_regex("(ent\\.skyFogMaxAngle\\s*=\\s*)(-?[0-9.]+)");
				static const std::regex sky_intensity_regex("(ent\\.skyFogIntensity\\s*=\\s*)(-?[0-9.]+)");

				std::unordered_map<std::string, fog_set> sets;
				std::string set_name;
				for (const auto& line : lines)
				{
					std::smatch match;
					if (std::regex_search(line, match, name_regex))
					{
						set_name = match[1].str();
					}
					else if (std::regex_search(line, match, color_regex))
					{
						sets[set_name].color_intensity = static_cast<float>(std::atof(match[1].str().data()));
					}
					else if (std::regex_search(line, match, sun_regex))
					{
						sets[set_name].sun_intensity = static_cast<float>(std::atof(match[1].str().data()));
					}
					else if (std::regex_search(line, match, override_regex))
					{
						sets[set_name].hdr_override = match[1].str();
					}
					else if (std::regex_search(line, match, sky_min_regex))
					{
						sets[set_name].sky_min_angle = static_cast<float>(std::atof(match[2].str().data()));
					}
					else if (std::regex_search(line, match, sky_max_regex))
					{
						sets[set_name].sky_max_angle = static_cast<float>(std::atof(match[2].str().data()));
					}
				}

				const auto intensity_to_ev = [](const float intensity)
				{
					return 2.84f + std::log2(1000.0f * std::max(intensity, 0.001f));
				};

				const auto get_fog_set = [&](const std::string& fog_name) -> const fog_set&
				{
					const auto& set = sets[fog_name];
					return set.hdr_override.empty() ? set : sets[set.hdr_override];
				};

				std::string buffer;

				for (const auto& line : lines)
				{
					if (line.contains("HDRColorIntensity") || line.contains("HDRSunColorIntensity"))
					{
						continue;
					}

					std::smatch match;
					if (std::regex_search(line, match, name_regex))
					{
						set_name = match[1].str();
					}

					if (const auto& set = sets[set_name]; set.sky_min_angle >= set.sky_max_angle)
					{
						const auto replace_sky_value = [&](const std::regex& regex, const float value)
						{
							if (!std::regex_search(line, match, regex))
							{
								return false;
							}

							const auto prefix = match.prefix().str() + match[1].str();
							buffer.append(prefix + utils::string::va("%g", value) + match.suffix().str() + "\n");
							return true;
						};

						if (replace_sky_value(sky_min_regex, 180.0f - set.sky_min_angle)
							|| replace_sky_value(sky_max_regex, 180.0f - set.sky_max_angle))
						{
							continue;
						}

						if (std::regex_search(line, match, sky_intensity_regex)
							&& replace_sky_value(sky_intensity_regex, static_cast<float>(std::atof(match[2].str().data())) * 0.5f))
						{
							continue;
						}
					}

					buffer.append(line);
					buffer.append("\n");

					if (line.contains("ent.blue"))
					{
						buffer.append(utils::string::va("\tent.HDRColorIntensity = %g;\n", intensity_to_ev(get_fog_set(set_name).color_intensity)));
					}
					else if (line.contains("ent.sunBlue"))
					{
						buffer.append(utils::string::va("\tent.HDRSunColorIntensity = %g;\n", intensity_to_ev(get_fog_set(set_name).sun_intensity)));
					}
				}

				const std::string hdr_name = utils::string::va("maps/createart/%s_fog_hdr.gsc", name.data());
				const auto new_asset = allocator.allocate<zonetool::h1::RawFile>();
				new_asset->name = allocator.duplicate_string(hdr_name);
				new_asset->buffer = allocator.duplicate_string(buffer);
				new_asset->len = static_cast<int>(buffer.size());
				new_asset->compressedLen = 0;
				zonetool::h1::rawfile::dump(new_asset);
				add_csv_row("rawfile", hdr_name);

				return buffer;
			}

			using convert_cb_t = std::function<std::string(const std::string&, const std::string&, utils::memory::allocator&)>;

			zonetool::h1::RawFile* convert(RawFile* asset, utils::memory::allocator& allocator)
			{
				struct entry_t
				{
					std::string expression;
					convert_cb_t cb;
				};

				std::vector<entry_t> convert_funcs =
				{
					{"vision/(.+)\\.vision", convert_vision_file},
					{"maps/createart/(.+)_fog\\.gsc", convert_fog_file},
				};

				const std::string name = asset->name;
				for (const auto& entry : convert_funcs)
				{
					std::regex regex(entry.expression);
					std::smatch match;
					if (!std::regex_search(name, match, regex))
					{
						continue;
					}

					const auto data = read_rawfile(asset);
					const auto new_data = entry.cb(match[1], data, allocator);

					const auto new_asset = allocator.allocate<zonetool::h1::RawFile>();
					new_asset->name = allocator.duplicate_string(asset->name);
					new_asset->buffer = allocator.duplicate_string(new_data);
					new_asset->len = static_cast<int>(new_data.size());
					new_asset->compressedLen = 0;
					return new_asset;
				}

				return reinterpret_cast<zonetool::h1::RawFile*>(asset);
			}

			void dump(RawFile* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::rawfile::dump(converted_asset);
			}
		}
	}
}
