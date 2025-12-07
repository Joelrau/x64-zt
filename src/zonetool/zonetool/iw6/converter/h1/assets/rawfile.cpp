#include <std_include.hpp>
#include "zonetool/iw6/converter/h1/include.hpp"
#include "rawfile.hpp"
#include "techset.hpp"

#include "zonetool/h1/assets/rawfile.hpp"
#include "zonetool/h1/assets/techset.hpp"

#include "zonetool/iw6/zonetool.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace rawfile
		{
			namespace
			{
				std::string read_rawfile(RawFile* asset)
				{
					if (asset->compressedLen > 0)
					{
						std::string uncompressed;
						uncompressed.resize(asset->len);

						uncompress(reinterpret_cast<unsigned char*>(uncompressed.data()), reinterpret_cast<uLongf*>(&asset->len),
							reinterpret_cast<const Bytef*>(asset->buffer), asset->compressedLen);

						return uncompressed;
					}
					else if (asset->len > 0)
					{
						return std::string(asset->buffer, static_cast<std::size_t>(asset->len));
					}

					return "";
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

			void generate_sky_technique(float sky_intensity, const float* sky_tint, utils::memory::allocator& allocator)
			{
				const auto techset = zonetool::iw6::db_find_x_asset_header_safe(ASSET_TYPE_TECHNIQUE_SET, "wc_sky_bl").techniqueSet;
				if (techset == nullptr)
				{
					return;
				}
				
				constexpr const auto adjust = 1.5f;

				ZONETOOL_INFO("Generating sky technique for vision (%f, %f %f %f)...", sky_intensity, sky_tint[0], sky_tint[1], sky_tint[2]);
				const auto new_tech = allocator.allocate<MaterialTechniqueSet>();
				std::memcpy(new_tech, techset, sizeof(MaterialTechniqueSet));

				const auto technique = techset->techniques[TECHNIQUE_LIT_DIR];
				const auto new_technique = allocator.allocate<MaterialTechnique>();

				std::memcpy(new_technique, technique, sizeof(MaterialTechnique));

				new_tech->techniques[TECHNIQUE_LIT_DIR] = new_technique;

				const auto arg_count = technique->passArray[0].perObjArgCount +
					technique->passArray[0].perPrimArgCount +
					technique->passArray[0].stableArgCount;
				const auto new_args = allocator.allocate_array<MaterialShaderArgument>(arg_count);
				std::memcpy(new_args, new_technique->passArray[0].args, sizeof(MaterialShaderArgument) * arg_count);
				new_technique->passArray[0].args = new_args;

				for (auto i = 0; i < arg_count; i++)
				{
					if (new_args[i].type == MTL_ARG_CODE_CONST && new_args[i].u.codeConst.index == CONST_SCR_CODE_HDR_SKY_INTENSITY)
					{
						new_args[i].type = MTL_ARG_LITERAL_CONST;
						const auto literal = allocator.allocate_array<float>(4);
						literal[0] = adjust * sky_intensity * sky_tint[0];
						literal[1] = adjust * sky_intensity * sky_tint[1];
						literal[2] = adjust * sky_intensity * sky_tint[2];
						literal[3] = adjust * sky_intensity * 1.f;
						new_args[i].u.literalConst = literal;
					}
				}

				const auto converted_tech = techset::convert(new_tech, allocator);
				zonetool::h1::techset::dump_technique(converted_tech->techniques[zonetool::h1::TECHNIQUE_LIT_DIR]);
			}

			std::string convert_vision_file(const std::string& name, const std::string& data, utils::memory::allocator& allocator)
			{
				const auto lines = utils::string::split(data, '\n');
				std::unordered_map<std::string, std::string> values;

				for (const auto& line : lines)
				{
					std::regex regex("(.+?)\\s+\"(.+)\"");
					std::smatch match;

					if (!std::regex_search(line, match, regex))
					{
						continue;
					}

					values.insert(std::make_pair(match[1], match[2]));
				}

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
						buffer.append(utils::string::va("%s \"%s\"\r\n", name.data(), default_value.data()));
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
				write_value("r_glowUseAltCutoff", "1");
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
				write_value("r_filmEnable", "1");
				write_value("r_filmContrast", "1");
				write_value("r_filmBrightness", "1");
				write_value("r_filmDesaturation", "0.4");
				write_value("r_filmDesaturationDark", "0.4");
				write_value("r_filmInvert", "0");
				write_value("r_filmLightTint", "1 1 1", true);
				write_value("r_filmMediumTint", "1 1 1", true);
				write_value("r_filmDarkTint", "1 1 1", true);
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
				separator();
				write_value("r_chromaticAberrationMode", "");
				write_value("r_chromaticSeparation", " ");
				write_value("r_chromaticAberrationAlpha", "");
				
				const auto sky_intensity = parse_vector(values["r_hdrSkyIntensity"]);
				const auto sky_color_tint = parse_vector(values["r_hdrSkyColorTint"]);

				if (name == filesystem::get_fastfile() && sky_color_tint.size() == 3 && sky_intensity.size() == 1)
				{
					generate_sky_technique(sky_intensity[0], sky_color_tint.data(), allocator);
				}

				return buffer;
			}

			std::string convert_fog_file(const std::string& name, const std::string& data, utils::memory::allocator& allocator)
			{
				const auto lines = utils::string::split(data, '\n');

				std::string buffer;

				for (const auto& line : lines)
				{
					if (line.contains("HDRColorIntensity") || line.contains("HDRSunColorIntensity"))
					{
						continue;
					}

					buffer.append(line);
					buffer.append("\n");
				}

				const std::string hdr_name = utils::string::va("maps/createart/%s_fog_hdr.gsc", name.data());
				const auto new_asset = allocator.allocate<zonetool::h1::RawFile>();
				new_asset->name = allocator.duplicate_string(hdr_name);
				new_asset->buffer = allocator.duplicate_string(buffer);
				new_asset->len = static_cast<int>(buffer.size());
				new_asset->compressedLen = 0;
				zonetool::h1::rawfile::dump(new_asset);

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
