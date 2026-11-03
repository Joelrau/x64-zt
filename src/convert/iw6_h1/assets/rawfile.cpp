#include <std_include.hpp>
#include "../include.hpp"
#include "rawfile.hpp"

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

				float linear_to_srgb(const float value)
				{
					return value <= 0.0031308f ? value * 12.92f : 1.055f * std::pow(value, 1.0f / 2.4f) - 0.055f;
				}

				float smooth_step(const float value)
				{
					return value * value * (3.0f - 2.0f * value);
				}

				color_t apply_colorization(const GfxColorization& colorization, const float value)
				{
					color_t result{};
					for (auto i = 0u; i < result.size(); i++)
					{
						result[i] = std::pow(std::max(value, 0.000001f), colorization.powerAndDesaturation[i]) * colorization.scale[i] + colorization.bias[i];
					}

					return result;
				}

				// the color of the grade in the gamma space of the h1 film tints, the brightness stays with the tonemap
				color_t get_grade_tint(const ColorizationSet& set, const float value)
				{
					auto color = rawfile::apply_colorization_set(set, value);
					for (auto& channel : color)
					{
						channel = linear_to_srgb(std::clamp(channel, 0.0f, 1.0f));
					}

					const auto luma = color[0] * 0.299f + color[1] * 0.587f + color[2] * 0.114f;
					return {color[0] / luma, color[1] / luma, color[2] / luma};
				}

				std::string tint_to_string(const color_t& tint)
				{
					return utils::string::va("%g %g %g", tint[0], tint[1], tint[2]);
				}

				// the grade of postfx_color_grad(_lmh) for a color
				color_t grade_color(const ColorizationSet& set, const color_t& color)
				{
					const auto luma = color[0] * 0.299f + color[1] * 0.587f + color[2] * 0.114f;

					const auto apply = [&](const GfxColorization& colorization)
					{
						color_t result{};
						for (auto i = 0u; i < result.size(); i++)
						{
							const auto value = color[i] + colorization.powerAndDesaturation[3] * (luma - color[i]);
							result[i] = std::pow(std::max(value, 0.000001f), colorization.powerAndDesaturation[i]) * colorization.scale[i] + colorization.bias[i];
						}
						return result;
					};

					if ((set.flags & COLORIZATION_USE_LOW_MID_HIGH) == 0 || !set.colorizationLow || !set.colorizationMid || !set.colorizationHigh)
					{
						return set.colorization ? apply(*set.colorization) : color;
					}

					const auto low = 1.0f - smooth_step(std::clamp((luma - set.lowAndHighEndPoints[0]) * set.lowAndHighEndPoints[1], 0.0f, 1.0f));
					const auto mid = 1.0001f - smooth_step(std::clamp((std::abs(luma - set.midEndPoints[0]) - set.midEndPoints[1]) * set.midEndPoints[2], 0.0f, 1.0f));
					const auto high = smooth_step(std::clamp((luma - set.lowAndHighEndPoints[2]) * set.lowAndHighEndPoints[3], 0.0f, 1.0f));

					const auto low_color = apply(*set.colorizationLow);
					const auto mid_color = apply(*set.colorizationMid);
					const auto high_color = apply(*set.colorizationHigh);

					color_t result{};
					for (auto i = 0u; i < result.size(); i++)
					{
						result[i] = (low_color[i] * low + mid_color[i] * mid + high_color[i] * high) / (low + mid + high);
					}
					return result;
				}

				// film tints only multiply, a grade bias pulls colors together: desaturate until the mean saturation matches the grade
				float get_grade_desaturation(const ColorizationSet& set, const color_t& dark, const color_t& medium, const color_t& light)
				{
					static constexpr std::array<color_t, 8> colors
					{{
						{1.0f, 1.0f, 1.0f}, {1.0f, 0.6f, 0.3f}, {0.3f, 0.6f, 1.0f}, {0.4f, 1.0f, 0.4f},
						{1.0f, 0.3f, 0.3f}, {0.6f, 0.6f, 1.0f}, {0.8f, 0.9f, 0.6f}, {0.15f, 0.35f, 1.0f},
					}};

					const auto saturation = [](const color_t& color)
					{
						return *std::max_element(color.begin(), color.end()) - *std::min_element(color.begin(), color.end());
					};

					auto film_saturation = 0.0f;
					auto grade_saturation = 0.0f;
					for (const auto& color : colors)
					{
						for (const auto scale : {0.02f, 0.05f, 0.1f, 0.2f, 0.35f, 0.5f, 0.7f, 1.0f})
						{
							color_t film{};
							color_t grade{};
							const auto graded = grade_color(set, {color[0] * scale, color[1] * scale, color[2] * scale});
							for (auto i = 0u; i < film.size(); i++)
							{
								film[i] = linear_to_srgb(color[i] * scale);
								grade[i] = linear_to_srgb(std::clamp(graded[i], 0.0f, 1.0f));
							}

							const auto luma = film[0] * 0.2126f + film[1] * 0.7152f + film[2] * 0.0722f;
							for (auto i = 0u; i < film.size(); i++)
							{
								const auto tint = dark[i] + (2.0f * medium[i] - 2.0f * dark[i]) * luma + (light[i] + dark[i] - 2.0f * medium[i]) * luma * luma;
								film[i] = std::clamp(film[i] * tint, 0.0f, 1.0f);
							}

							film_saturation += saturation(film);
							grade_saturation += saturation(grade);
						}
					}

					return film_saturation > 0.0f ? std::clamp(1.0f - grade_saturation / film_saturation, 0.0f, 0.5f) : 0.0f;
				}
			}

			// the grade of postfx_color_grad(_lmh) for a gray value
			std::array<float, 3> apply_colorization_set(const ColorizationSet& set, const float value)
			{
				if ((set.flags & COLORIZATION_USE_LOW_MID_HIGH) == 0 || !set.colorizationLow || !set.colorizationMid || !set.colorizationHigh)
				{
					return set.colorization ? apply_colorization(*set.colorization, value) : std::array<float, 3>{value, value, value};
				}

				const auto low = 1.0f - smooth_step(std::clamp((value - set.lowAndHighEndPoints[0]) * set.lowAndHighEndPoints[1], 0.0f, 1.0f));
				const auto mid = 1.0001f - smooth_step(std::clamp((std::abs(value - set.midEndPoints[0]) - set.midEndPoints[1]) * set.midEndPoints[2], 0.0f, 1.0f));
				const auto high = smooth_step(std::clamp((value - set.lowAndHighEndPoints[2]) * set.lowAndHighEndPoints[3], 0.0f, 1.0f));

				const auto low_color = apply_colorization(*set.colorizationLow, value);
				const auto mid_color = apply_colorization(*set.colorizationMid, value);
				const auto high_color = apply_colorization(*set.colorizationHigh, value);

				std::array<float, 3> result{};
				for (auto i = 0u; i < result.size(); i++)
				{
					result[i] = (low_color[i] * low + mid_color[i] * mid + high_color[i] * high) / (low + mid + high);
				}

				return result;
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

			std::array<float, 4> get_sky_intensity()
			{
				const auto* vision = db_find_x_asset_header_safe(ASSET_TYPE_RAWFILE, std::format("vision/{}.vision", filesystem::get_fastfile())).rawfile;
				if (!vision)
				{
					return {1.0f, 1.0f, 1.0f, 1.0f};
				}

				auto values = read_vision_values(read_rawfile(vision));
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
				// iw6 grades with the colorization set instead of its film values, h1 film tints are a bezier over gamma luma
				const auto colorization_name = values.find("colorizationSet");
				const auto* colorization_set = colorization_name != values.end()
					? db_find_x_asset_header_safe(ASSET_TYPE_COLORIZATIONSET, colorization_name->second).colorizationSet : nullptr;

				color_t dark_tint{1.0f, 1.0f, 1.0f};
				color_t medium_tint{1.0f, 1.0f, 1.0f};
				color_t light_tint{1.0f, 1.0f, 1.0f};
				if (colorization_set)
				{
					// a bias only shows near black, the bezier would spread it far up the shadows
					dark_tint = get_grade_tint(*colorization_set, 0.01f);
					for (auto& channel : dark_tint)
					{
						channel = std::clamp(channel, 0.75f, 1.35f);
					}

					// a clipped grade has lost its color, take the brightest value that keeps it
					auto light_value = 0.7f;
					while (light_value > 0.3f)
					{
						const auto color = apply_colorization_set(*colorization_set, light_value);
						if (*std::max_element(color.begin(), color.end()) < 1.0f)
						{
							break;
						}
						light_value -= 0.05f;
					}

					light_tint = get_grade_tint(*colorization_set, light_value);
					const auto middle = get_grade_tint(*colorization_set, 0.21f);
					for (auto i = 0u; i < medium_tint.size(); i++)
					{
						// 0.75 from the mp_strikezone h1/iw6 screenshots, the h1 curve already keeps some of the shift
						medium_tint[i] = 1.0f + 0.75f * (2.0f * middle[i] - 0.5f * (dark_tint[i] + light_tint[i]) - 1.0f);
					}
				}

				write_value("r_filmEnable", "1");
				write_value("r_filmContrast", "1", colorization_set != nullptr);
				write_value("r_filmBrightness", "0", colorization_set != nullptr);
				const auto desaturation = colorization_set ? get_grade_desaturation(*colorization_set, dark_tint, medium_tint, light_tint) : 0.0f;
				write_value("r_filmDesaturation", utils::string::va("%g", desaturation), colorization_set != nullptr);
				write_value("r_filmDesaturationDark", utils::string::va("%g", desaturation), colorization_set != nullptr);
				write_value("r_filmInvert", "0");
				write_value("r_filmLightTint", tint_to_string(light_tint), true);
				write_value("r_filmMediumTint", tint_to_string(medium_tint), true);
				write_value("r_filmDarkTint", tint_to_string(dark_tint), true);
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

				struct fog_set
				{
					float color_intensity = 1.0f;
					float sun_intensity = 1.0f;
					std::string hdr_override;
				};

				static const std::regex name_regex("create_vision_set_fog\\(\\s*\"([^\"]+)\"");
				static const std::regex color_regex("ent\\.HDRColorIntensity\\s*=\\s*([0-9.]+)");
				static const std::regex sun_regex("ent\\.HDRSunColorIntensity\\s*=\\s*([0-9.]+)");
				static const std::regex override_regex("ent\\.HDROverride\\s*=\\s*\"([^\"]+)\"");

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
				}

				// the intensity multiplies the fog color in iw6, in h1 it is an ev: fb = nits * 0.001, nits = 2^(ev - 2.84)
				const auto intensity_to_ev = [](const float intensity)
				{
					return 2.84f + std::log2(1000.0f * std::max(intensity, 0.001f));
				};

				// iw6 draws the override of a fog set
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

					// iw6 fog colors are linear, explicit h1 ones are read as srgb
					static const std::regex fog_color_regex("(ent\\.(?:red|green|blue|sunRed|sunGreen|sunBlue)\\s*=\\s*)([0-9.]+)");
					if (std::regex_search(line, match, fog_color_regex))
					{
						buffer.append(match.prefix().str() + match[1].str());
						buffer.append(utils::string::va("%g", linear_to_srgb(static_cast<float>(std::atof(match[2].str().data())))));
						buffer.append(match.suffix().str());
					}
					else
					{
						buffer.append(line);
					}
					buffer.append("\n");

					// without them h1 reads the fog as legacy
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
				add_csv_row("rawfile", hdr_name); // H1 reads the fog of the map from this file

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
