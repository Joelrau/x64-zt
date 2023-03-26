#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "techset.hpp"

//#include "zonetool/h1/assets/techset.hpp"

#include "zonetool/s1/functions.hpp"

#include "domainshader.hpp"
#include "hullshader.hpp"
#include "pixelshader.hpp"
#include "vertexdecl.hpp"
#include "vertexshader.hpp"

#include "zonetool/h1/assets/vertexdecl.hpp"

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace techset
		{
			namespace ree
			{
				void dump_constant_buffer_indexes(const std::string& techset, unsigned char* cbi)
				{
					const auto path = "techsets\\constantbuffer\\"s + techset + ".cbi";
					auto file = filesystem::file(path);
					file.open("wb");
					auto fp = file.get_fp();

					if (fp)
					{
						fwrite(cbi, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
						file.close();
					}
				}

				void dump_constant_buffer_def_array(const std::string& techset, unsigned char count, zonetool::h1::MaterialConstantBufferDef* def)
				{
					const auto path = "techsets\\constantbuffer\\"s + techset + ".cbt";
					assetmanager::dumper dump;
					if (!dump.open(path))
					{
						return;
					}

					dump.dump_int(count);
					dump.dump_array(def, count);
					for (int i = 0; i < count; i++)
					{
						if (def[i].vsData)
						{
							dump.dump_array(def[i].vsData, def[i].vsDataSize);
						}
						if (def[i].hsData)
						{
							dump.dump_array(def[i].hsData, def[i].hsDataSize);
						}
						if (def[i].dsData)
						{
							dump.dump_array(def[i].dsData, def[i].dsDataSize);
						}
						if (def[i].psData)
						{
							dump.dump_array(def[i].psData, def[i].psDataSize);
						}
						if (def[i].vsOffsetData)
						{
							dump.dump_array(def[i].vsOffsetData, def[i].vsOffsetDataSize);
						}
						if (def[i].hsOffsetData)
						{
							dump.dump_array(def[i].hsOffsetData, def[i].hsOffsetDataSize);
						}
						if (def[i].dsOffsetData)
						{
							dump.dump_array(def[i].dsOffsetData, def[i].dsOffsetDataSize);
						}
						if (def[i].psOffsetData)
						{
							dump.dump_array(def[i].psOffsetData, def[i].psOffsetDataSize);
						}
					}

					dump.close();
				}

				void dump_stateinfo(const std::string& techset, zonetool::h1::Material* mat)
				{
					const auto path = "techsets\\state\\"s + techset + ".stateinfo";

					ordered_json json_data = {};

					json_data["stateFlags"] = mat->stateFlags;

					auto file = filesystem::file(path);
					file.open("wb");
					auto fp = file.get_fp();
					if (fp)
					{
						const auto json_dump = json_data.dump(4);
						file.write(json_dump.data(), json_dump.size(), 1);
						file.close();
					}
				}

				void dump_statebits(const std::string& techset, unsigned char* statebits)
				{
					const auto path = "techsets\\state\\"s + techset + ".statebits";
					auto file = filesystem::file(path);
					file.open("wb");
					auto fp = file.get_fp();

					if (fp)
					{
						fwrite(statebits, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT, 1, fp);
						file.close();
					}
				}

				void dump_statebits_map(const std::string& techset, zonetool::h1::GfxStateBits* map, unsigned char count)
				{
					const auto path = "techsets\\state\\"s + techset + ".statebitsmap";

					ordered_json json_data = {};
					for (unsigned char i = 0; i < count; i++)
					{
						XGfxGlobals* varXGfxGlobals = zonetool::s1::GetXGfxGlobalsForZone(map[i].zone);
						ordered_json entry;
						entry["loadBits"][0] = map[i].loadBits[0];
						entry["loadBits"][1] = map[i].loadBits[1];
						entry["loadBits"][2] = map[i].loadBits[2];
						entry["loadBits"][3] = map[i].loadBits[3];
						entry["loadBits"][4] = map[i].loadBits[4];
						entry["loadBits"][5] = map[i].loadBits[5];
						for (int j = 0; j < 10; j++)
						{
							entry["depthStencilStateBits"][j] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[j]] : 0;
						}
						for (int j = 0; j < 3; j++)
						{
							entry["blendStateBits"][j] = varXGfxGlobals ? varXGfxGlobals->blendStateBits[map[i].blendState][j] : 0;
						}
						entry["rasterizerState"] = map[i].rasterizerState;
						json_data[i] = entry;
					}

					auto file = filesystem::file(path);
					file.open("wb");
					auto fp = file.get_fp();
					if (fp)
					{
						const auto json_dump = json_data.dump(4);
						file.write(json_dump.data(), json_dump.size(), 1);
						file.close();
					}
				}

				void dump_technique(zonetool::h1::MaterialTechnique* asset)
				{
					const auto path = "techsets\\"s + asset->hdr.name + ".technique";

					assetmanager::dumper dumper;
					if (!dumper.open(path))
					{
						return;
					}

					dumper.dump_single(&asset->hdr);
					dumper.dump_array(asset->passArray, asset->hdr.passCount);

					dumper.dump_string(asset->hdr.name);
					for (unsigned short i = 0; i < asset->hdr.passCount; i++)
					{
						if (asset->passArray[i].vertexShader)
						{
							dumper.dump_asset(asset->passArray[i].vertexShader);
							//zonetool::h1::IVertexShader::dump(asset->passArray[i].vertexShader);
						}

						if (asset->passArray[i].vertexDecl)
						{
							dumper.dump_asset(asset->passArray[i].vertexDecl);
							zonetool::h1::IVertexDecl::dump(asset->passArray[i].vertexDecl);
						}

						if (asset->passArray[i].hullShader)
						{
							dumper.dump_asset(asset->passArray[i].hullShader);
							//zonetool::h1::IHullShader::dump(asset->passArray[i].hullShader);
						}

						if (asset->passArray[i].domainShader)
						{
							dumper.dump_asset(asset->passArray[i].domainShader);
							//zonetool::h1::IDomainShader::dump(asset->passArray[i].domainShader);
						}

						if (asset->passArray[i].pixelShader)
						{
							dumper.dump_asset(asset->passArray[i].pixelShader);
							//zonetool::h1::IPixelShader::dump(asset->passArray[i].pixelShader);
						}

						if (asset->passArray[i].args)
						{
							dumper.dump_array(asset->passArray[i].args, asset->passArray[i].perObjArgCount + asset->passArray[i].perPrimArgCount + asset->passArray[i].stableArgCount);
							for (auto arg = 0; arg < asset->passArray[i].perObjArgCount + asset->passArray[i].perPrimArgCount + asset->passArray[i].stableArgCount; arg++)
							{
								if (asset->passArray[i].args[arg].type == 4)
								{
									dumper.dump_array(const_cast<float*>(asset->passArray[i].args[arg].u.literalConst), 4);
								}
							}
						}
					}

					dumper.close();
				}

				void yeet(zonetool::h1::MaterialTechniqueSet* asset)
				{
					const auto path = "techsets\\"s + asset->name + ".techset.txt";

					filesystem::file file(path);
					file.open("wb");
					auto fp = file.get_fp();

					for (auto i = 0u; i < zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
					{
						if (asset->techniques[i])
						{
							fprintf(fp, "%i: %s\n", i, asset->techniques[i]->hdr.name);
						}
						else
						{
							fprintf(fp, "%i: nullptr\n", i);
						}
					}

					file.close();
				}

				void dump(MaterialTechniqueSet* s1_asset, ZoneMemory* mem)
				{
					auto* asset = converter::h1::techset::convert(s1_asset, mem);

					const auto path = "techsets\\"s + asset->name + ".techset";

					yeet(asset);

					assetmanager::dumper dumper;
					if (!dumper.open(path))
					{
						return;
					}

					dumper.dump_single(asset);
					dumper.dump_string(asset->name);

					for (auto i = 0u; i < zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT; i++)
					{
						if (asset->techniques[i])
						{
							dumper.dump_string(asset->techniques[i]->hdr.name);
							dump_technique(asset->techniques[i]);
						}
					}

					dumper.close();
				}
			}

			const std::unordered_map <std::uint16_t, std::uint16_t> const_src_code_map =
			{
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE1, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET1, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE2, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET2, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE3, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET3, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_PHYSICALSIZE_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_FADEOFFSET_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_COUNT, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_COUNT},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_TYPES},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR},
				{zonetool::s1::CONST_SRC_CODE_GAMETIME, zonetool::h1::CONST_SRC_CODE_GAMETIME},
				{zonetool::s1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::h1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
				{zonetool::s1::CONST_SRC_CODE_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_EYEOFFSET},
				{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR, zonetool::h1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR},
				{zonetool::s1::CONST_SRC_CODE_MODEL_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_PARMS},
				{zonetool::s1::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_SKINNED_PARMS},
				{zonetool::s1::CONST_SRC_CODE_HUD_OUTLINE_PARMS, zonetool::h1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR}, //
				//{zonetool::s1::CONST_SRC_CODE_CHROMATIC_ABERRATION_PARMS, zonetool::h1::CONST_SRC_CODE_CHROMATIC_ABERRATION_PARMS},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL0, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL0},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL1, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL1},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL2, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL2},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL3, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL3},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL4, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL4},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL5, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL5},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL6, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL6},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL7, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL7},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL8, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL8},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROL9, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL9},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLA, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLA},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLB, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLB},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLC, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLC},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLD, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLD},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLE, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLE},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_CONTROLF, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLF},
				{zonetool::s1::CONST_SRC_CODE_SCRIPT_PARMS, zonetool::h1::CONST_SRC_CODE_SCRIPT_PARMS},
				{zonetool::s1::CONST_SRC_CODE_EFFECT_MODEL_COLOR, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR},
				{zonetool::s1::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE, zonetool::h1::CONST_SRC_CODE_EFFECT_MODEL_COLOR_EMISSIVE},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_0, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_0},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_1, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_1},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_2, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_2},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_3, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_3},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_4, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_4},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_5, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_5},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_6, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_6},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_7, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_7},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_8, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_8},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_9, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_9},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_10, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_10},
				{zonetool::s1::CONST_SRC_CODE_FILTER_TAP_11, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_11},
				{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_R, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_R},
				{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_G, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_G},
				{zonetool::s1::CONST_SRC_CODE_COLOR_MATRIX_B, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_B},
				{zonetool::s1::CONST_SRC_CODE_RENDER_TARGET_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_TARGET_SIZE},
				{zonetool::s1::CONST_SRC_CODE_RENDER_SOURCE_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_SOURCE_SIZE},
				{zonetool::s1::CONST_SRC_CODE_VEIL_PARAMS, zonetool::h1::CONST_SRC_CODE_VEIL_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_FXAA_RCPFRAMEOPT, zonetool::h1::CONST_SRC_CODE_FXAA_RCPFRAMEOPT},
				{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1},
				{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2},
				{zonetool::s1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3},
				{zonetool::s1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::h1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_NEARPLANE_ORG, zonetool::h1::CONST_SRC_CODE_NEARPLANE_ORG},
				{zonetool::s1::CONST_SRC_CODE_NEARPLANE_DX, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DX},
				{zonetool::s1::CONST_SRC_CODE_NEARPLANE_DY, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DY},
				{zonetool::s1::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP, zonetool::h1::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP},
				{zonetool::s1::CONST_SRC_CODE_SSAO_COLOR_COEFF, zonetool::h1::CONST_SRC_CODE_SSAO_COLOR_COEFF},
				{zonetool::s1::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE},
				{zonetool::s1::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE, zonetool::h1::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE},
				{zonetool::s1::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE},
				{zonetool::s1::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT, zonetool::h1::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_LIGHTDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_LIGHTDIRX2},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR_THIN, zonetool::h1::CONST_SRC_CODE_LIT2D_HUDBLOOD_COLOR_THIN},
				{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR},
				{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION_SCENE, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_SCENE},
				{zonetool::s1::CONST_SRC_CODE_DOF_LERP_SCALE, zonetool::h1::CONST_SRC_CODE_DOF_LERP_SCALE},
				{zonetool::s1::CONST_SRC_CODE_DOF_LERP_BIAS, zonetool::h1::CONST_SRC_CODE_DOF_LERP_BIAS},
				{zonetool::s1::CONST_SRC_CODE_DOF_ROW_DELTA, zonetool::h1::CONST_SRC_CODE_DOF_ROW_DELTA},
				{zonetool::s1::CONST_SRC_CODE_DOF_EQUATION, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION},
				{zonetool::s1::CONST_SRC_CODE_DOF_PARAMS, zonetool::h1::CONST_SRC_CODE_DOF_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_DOF_TILE_PARAMS, zonetool::h1::CONST_SRC_CODE_DOF_TILE_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_FRAME_COUNT_PARAMS, zonetool::h1::CONST_SRC_CODE_FRAME_COUNT_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_X, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_X},
				{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_Y, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_Y},
				{zonetool::s1::CONST_SRC_CODE_MOTION_MATRIX_W, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_W},
				{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{zonetool::s1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				{zonetool::s1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::h1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
				{zonetool::s1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS, zonetool::h1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS},
				{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS, zonetool::h1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS},
				{zonetool::s1::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF, zonetool::h1::CONST_SRC_CODE_SSS_MRT_OFF_SHADER_OFF},
				{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R0},
				{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R1},
				{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R2},
				{zonetool::s1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_MDAO_VIEWTOOCCLUDER_MATRIX_R3},
				{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R0},
				{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R1},
				{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R2},
				{zonetool::s1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_MDAO_WORLDTOOCCLUDERFRAME_MATRIX_R3},
				{zonetool::s1::CONST_SRC_CODE_MDAO_SCREEN_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_SCREEN_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MDAO_CAMERA_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_CAMERA_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MDAO_LOOKUP_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_LOOKUP_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MDAO_VOLUME_PARAMS, zonetool::h1::CONST_SRC_CODE_MDAO_VOLUME_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS},
				{zonetool::s1::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS},
				{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS},
				{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS},
				{zonetool::s1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS, zonetool::h1::CONST_SRC_CODE_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_STATIC_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_STATIC_VELOCITY_PARMS},
				{zonetool::s1::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS},
				{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS},
				{zonetool::s1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BIAS},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_SCALE, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SCALE},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_PARTITION_UV_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_CASCADE_MASK},
				{zonetool::s1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_DISTANCE_BLEND},
				{zonetool::s1::CONST_SRC_CODE_ZNEAR, zonetool::h1::CONST_SRC_CODE_ZNEAR},
				{zonetool::s1::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE},
				{zonetool::s1::CONST_SRC_CODE_INV_SCENE_PROJECTION, zonetool::h1::CONST_SRC_CODE_INV_SCENE_PROJECTION},
				{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_0_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_DIR},
				{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_0_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_COLOR},
				{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_1_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_DIR},
				{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_1_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_COLOR},
				{zonetool::s1::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE},
				{zonetool::s1::CONST_SRC_CODE_DEBUG_BUMPMAP, zonetool::h1::CONST_SRC_CODE_DEBUG_BUMPMAP},
				{zonetool::s1::CONST_SRC_CODE_MATERIAL_COLOR, zonetool::h1::CONST_SRC_CODE_MATERIAL_COLOR},
				{zonetool::s1::CONST_SRC_CODE_FOG, zonetool::h1::CONST_SRC_CODE_FOG},
				{zonetool::s1::CONST_SRC_CODE_FOG_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_LINEAR},
				{zonetool::s1::CONST_SRC_CODE_FOG_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_GAMMA},
				{zonetool::s1::CONST_SRC_CODE_FOG_SUN_CONSTS, zonetool::h1::CONST_SRC_CODE_FOG_SUN_CONSTS},
				{zonetool::s1::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR},
				{zonetool::s1::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA},
				{zonetool::s1::CONST_SRC_CODE_FOG_SUN_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_DIR},
				{zonetool::s1::CONST_SRC_CODE_FOG_SKY_FOG, zonetool::h1::CONST_SRC_CODE_FOG_SKY_FOG},
				{zonetool::s1::CONST_SRC_CODE_FOG_HEIGHT_FOG, zonetool::h1::CONST_SRC_CODE_FOG_HEIGHT_FOG},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_0, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_0},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_1, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_1},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_2, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_2},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_3, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_3},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_4, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_4},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_5, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_5},
				{zonetool::s1::CONST_SRC_CODE_ATMOS_FOG_PARMS_6, zonetool::h1::CONST_SRC_CODE_ATMOS_FOG_PARMS_6},
				{zonetool::s1::CONST_SRC_CODE_FOG_SKY_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SKY_DIR},
				{zonetool::s1::CONST_SRC_CODE_FOG_DEPTHHACK, zonetool::h1::CONST_SRC_CODE_FOG_DEPTHHACK},
				{zonetool::s1::CONST_SRC_CODE_GLOW_SETUP, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP},
				{zonetool::s1::CONST_SRC_CODE_GLOW_APPLY, zonetool::h1::CONST_SRC_CODE_GLOW_APPLY},
				{zonetool::s1::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE},
				{zonetool::s1::CONST_SRC_CODE_COLOR_BIAS, zonetool::h1::CONST_SRC_CODE_COLOR_BIAS},
				{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_BASE, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_BASE},
				{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_DELTA},
				{zonetool::s1::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA},
				{zonetool::s1::CONST_SRC_CODE_PERCEPTUAL_PARMS, zonetool::h1::CONST_SRC_CODE_PERCEPTUAL_PARMS},
				{zonetool::s1::CONST_SRC_CODE_FILMIC_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_PARMS},
				{zonetool::s1::CONST_SRC_CODE_FILMIC_SHOULDER_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_SHOULDER_PARMS},
				{zonetool::s1::CONST_SRC_CODE_FILMIC_TOE_PARMS, zonetool::h1::CONST_SRC_CODE_FILMIC_TOE_PARMS},
				{zonetool::s1::CONST_SRC_CODE_TONEMAP_PARMS, zonetool::h1::CONST_SRC_CODE_TONEMAP_PARMS},
				{zonetool::s1::CONST_SRC_CODE_HDR_STAGE_PARMS, zonetool::h1::CONST_SRC_CODE_HDR_STAGE_PARMS},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_0, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_0},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_1, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_1},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_2, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_2},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_3, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_3},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_4, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_4},
				{zonetool::s1::CONST_SRC_CODE_UI3D_UV_SETUP_5, zonetool::h1::CONST_SRC_CODE_UI3D_UV_SETUP_5},
				{zonetool::s1::CONST_SRC_CODE_HUDFX_PARMS, zonetool::h1::CONST_SRC_CODE_HUDFX_PARMS},
				{zonetool::s1::CONST_SRC_CODE_HUDFX_PARMS2, zonetool::h1::CONST_SRC_CODE_HUDFX_PARMS2},
				{zonetool::s1::CONST_SRC_CODE_SSSS_PARAMS, zonetool::h1::CONST_SRC_CODE_SSSS_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_SSSS_SCALE, zonetool::h1::CONST_SRC_CODE_SSSS_SCALE},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_0, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_0},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_1, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_1},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_2, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_2},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_3, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_3},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_4, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_4},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_5, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_5},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_6, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_6},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_7, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_7},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_8, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_8},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_9, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_9},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_10, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_10},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_11, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_11},
				{zonetool::s1::CONST_SRC_CODE_SSSS_KERNEL_12, zonetool::h1::CONST_SRC_CODE_SSSS_KERNEL_12},
				{zonetool::s1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, zonetool::h1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
				{zonetool::s1::CONST_SRC_CODE_ENVMAP_PARMS, zonetool::h1::CONST_SRC_CODE_ENVMAP_PARMS},
				{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_CASCADE_V_CLAMP},
				{zonetool::s1::CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_NEAR_FAR_PLANE},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_3},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_1},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_2},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_3},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_0},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_1},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_2},
				{zonetool::s1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_ZTRANSFORM_ARRAY_3},
				{zonetool::s1::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION, zonetool::h1::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION},
				{zonetool::s1::CONST_SRC_CODE_POSTFX_FADE_EFFECT, zonetool::h1::CONST_SRC_CODE_POSTFX_FADE_EFFECT},
				{zonetool::s1::CONST_SRC_CODE_SCENE_UV_MULT, zonetool::h1::CONST_SRC_CODE_SCENE_UV_MULT},
				{zonetool::s1::CONST_SRC_CODE_SCENE_VIEW_MULT, zonetool::h1::CONST_SRC_CODE_SCENE_VIEW_MULT},
				{zonetool::s1::CONST_SRC_CODE_TEXCOORD_CLAMP_0, zonetool::h1::CONST_SRC_CODE_TEXCOORD_CLAMP_0},
				{zonetool::s1::CONST_SRC_CODE_TEXCOORD_CLAMP_1, zonetool::h1::CONST_SRC_CODE_TEXCOORD_CLAMP_1},
				{zonetool::s1::CONST_SRC_CODE_VIEWPORT_DIMENSIONS, zonetool::h1::CONST_SRC_CODE_VIEWPORT_DIMENSIONS},
				{zonetool::s1::CONST_SRC_CODE_FRAMEBUFFER_READ, zonetool::h1::CONST_SRC_CODE_FRAMEBUFFER_READ},
				{zonetool::s1::CONST_SRC_CODE_THERMAL_COLOR_OFFSET, zonetool::h1::CONST_SRC_CODE_THERMAL_COLOR_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_THERMAL_FADE_CONTROL, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_CONTROL},
				{zonetool::s1::CONST_SRC_CODE_THERMAL_FADE_COLOR, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_COLOR},
				{zonetool::s1::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS, zonetool::h1::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS},
				{zonetool::s1::CONST_SRC_CODE_TESSELLATION_PARMS, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS},
				{zonetool::s1::CONST_SRC_CODE_TESSELLATION_PARMS2, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS2},
				{zonetool::s1::CONST_SRC_CODE_EYE_PARAMETER_DVAR, zonetool::h1::CONST_SRC_CODE_EYE_PARAMETER_DVAR},
				{zonetool::s1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR1, zonetool::h1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR1},
				{zonetool::s1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR2, zonetool::h1::CONST_SRC_CODE_MP_RIM_PARAMETER_DVAR2},
				{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R0, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R0},
				{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R1, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R1},
				{zonetool::s1::CONST_SRC_CODE_HEAD_ROT_MAT_R2, zonetool::h1::CONST_SRC_CODE_HEAD_ROT_MAT_R2},
				{zonetool::s1::CONST_SRC_CODE_GUN_SIGHT_COLOR, zonetool::h1::CONST_SRC_CODE_GUN_SIGHT_COLOR},
				{zonetool::s1::CONST_SRC_CODE_GUN_RETICLE_COLOR, zonetool::h1::CONST_SRC_CODE_GUN_RETICLE_COLOR},
				{zonetool::s1::CONST_SRC_CODE_BASE_LIGHTING_COORDS, zonetool::h1::CONST_SRC_CODE_BASE_LIGHTING_COORDS},
				{zonetool::s1::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, zonetool::h1::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT},
				{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
				{zonetool::s1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_TEXTURE_ATLAS_SETTINGS},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_VEL_WORLD},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0},
				{zonetool::s1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1},
				{zonetool::s1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS},
				{zonetool::s1::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV, zonetool::h1::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14},
				{zonetool::s1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15},
				{zonetool::s1::CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::h1::CONST_SRC_CODE_DEPTH_FROM_CLIP},
				{zonetool::s1::CONST_SRC_CODE_CODE_MESH_ARG_0, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_0},
				{zonetool::s1::CONST_SRC_CODE_CODE_MESH_ARG_1, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_1},
				{zonetool::s1::CONST_SRC_CODE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::s1::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX},
				{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::s1::CONST_SRC_CODE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::s1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
			};

			const std::unordered_map <std::uint16_t, std::uint16_t> texture_src_code_map =
			{
				{zonetool::s1::TEXTURE_SRC_CODE_BLACK, zonetool::h1::TEXTURE_SRC_CODE_BLACK},
				{zonetool::s1::TEXTURE_SRC_CODE_WHITE, zonetool::h1::TEXTURE_SRC_CODE_WHITE},
				{zonetool::s1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::h1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
				{zonetool::s1::TEXTURE_SRC_CODE_MODEL_LIGHTING, zonetool::h1::TEXTURE_SRC_CODE_MODEL_LIGHTING},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY},
				{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
				{zonetool::s1::TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_TRANS_SHADOWMAP_SUN},
				{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT},
				{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1},
				{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2},
				{zonetool::s1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3},
				{zonetool::s1::TEXTURE_SRC_CODE_FEEDBACK, zonetool::h1::TEXTURE_SRC_CODE_FEEDBACK},
				{zonetool::s1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN},
				{zonetool::s1::TEXTURE_SRC_CODE_SSR_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_SSR_BUFFER},
				{zonetool::s1::TEXTURE_SRC_CODE_HQ_SSR_DEPTH, zonetool::h1::TEXTURE_SRC_CODE_HQ_SSR_DEPTH},
				{zonetool::s1::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER, zonetool::h1::TEXTURE_SRC_CODE_BLUR_DISTORTION_BUFFER},
				{zonetool::s1::TEXTURE_SRC_CODE_RESOLVED_SCENE, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_SCENE},
				{zonetool::s1::TEXTURE_SRC_CODE_POST_EFFECT_0, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_0},
				{zonetool::s1::TEXTURE_SRC_CODE_POST_EFFECT_1, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_1},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3},
				{zonetool::s1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3},
				{zonetool::s1::TEXTURE_SRC_CODE_OUTDOOR, zonetool::h1::TEXTURE_SRC_CODE_OUTDOOR},
				{zonetool::s1::TEXTURE_SRC_CODE_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ},
				{zonetool::s1::TEXTURE_SRC_CODE_PROCESSED_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_PROCESSED_FLOATZ},
				{zonetool::s1::TEXTURE_SRC_CODE_RAW_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_RAW_FLOATZ},
				{zonetool::s1::TEXTURE_SRC_CODE_FLOATZ_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ_STENCIL},
				{zonetool::s1::TEXTURE_SRC_CODE_RAW_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_RAW_STENCIL},
				{zonetool::s1::TEXTURE_SRC_CODE_HALF_PARTICLES, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES},
				{zonetool::s1::TEXTURE_SRC_CODE_HALF_PARTICLES_Z, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES_Z},
				{zonetool::s1::TEXTURE_SRC_CODE_CASE_TEXTURE, zonetool::h1::TEXTURE_SRC_CODE_CASE_TEXTURE},
				{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_Y, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_Y},
				{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CR},
				{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CB, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CB},
				{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_A, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_A},
				//{zonetool::s1::TEXTURE_SRC_CODE_CINEMATIC_CBCR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CBCR},
				{zonetool::s1::TEXTURE_SRC_CODE_REFLECTION_PROBE, zonetool::h1::TEXTURE_SRC_CODE_REFLECTION_PROBE},
				{zonetool::s1::TEXTURE_SRC_CODE_DALTONIZE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_DALTONIZE_CLUT},
				{zonetool::s1::TEXTURE_SRC_CODE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_CLUT},
				{zonetool::s1::TEXTURE_SRC_CODE_LINEAR_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_LINEAR_CLAMP},
				{zonetool::s1::TEXTURE_SRC_CODE_POINT_CLAMP, zonetool::h1::TEXTURE_SRC_CODE_POINT_CLAMP},
				{zonetool::s1::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE, zonetool::h1::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE},
				{zonetool::s1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS, zonetool::h1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS},
				{zonetool::s1::TEXTURE_SRC_CODE_SSAO, zonetool::h1::TEXTURE_SRC_CODE_SSAO},
				{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0},
				{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1},
				{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2},
				{zonetool::s1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3},
				{zonetool::s1::TEXTURE_SRC_CODE_MSAA_SCENE, zonetool::h1::TEXTURE_SRC_CODE_MSAA_SCENE},
				{zonetool::s1::TEXTURE_SRC_CODE_DEPTH_BLUR_0, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_0},
				{zonetool::s1::TEXTURE_SRC_CODE_DEPTH_BLUR_1, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_1},
				{zonetool::s1::TEXTURES_SRC_CODE_DEFAULT_DEPTH, zonetool::h1::TEXTURES_SRC_CODE_DEFAULT_DEPTH},
				{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY},
				{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE0, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE0},
				{zonetool::s1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE1, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_TILE1},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_DOF_FLOATZ},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_PREPASS, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_PREPASS},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_COLOR, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_COLOR},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_HALF_ALPHA, zonetool::h1::TEXTURE_SRC_CODE_DOF_HALF_ALPHA},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_TILE0, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE0},
				{zonetool::s1::TEXTURE_SRC_CODE_DOF_TILE1, zonetool::h1::TEXTURE_SRC_CODE_DOF_TILE1},
				{zonetool::s1::TEXTURE_SRC_CODE_SCENE_SPECULAR, zonetool::h1::TEXTURE_SRC_CODE_SCENE_SPECULAR},
				{zonetool::s1::TEXTURE_SRC_CODE_SCENE_ALBEDO, zonetool::h1::TEXTURE_SRC_CODE_SCENE_ALBEDO},
				{zonetool::s1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA, zonetool::h1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA},
				{zonetool::s1::TEXTURE_SRC_CODE_UI3D, zonetool::h1::TEXTURE_SRC_CODE_UI3D},
				{zonetool::s1::TEXTURE_SRC_CODE_STREAMING_LOADING, zonetool::h1::TEXTURE_SRC_CODE_STREAMING_LOADING},
				{zonetool::s1::TEXTURE_SRC_CODE_MDAO_HDIR, zonetool::h1::TEXTURE_SRC_CODE_MDAO_HDIR},
				{zonetool::s1::TEXTURE_SRC_CODE_MDAO_DEPTH, zonetool::h1::TEXTURE_SRC_CODE_MDAO_DEPTH},
				{zonetool::s1::TEXTURE_SRC_CODE_MDAO_OCCLUSION_VOLUME, zonetool::h1::TEXTURE_SRC_CODE_MDAO_OCCLUSION_VOLUME},
				{zonetool::s1::TEXTURE_SRC_CODE_MDAO_LOOKUP, zonetool::h1::TEXTURE_SRC_CODE_MDAO_LOOKUP},
				{zonetool::s1::TEXTURE_SRC_CODE_TEXTURE_0, zonetool::h1::TEXTURE_SRC_CODE_TEXTURE_0},
				{zonetool::s1::TEXTURE_SRC_CODE_COMPOSITE_RESULT, zonetool::h1::TEXTURE_SRC_CODE_COMPOSITE_RESULT},
				//{zonetool::s1::TEXTURE_SRC_CODE_FONT_CACHE, zonetool::h1::TEXTURE_SRC_CODE_FONT_CACHE},
			};

			const std::unordered_map <std::uint8_t, std::uint8_t> worldvertexformat_map =
			{
				{0, 0},
				{1, 1},
				{2, 4},
				{3, 7},
				{4, 11},
				{5, 15},
				{6, 19},
				{7, 24},
				{8, 29},
				{9, 34},
				{10, 39},
				{11, 45},
				{12, 51},
				{13, 57},
				{14, 63},
			};

			std::unordered_map<std::string, zonetool::h1::MaterialTechniqueSet*> converted_techset_assets;

			zonetool::h1::MaterialTechniqueSet* convert(MaterialTechniqueSet* asset, ZoneMemory* mem)
			{
				if (converted_techset_assets.contains(asset->name + TECHSET_PREFIX))
				{
					return converted_techset_assets[asset->name];
				}

				auto* new_asset = mem->Alloc<zonetool::h1::MaterialTechniqueSet>();

				new_asset->name = mem->StrDup(asset->name + TECHSET_PREFIX);
				new_asset->flags = asset->flags; // convert?

				if (worldvertexformat_map.contains(asset->worldVertFormat))
				{
					new_asset->worldVertFormat = worldvertexformat_map.at(asset->worldVertFormat);
				}
				else
				{
					ZONETOOL_ERROR("Unable to map worldVertFormat %d for technique '%s'!\n", asset->worldVertFormat, asset->name);
					new_asset->worldVertFormat = 0;
				}

				new_asset->preDisplacementOnlyCount = asset->preDisplacementOnlyCount;

				std::unordered_map<std::uintptr_t, zonetool::h1::MaterialTechnique*> converted_asset_techniques;
				for (std::int32_t i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (asset->techniques[i])
					{
						if (converted_asset_techniques.contains(reinterpret_cast<std::uintptr_t>(asset->techniques[i])))
						{
							new_asset->techniques[i] = converted_asset_techniques[reinterpret_cast<std::uintptr_t>(asset->techniques[i])];
						}
						else
						{
							const auto size = sizeof(MaterialTechniqueHeader) + sizeof(MaterialPass) * asset->techniques[i]->hdr.passCount;
							new_asset->techniques[i] = mem->ManualAlloc<zonetool::h1::MaterialTechnique>(size); // no need to change index, it's the same

							auto* technique = asset->techniques[i];
							auto* new_technique = new_asset->techniques[i];

							std::memcpy(new_technique, technique, size); // same struct

							new_technique->hdr.name = mem->StrDup(technique->hdr.name + TECHSET_PREFIX);

							for (unsigned short pass_index = 0; pass_index < technique->hdr.passCount; pass_index++)
							{
								auto* pass = &technique->passArray[pass_index];
								auto* new_pass = &new_technique->passArray[pass_index];

								if (pass->vertexShader)
								{
									new_pass->vertexShader = converter::h1::vertexshader::convert(pass->vertexShader, mem);
								}
								if (pass->vertexDecl)
								{
									new_pass->vertexDecl = converter::h1::vertexdecl::convert(pass->vertexDecl, mem);
								}
								if (pass->hullShader)
								{
									new_pass->hullShader = converter::h1::hullshader::convert(pass->hullShader, mem);
								}
								if (pass->domainShader)
								{
									new_pass->domainShader = converter::h1::domainshader::convert(pass->domainShader, mem);
								}
								if (pass->pixelShader)
								{
									new_pass->pixelShader = converter::h1::pixelshader::convert(pass->pixelShader, mem);
								}

								//new_pass->stableArgSize += 16;

								if (pass->args)
								{
									const auto arg_count = pass->perObjArgCount + pass->perPrimArgCount + pass->stableArgCount;
									new_pass->args = mem->Alloc<zonetool::h1::MaterialShaderArgument>(arg_count);
									std::memcpy(new_pass->args, pass->args, sizeof(MaterialShaderArgument) * arg_count); // same struct
									for (auto arg_index = 0; arg_index < arg_count; arg_index++)
									{
										auto* arg = &pass->args[arg_index];
										auto* new_arg = &new_pass->args[arg_index];

										if (arg->type == MTL_ARG_CODE_CONST)
										{
											if (const_src_code_map.contains(arg->u.codeConst.index))
											{
												auto new_index = const_src_code_map.at(arg->u.codeConst.index);
												new_arg->u.codeConst.index = new_index;

												assert(new_arg->u.codeConst.index < zonetool::h1::CONST_SRC_TOTAL_COUNT);
											}
											else
											{
												ZONETOOL_ERROR("Unable to map code constant %d for technique '%s'!", arg->u.codeConst.index, asset->name);
												new_arg->u.codeConst.index = 0;
												new_arg->u.codeConst.firstRow = 0;
												new_arg->u.codeConst.rowCount = 0;
											}
										}
										else if (arg->type == MTL_ARG_CODE_TEXTURE)
										{
											if (texture_src_code_map.contains(arg->u.codeConst.index))
											{
												auto new_index = texture_src_code_map.at(arg->u.codeConst.index);
												new_arg->u.codeConst.index = new_index;

												assert(new_arg->u.codeConst.index < zonetool::h1::TEXTURE_SRC_CODE_COUNT);
											}
											else
											{
												ZONETOOL_ERROR("Unable to map code sampler %d for technique '%s'!\n", arg->u.codeSampler, asset->name);
												new_arg->u.codeConst.index = zonetool::h1::TEXTURE_SRC_CODE_BLACK;
											}
										}
									}
								}
							}
							converted_asset_techniques[reinterpret_cast<std::uintptr_t>(asset->techniques[i])] = new_technique;
						}
					}
				}

				converted_techset_assets[asset->name] = new_asset;
				return new_asset;
			}

			void dump(MaterialTechniqueSet* asset, ZoneMemory* mem)
			{
				ree::dump(asset, mem);
			}
		}
	}
}