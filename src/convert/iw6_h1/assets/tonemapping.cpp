#include <std_include.hpp>
#include "../include.hpp"
#include "tonemapping.hpp"
#include "rawfile.hpp"

#include <h1/assets/stringtable.hpp>


namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace tonemapping
		{
			namespace
			{
				std::string convert_name(ToneMapping* asset)
				{
					std::string name = asset->name;
					const auto idx = name.find("_tone");
					const auto base_name = name.substr(0, idx);
					return base_name;
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

				color_t grade_color(const ColorizationSet& set, const color_t& color)
				{
					const auto luma = color[0] * 0.299f + color[1] * 0.587f + color[2] * 0.114f;

					const auto apply = [&](const GfxColorization& colorization)
					{
						color_t result{};
						for (auto i = 0u; i < result.size(); i++)
						{
							const auto value = color[i] + colorization.powerAndDesaturation[3] * (luma - color[i]);
							result[i] = (value > 0.0f ? std::pow(value, colorization.powerAndDesaturation[i]) : 0.0f) * colorization.scale[i] + colorization.bias[i];
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

				color_t iw6_tone(const ToneMapping& tone, const ColorizationSet* set, const color_t& x)
				{
					color_t value{};
					for (auto i = 0u; i < value.size(); i++)
					{
						const auto curve = [&](const float v)
						{
							const auto a = tone.shoulderStrength[i];
							const auto b = tone.linearStrength[i];
							const auto c = tone.linearAngle[i];
							const auto d = tone.toeStrength[i];
							const auto e = tone.toeAngleNumerator[i];
							const auto f = tone.toeAngleDenominator[i];
							return (v * (a * v + c * b) + d * e) / (v * (a * v + b) + d * f) - e / f;
						};

						value[i] = curve(x[i]) / curve(tone.whitePoint[i]);
					}

					if (set)
					{
						value = grade_color(*set, value);
					}

					for (auto& channel : value)
					{
						channel = linear_to_srgb(std::clamp(channel, 0.0f, 1.0f));
					}
					return value;
				}

				enum fit_parameter
				{
					FIT_EXPOSURE,
					FIT_BLACK,
					FIT_WHITE,
					FIT_CROSSOVER,
					FIT_SHOULDER,
					FIT_TOE,
					FIT_DARK_TINT,
					FIT_MEDIUM_TINT = FIT_DARK_TINT + 3,
					FIT_LIGHT_TINT = FIT_MEDIUM_TINT + 3,
					FIT_DESATURATION = FIT_LIGHT_TINT + 3,
					FIT_COUNT,
				};

				using filmic_fit = std::array<float, FIT_COUNT>;

				color_t h1_tone(const filmic_fit& fit, const color_t& scene)
				{
					const auto s = std::pow(fit[FIT_SHOULDER], 0.25f);
					const auto t = fit[FIT_TOE] * fit[FIT_TOE];
					const auto b = fit[FIT_BLACK];
					const auto c = fit[FIT_CROSSOVER];
					const auto k = (1.0f - t) * (c - b) / ((1.0f - s) * (fit[FIT_WHITE] - c) + (1.0f - t) * (c - b));

					color_t value{};
					for (auto i = 0u; i < value.size(); i++)
					{
						const auto x = scene[i] * fit[FIT_EXPOSURE];
						const auto y = x < c ? k * (1.0f - t) * (x - b) / (c - (1.0f - t) * b - t * x)
							: (1.0f - k) + k * (x - c) / (s * x + (1.0f - s) * fit[FIT_WHITE] - c);
						value[i] = linear_to_srgb(std::clamp(y, 0.0f, 1.0f));
					}

					const auto luma = std::clamp(value[0] * 0.299f + value[1] * 0.587f + value[2] * 0.114f, 0.0f, 1.0f);
					for (auto i = 0u; i < value.size(); i++)
					{
						const auto dark = fit[FIT_DARK_TINT + i];
						const auto medium = fit[FIT_MEDIUM_TINT + i];
						const auto light = fit[FIT_LIGHT_TINT + i];
						const auto tint = dark + (2.0f * medium - 2.0f * dark) * luma + (light + dark - 2.0f * medium) * luma * luma;
						value[i] = std::clamp((value[i] + (luma - value[i]) * fit[FIT_DESATURATION]) * tint, 0.0f, 1.0f);
					}
					return value;
				}

				struct fit_sample
				{
					color_t scene;
					color_t target;
					float weight;
				};

				std::vector<fit_sample> get_fit_samples(const ToneMapping& tone, const ColorizationSet* set)
				{
					static constexpr std::array<std::pair<color_t, float>, 12> hues
					{{
						{{1.0f, 1.0f, 1.0f}, 4.0f}, {{1.0f, 0.85f, 0.7f}, 2.0f}, {{0.7f, 0.85f, 1.0f}, 2.0f}, {{0.8f, 1.0f, 0.8f}, 2.0f},
						{{1.0f, 0.75f, 0.75f}, 2.0f}, {{1.0f, 0.6f, 0.3f}, 1.0f}, {{0.3f, 0.6f, 1.0f}, 1.0f}, {{0.4f, 1.0f, 0.4f}, 1.0f},
						{{1.0f, 0.3f, 0.3f}, 1.0f}, {{0.15f, 0.35f, 1.0f}, 1.0f}, {{0.9f, 0.9f, 0.6f}, 1.0f}, {{0.7f, 0.7f, 0.7f}, 4.0f},
					}};

					std::vector<fit_sample> samples;
					samples.reserve(hues.size() * 40u);
					for (const auto& [hue, weight] : hues)
					{
						for (auto i = 0u; i < 40u; i++)
						{
							const auto scale = 0.001f * std::pow(10000.0f, static_cast<float>(i) / (40u - 1));
							const color_t scene{hue[0] * scale, hue[1] * scale, hue[2] * scale};
							samples.push_back({scene, iw6_tone(tone, set, scene), weight});
						}
					}
					return samples;
				}

				float fit_error(const std::vector<fit_sample>& samples, const filmic_fit& fit)
				{
					auto sum = 0.0f;
					for (const auto& sample : samples)
					{
						const auto value = h1_tone(fit, sample.scene);
						sum += sample.weight * (std::abs(value[0] - sample.target[0]) + std::abs(value[1] - sample.target[1]) + std::abs(value[2] - sample.target[2]));
					}
					return sum;
				}

				filmic_fit search_fit(const std::vector<fit_sample>& samples, filmic_fit best, const int first_parameter)
				{
					const auto get_range = [](const int parameter) -> range
					{
						switch (parameter)
						{
						case FIT_EXPOSURE: return {0.01f, 64.0f, 0.5f};
						case FIT_BLACK: return {0.0f, 0.2f, 0.01f};
						case FIT_WHITE: return {3.0f, 512.0f, 4.0f};
						case FIT_CROSSOVER: return {0.3f, 8.0f, 0.25f};
						case FIT_SHOULDER: return {0.5f, 0.999f, 0.02f};
						case FIT_TOE: return {0.0f, 0.98f, 0.05f};
						case FIT_DESATURATION: return {0.0f, 0.6f, 0.02f};
						default: return {0.2f, 2.0f, 0.05f};
						}
					};

					auto best_error = fit_error(samples, best);
					for (auto scale = 1.0f; scale > 0.02f; scale *= 0.7f)
					{
						for (auto round = 0; round < 4; round++)
						{
							for (auto parameter = first_parameter; parameter < FIT_COUNT; parameter++)
							{
								const auto limits = get_range(parameter);
								for (const auto direction : {1.0f, -1.0f})
								{
									auto fit = best;
									fit[parameter] = std::clamp(fit[parameter] + direction * limits.step * scale, limits.min, limits.max);
									const auto error = fit_error(samples, fit);
									if (error < best_error)
									{
										best = fit;
										best_error = error;
									}
								}
							}
						}
					}
					return best;
				}

				const ColorizationSet* get_tone_colorization_set(const ToneMapping* tone)
				{
					std::string set_name = tone->name;
					set_name = set_name.substr(0, set_name.find("_tone")) + "_colorset";
					return db_find_x_asset_header_safe(ASSET_TYPE_COLORIZATIONSET, set_name).colorizationSet;
				}

				const filmic_fit& get_tone_fit(ToneMapping* tone)
				{
					static std::unordered_map<ToneMapping*, filmic_fit> fits;
					if (const auto entry = fits.find(tone); entry != fits.end())
					{
						return entry->second;
					}

					const auto samples = get_fit_samples(*tone, get_tone_colorization_set(tone));

					filmic_fit best{};
					auto best_error = std::numeric_limits<float>::max();
					for (const auto& [exposure, white] : std::array<std::pair<float, float>, 5>{{{2.0f, 16.0f}, {2.0f, 64.0f}, {4.0f, 64.0f}, {6.0f, 16.0f}, {6.0f, 64.0f}}})
					{
						filmic_fit start{};
						start.fill(1.0f);
						start[FIT_EXPOSURE] = exposure;
						start[FIT_BLACK] = 0.0f;
						start[FIT_WHITE] = white;
						start[FIT_CROSSOVER] = 2.0f;
						start[FIT_SHOULDER] = 0.9f;
						start[FIT_TOE] = 0.3f;
						start[FIT_DESATURATION] = 0.0f;

						const auto fit = search_fit(samples, start, FIT_EXPOSURE);
						const auto error = fit_error(samples, fit);
						if (error < best_error)
						{
							best = fit;
							best_error = error;
						}
					}

					return fits.emplace(tone, best).first->second;
				}

				float user_exposure(ToneMapping* asset, const float exposure)
				{
					return 2.84f + std::log2(1000.0f / (get_tone_fit(asset)[FIT_EXPOSURE] * exposure));
				}

				float iris_key(ToneMapping* asset) { return std::max(asset->exposure[2] * 0.77f, 0.001f); }
				float iris_min(ToneMapping* asset) { return std::max(asset->exposure[0], 0.001f); }
				float iris_max(ToneMapping* asset) { return std::max(asset->exposure[1], iris_min(asset)); }

				float iris_ev(ToneMapping* asset) { return 2.84f + std::log2(1000.0f * iris_key(asset) / iris_min(asset)); }
				float iris_adjust(ToneMapping* asset) { return std::log2(get_tone_fit(asset)[FIT_EXPOSURE] * iris_key(asset)); }

				constexpr auto iris_mid_range = 1.6f;
				constexpr auto iris_light_range = 4.0f;

				std::string convert_exposure(ToneMapping* asset) { return std::to_string(user_exposure(asset, iris_min(asset))); }
				std::string convert_max_exposure(ToneMapping* asset) { return std::to_string(user_exposure(asset, iris_max(asset))); }
				std::string convert_dark_ev(ToneMapping* asset) { return std::to_string(iris_ev(asset)); }
				std::string convert_mid_ev(ToneMapping* asset) { return std::to_string(iris_ev(asset) + iris_mid_range); }
				std::string convert_light_ev(ToneMapping* asset) { return std::to_string(iris_ev(asset) + iris_light_range); }
				std::string convert_dark_adjust(ToneMapping* asset) { return std::to_string(iris_adjust(asset)); }
				std::string convert_mid_adjust(ToneMapping* asset) { return std::to_string(iris_adjust(asset) + iris_mid_range); }
				std::string convert_light_adjust(ToneMapping* asset) { return std::to_string(iris_adjust(asset) + iris_light_range); }

				std::string convert_black(ToneMapping* asset) { return std::to_string(get_tone_fit(asset)[FIT_BLACK]); }
				std::string convert_white(ToneMapping* asset) { return std::to_string(get_tone_fit(asset)[FIT_WHITE]); }
				std::string convert_crossover(ToneMapping* asset) { return std::to_string(get_tone_fit(asset)[FIT_CROSSOVER]); }
				std::string convert_shoulder(ToneMapping* asset) { return std::to_string(get_tone_fit(asset)[FIT_SHOULDER]); }
				std::string convert_toe(ToneMapping* asset) { return std::to_string(get_tone_fit(asset)[FIT_TOE]); }

				std::vector<lightset_entry> lightset_values =
				{
					{"sunenable", "1"},
					{"sunshadowscale", "1"},
					{"sunshadowscale_time", "0"},
					{"spotlimit", "4"},
					{"qualityspotshadow", "1"},
					{"usedsuncascadecount", "3"},
					{"sunsamplesizenear", "0.25"},
					{"sunsamplesizenear_time", "0"},
					{"sunfilterradius", "0.2"},
					{"sunfilterradius_time", "0"},
					{"spotfilterradius", "1.5"},
					{"spotfilterradius_time", "0"},

					{"specularcolorscale", "2"},
					{"specularcolorscale_time", "0"},
					{"diffusecolorscale", "1"},
					{"diffusecolorscale_time", "0"},

					{"veil", "0"},
					{"veilstrength", "0"},
					{"veilstrength_time", "0"},
					{"veilbackgroundstrength", "0"},
					{"veilbackgroundstrength_time", "0"},

					{"tonemap", "2"},
					{"tonemapauto", "1"},
					{"tonemapblend", "1"},
					{"tonemaplockautoexposureadjust", "0"},
					{"tonemapautoexposureadjust", "0"},
					{"tonemapautoexposureadjust_time", "0"},
					{"tonemapexposure", "-10", convert_exposure},
					{"tonemapexposure_time", "0"},
					{"tonemapexposureadjust", "0"},
					{"tonemapexposureadjust_time", "0"},
					{"tonemapmaxexposure", "-10", convert_max_exposure},
					{"tonemapmaxexposure_time", "0.5"},
					{"tonemapadaptspeed", "0.02"},
					{"tonemapadaptspeed_time", "0"},
					{"tonemapdarkev", "2.84", convert_dark_ev},
					{"tonemapdarkev_time", "0.5"},
					{"tonemapmidev", "7.823", convert_mid_ev},
					{"tonemapmidev_time", "0.5"},
					{"tonemaplightev", "12.81", convert_light_ev},
					{"tonemaplightev_time", "0.5"},
					{"tonemapdarkexposureadjust", "-3.17", convert_dark_adjust},
					{"tonemapdarkexposureadjust_time", "0.5"},
					{"tonemapmidexposureadjust", "-0.0651", convert_mid_adjust},
					{"tonemapmidexposureadjust_time", "0.5"},
					{"tonemaplightexposureadjust", "1.47", convert_light_adjust},
					{"tonemaplightexposureadjust_time", "0.5"},
					{"tonemapminexposureadjust", "-3.17", convert_dark_adjust},
					{"tonemapminexposureadjust_time", "0.5"},
					{"tonemapmaxexposureadjust", "2.3", convert_light_adjust},
					{"tonemapmaxexposureadjust_time", "0.5"},
					{"tonemapwhite", "512", convert_white},
					{"tonemapwhite_time", "0.5"},
					{"tonemapshoulder", "0.96", convert_shoulder},
					{"tonemapshoulder_time", "0.5"},
					{"tonemapcrossover", "2.6", convert_crossover},
					{"tonemapcrossover_time", "0.5"},
					{"tonemaptoe", "0.38", convert_toe},
					{"tonemaptoe_time", "0.5"},
					{"tonemapblack", "0.016", convert_black},
					{"tonemapblack_time", "0.5"},

					{"aodiminish", "1"},
					{"aodiminish_time", "0"},
					{"ssaostrength", "0.45"},
					{"ssaostrength_time", "0"},
					{"ssaopower", "12"},
					{"ssaopower_time", "0"},
					{"ssaominstrengthdepth", "25"},
					{"ssaominstrengthdepth_time", "0"},
					{"ssaomaxstrengthdepth", "40"},
					{"ssaomaxstrengthdepth_time", "0"},
					{"hemiaostrength", "0.5"},
					{"hemiaostrength_time", "0"},
					{"hemiaopower", "1"},
					{"hemiaopower_time", "0"},
					{"hemiaorejectionfalloff", "0.5"},
					{"hemiaorejectionfalloff_time", "0"},

					{"mdao", "1"},
					{"mdaooccluderculldistance", "1000"},
					{"mdaooccluderculldistance_time", "0"},
					{"mdaooccluderfadeoutstartdistance", "900"},
					{"mdaooccluderfadeoutstartdistance_time", "0"},
					{"mdaocapsulestrength", "1"},
					{"mdaocapsulestrength_time", "0"},
					{"mdaovolumestrength", "1"},
					{"mdaovolumestrength_time", "0"},
					{"ssaowidth", "36"},
					{"ssaowidth_time", "0"},
					{"ssaogapfalloff", "0.25"},
					{"ssaogapfalloff_time", "0"},
					{"ssaogradientfalloff", "0.05"},
					{"ssaogradientfalloff_time", "0"},
					{"tessellationfactor", "60"},
					{"tessellationfactor_time", "0"},
					{"tessellationcutoffdistance", "960"},
					{"tessellationcutoffdistance_time", "0"},
					{"tessellationcutofffalloff", "320"},
					{"tessellationcutofffalloff_time", "0"},

					{"ssaomode", "1"},
					{"ssaofadedepth", "2500"},
					{"ssaofadedepth_time", "0"},
					{"ssaorejectdepth", "2500"},
					{"ssaorejectdepth_time", "0"},
				};

				ToneMapping* get_vision_tone_mapping(const std::string& vision)
				{
					const auto values = rawfile::read_vision(vision);
					const auto tone_name = values.find("toneMapping");
					if (tone_name == values.end())
					{
						return nullptr;
					}

					return db_find_x_asset_header_safe(ASSET_TYPE_TONEMAPPING, tone_name->second).toneMapping;
				}

				std::map<std::string, ToneMapping*> get_map_lightsets(ToneMapping* asset)
				{
					const auto& fastfile = filesystem::get_fastfile();

					std::map<std::string, ToneMapping*> lightsets;
					std::set<std::string> vision_tone_names;
					db_enum_assets(ASSET_TYPE_RAWFILE, [&](void* header)
					{
						const std::string name = static_cast<RawFile*>(header)->name;
						if (!name.starts_with("vision/") || !name.ends_with(".vision") || !name.contains(fastfile))
						{
							return;
						}

						const auto vision = name.substr(7, name.size() - 14);
						if (const auto tone_mapping = get_vision_tone_mapping(vision))
						{
							lightsets[vision] = tone_mapping;
							vision_tone_names.insert(tone_mapping->name);
						}
					});

					std::set<std::string> names{asset->name};
					db_enum_assets(ASSET_TYPE_TONEMAPPING, [&](void* header)
					{
						const auto tone_mapping = static_cast<ToneMapping*>(header);
						if (std::string_view(tone_mapping->name).contains(fastfile))
						{
							names.insert(tone_mapping->name);
						}
					});

					for (const auto& name : names)
					{
						const auto tone_mapping = db_find_x_asset_header_safe(ASSET_TYPE_TONEMAPPING, name).toneMapping;
						if (!vision_tone_names.contains(name))
						{
							const auto lightset = tone_mapping ? tone_mapping : asset;
							lightsets.emplace(convert_name(lightset), lightset);
						}
					}

					return lightsets;
				}
			}

			film_values get_film_values(const std::string& vision, const ColorizationSet* set)
			{
				film_values values{};
				values.dark.fill(1.0f);
				values.medium.fill(1.0f);
				values.light.fill(1.0f);

				auto* tone = get_vision_tone_mapping(vision);
				if (!tone)
				{
					tone = db_find_x_asset_header_safe(ASSET_TYPE_TONEMAPPING, vision + "_tone").toneMapping;
				}
				if (!tone)
				{
					tone = db_find_x_asset_header_safe(ASSET_TYPE_TONEMAPPING, filesystem::get_fastfile() + "_tone").toneMapping;
				}

				if (!tone || !set)
				{
					return values;
				}

				auto fit = get_tone_fit(tone);
				if (set != get_tone_colorization_set(tone))
				{
					fit[FIT_DESATURATION] = 0.0f;
					fit = search_fit(get_fit_samples(*tone, set), fit, FIT_DARK_TINT);
				}

				for (auto i = 0u; i < 3u; i++)
				{
					values.dark[i] = fit[FIT_DARK_TINT + i];
					values.medium[i] = fit[FIT_MEDIUM_TINT + i];
					values.light[i] = fit[FIT_LIGHT_TINT + i];
				}
				values.desaturation = fit[FIT_DESATURATION];
				return values;
			}

			zonetool::h1::StringTable* convert(ToneMapping* asset, utils::memory::allocator& allocator)
			{
				const auto& fastfile = filesystem::get_fastfile();
				const auto new_asset = allocator.allocate<zonetool::h1::StringTable>();
				new_asset->name = allocator.duplicate_string(utils::string::va("maps/createart/%s_lightsets_hdr.csv", fastfile.data()));

				const auto lightsets = get_map_lightsets(asset);

				new_asset->columnCount = 1 + static_cast<int>(lightsets.size());
				new_asset->rowCount = 1 + static_cast<int>(lightset_values.size());
				new_asset->values = allocator.allocate_array<zonetool::h1::StringTableCell>(new_asset->columnCount * new_asset->rowCount);

				const auto insert_value = [&](const int column, const int row, const std::string& value)
				{
					const auto index = (row * new_asset->columnCount) + column;
					new_asset->values[index].string = allocator.duplicate_string(value.data());
					new_asset->values[index].hash = string_table_hash(new_asset->values[index].string);
				};

				insert_value(0, 0, "name");
				for (auto i = 0u; i < lightset_values.size(); i++)
				{
					insert_value(0, i + 1, lightset_values[i].name);
				}

				auto i = 0u;
				for (const auto& [lightset_name, tone_mapping] : lightsets)
				{
					insert_value(i + 1, 0, lightset_name);
					for (auto o = 0u; o < lightset_values.size(); o++)
					{
						const auto& entry = lightset_values[o];
						if (entry.convert.has_value())
						{
							insert_value(i + 1, o + 1, entry.convert->operator()(tone_mapping));
						}
						else
						{
							insert_value(i + 1, o + 1, entry.default_value);
						}
					}

					++i;
				}

				return new_asset;
			}

			void dump(ToneMapping* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::string_table::dump(converted_asset);
				add_csv_row("stringtable", converted_asset->name);
			}
		}
	}
}
