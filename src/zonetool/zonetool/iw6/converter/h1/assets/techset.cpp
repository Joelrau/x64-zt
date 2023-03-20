#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "techset.hpp"

//#include "zonetool/h1/assets/techset.hpp"

#include "zonetool/iw6/zonetool.hpp"

#include "domainshader.hpp"
#include "hullshader.hpp"
#include "pixelshader.hpp"
#include "vertexdecl.hpp"
#include "vertexshader.hpp"

#include "zonetool/h1/assets/vertexdecl.hpp"

#define TECHSET_PREFIX "_iw6"s

namespace zonetool::iw6
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

				void dump_statebits_map(const std::string& techset, GfxStateBits* map, unsigned char count)
				{
					const auto path = "techsets\\state\\"s + techset + ".statebitsmap";

					ordered_json json_data = {};
					for (unsigned char i = 0; i < count; i++)
					{
						XGfxGlobals* varXGfxGlobals = zonetool::iw6::GetXGfxGlobalsForZone(map[i].zone);
						ordered_json entry;
						entry["loadBits"][0] = map[i].loadBits[0]; // convert?
						entry["loadBits"][1] = map[i].loadBits[1]; // convert?
						entry["loadBits"][2] = 0xFFFF;
						entry["loadBits"][3] = map[i].loadBits[2]; // convert?
						entry["loadBits"][4] = 0;
						entry["loadBits"][5] = 0;
						
						entry["depthStencilStateBits"][0] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[0]] : 0;
						entry["depthStencilStateBits"][1] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[1]] : 0;
						entry["depthStencilStateBits"][2] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[3]] : 0;
						entry["depthStencilStateBits"][3] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[4]] : 0;
						entry["depthStencilStateBits"][4] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[2]] : 0;
						entry["depthStencilStateBits"][5] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[5]] : 0;
						entry["depthStencilStateBits"][6] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[6]] : 0;
						entry["depthStencilStateBits"][7] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[8]] : 0;
						entry["depthStencilStateBits"][8] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[9]] : 0;
						entry["depthStencilStateBits"][9] = varXGfxGlobals ? varXGfxGlobals->depthStencilStateBits[map[i].depthStencilState[10]] : 0;
						// 0 -> 0
						// 1 -> 1/2/6/7
						// 2 -> 3
						// 3 -> 4
						// 4 -> 1/2/6/7
						// 5 -> 5
						// 6 -> 1/2/6/7
						// 7 -> 8
						// 8 -> 9
						// 9 -> 10
						// 
						//

						entry["blendStateBits"][0] = varXGfxGlobals ? varXGfxGlobals->blendStateBits[map[i].blendState][0] : 0;
						entry["blendStateBits"][1] = 0;
						entry["blendStateBits"][2] = 0;

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

				void dump(MaterialTechniqueSet* iw6_asset, ZoneMemory* mem)
				{
					auto* asset = converter::h1::techset::convert(iw6_asset, mem);

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

			const std::unordered_map <std::uint8_t, std::uint8_t> worldvertexformat_map =
			{
				{0, 0},
				{1, 1},
			};

			const std::unordered_map <std::uint16_t, std::uint16_t> const_src_code_map =
			{
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_POSITION_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_DIFFUSE_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPECULAR_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTDIR_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_SPOTFACTORS_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_CUCOLORIS_ANIM_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3, zonetool::h1::CONST_SRC_CODE_LIGHT_FALLOFF_PLACEMENT_DB_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DYN_COUNT, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_COUNT},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DYN_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_TYPES},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES, zonetool::h1::CONST_SRC_CODE_LIGHT_DYN_SHADOW_TYPES},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_COLOR},
				{zonetool::iw6::CONST_SRC_CODE_GAMETIME, zonetool::h1::CONST_SRC_CODE_GAMETIME},
				{zonetool::iw6::CONST_SRC_CODE_GENERIC_MATERIAL_DATA, zonetool::h1::CONST_SRC_CODE_GENERIC_MATERIAL_DATA},
				{zonetool::iw6::CONST_SRC_CODE_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX_EYE_OFFSET}, // EYEOFFSET -> WORLD_MATRIX_EYE_OFFSET
				{zonetool::iw6::CONST_SRC_CODE_MODEL_MOTIONBLUR_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_MODEL_VELOCITY_PARMS}, //
				{zonetool::iw6::CONST_SRC_CODE_HUD_OUTLINE_PARMS, zonetool::h1::CONST_SRC_CODE_EMBLEM_LUT_SELECTOR}, //
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL0, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL0},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL1, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL1},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL2, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL2},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL3, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL3},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL4, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL4},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL5, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL5},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL6, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL6},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL7, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL7},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL8, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL8},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROL9, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROL9},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLA, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLA},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLB, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLB},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLC, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLC},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLD, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLD},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLE, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLE},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_CONTROLF, zonetool::h1::CONST_SRC_CODE_POSTFX_CONTROLF},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_0, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_0},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_1, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_1},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_2, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_2},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_3, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_3},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_4, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_4},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_5, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_5},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_6, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_6},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_7, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_7},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_8, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_8},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_9, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_9},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_10, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_10},
				{zonetool::iw6::CONST_SRC_CODE_FILTER_TAP_11, zonetool::h1::CONST_SRC_CODE_FILTER_TAP_11},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_MATRIX_R, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_R},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_MATRIX_G, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_G},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_MATRIX_B, zonetool::h1::CONST_SRC_CODE_COLOR_MATRIX_B},
				{zonetool::iw6::CONST_SRC_CODE_RENDER_TARGET_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_TARGET_SIZE},
				{zonetool::iw6::CONST_SRC_CODE_RENDER_SOURCE_SIZE, zonetool::h1::CONST_SRC_CODE_RENDER_SOURCE_SIZE},
				//{zonetool::iw6::CONST_SRC_CODE_BLOOM_HQ_RADIUS, zonetool::h1::CONST_SRC_CODE_BLOOM_HQ_RADIUS},
				//{zonetool::iw6::CONST_SRC_CODE_BLOOM_HQ_METERING_SENSITIVITY, zonetool::h1::CONST_SRC_CODE_BLOOM_HQ_METERING_SENSITIVITY},
				//{zonetool::iw6::CONST_SRC_CODE_BLOOM_HQ_IRIS_PARAMS, zonetool::h1::CONST_SRC_CODE_BLOOM_HQ_IRIS_PARAMS},
				//{zonetool::iw6::CONST_SRC_CODE_BLOOM_HQ_FALLOFF_1, zonetool::h1::CONST_SRC_CODE_BLOOM_HQ_FALLOFF_1},
				//{zonetool::iw6::CONST_SRC_CODE_BLOOM_HQ_FALLOFF_2, zonetool::h1::CONST_SRC_CODE_BLOOM_HQ_FALLOFF_2},
				{zonetool::iw6::CONST_SRC_CODE_FXAA_RCPFRAMEOPT, zonetool::h1::CONST_SRC_CODE_FXAA_RCPFRAMEOPT},
				{zonetool::iw6::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_1},
				{zonetool::iw6::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_2},
				{zonetool::iw6::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3, zonetool::h1::CONST_SRC_CODE_VOLUME_LIGHT_SCATTER_3},
				{zonetool::iw6::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS, zonetool::h1::CONST_SRC_CODE_MODEL_LIGHTMAP_PARAMS},
				//{zonetool::iw6::CONST_SRC_CODE_AVG_LUMINANCE_PARAMS, zonetool::h1::CONST_SRC_CODE_AVG_LUMINANCE_PARAMS},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP, zonetool::h1::CONST_SRC_CODE_SSAO_POWER_BB_SHARPNESS_STEP},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_COLOR_COEFF, zonetool::h1::CONST_SRC_CODE_SSAO_COLOR_COEFF},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_FALLOFF_DEPTH_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE, zonetool::h1::CONST_SRC_CODE_SSAO_UV_TO_ROT_SCALE_AND_FADE},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE, zonetool::h1::CONST_SRC_CODE_SSAO_SAMPLE_MAT_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT, zonetool::h1::CONST_SRC_CODE_SSAO_DEPTH_OFFSET_REJECT},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_LIGHTDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_LIGHTDIRX2},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECHALFANGLEDIRX2},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_AMBIENTCOLOR},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_DIFFUSECOLOR},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_SPECCOLOR_SPECEXPONENT},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_ADDITIVECOLOR},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_AMBIENTCOLOR},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_DIFFUSECOLOR},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_SPECCOLOR_SPECEXPONENT},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_ADDITIVECOLOR},
				{zonetool::iw6::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_VIEWMODEL_AND_FAR_BLUR},
				{zonetool::iw6::CONST_SRC_CODE_DOF_EQUATION_SCENE, zonetool::h1::CONST_SRC_CODE_DOF_EQUATION_SCENE},
				{zonetool::iw6::CONST_SRC_CODE_DOF_LERP_SCALE, zonetool::h1::CONST_SRC_CODE_DOF_LERP_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_DOF_LERP_BIAS, zonetool::h1::CONST_SRC_CODE_DOF_LERP_BIAS},
				{zonetool::iw6::CONST_SRC_CODE_DOF_ROW_DELTA, zonetool::h1::CONST_SRC_CODE_DOF_ROW_DELTA},
				{zonetool::iw6::CONST_SRC_CODE_MOTION_MATRIX_X, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_X},
				{zonetool::iw6::CONST_SRC_CODE_MOTION_MATRIX_Y, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_Y},
				{zonetool::iw6::CONST_SRC_CODE_MOTION_MATRIX_W, zonetool::h1::CONST_SRC_CODE_MOTION_MATRIX_W},
				{zonetool::iw6::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{zonetool::iw6::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{zonetool::iw6::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{zonetool::iw6::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_SSR_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				{zonetool::iw6::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM, zonetool::h1::CONST_SRC_CODE_PREV_EYEPOSITION_TRANSFORM},
				{zonetool::iw6::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_FADE, zonetool::h1::CONST_SRC_CODE_SCREEN_SPACE_REFLECTION_PARAMETERS}, //
				{zonetool::iw6::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE_AND_OFFSET},
				{zonetool::iw6::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS, zonetool::h1::CONST_SRC_CODE_SSR_CLIP_TO_FADE_SCALE_OFFSET_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_ALPHA_TEST_OFFSETS_PS, zonetool::h1::CONST_SRC_CODE_MSAA_ALPHA_TEST_OFFSETS_PS},
				{zonetool::iw6::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_DISTORTION_SAMPLE_LIMITS_PS},
				{zonetool::iw6::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS, zonetool::h1::CONST_SRC_CODE_SCOPE_SAMPLE_LIMITS_PS},
				{zonetool::iw6::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C0_PS},
				{zonetool::iw6::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C1_PS},
				{zonetool::iw6::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS, zonetool::h1::CONST_SRC_CODE_MB_UV_TO_PREV_SCALED_CLIP_MAT_C3_PS},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R0},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R1},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R2},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_VIEWPROJECTION_MATRIX_R3},
				//{zonetool::iw6::CONST_SRC_CODE_SCENE_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_SCENE_EYEOFFSET},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_EYEOFFSET, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX_EYE_OFFSET}, //
				{zonetool::iw6::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_PARAMS},
				{zonetool::iw6::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_TILE_MAX_PARAMS},
				//{zonetool::iw6::CONST_SRC_CODE_MOTION_BLUR_HQ_DECODE_SCALE_BIAS, zonetool::h1::CONST_SRC_CODE_MOTION_BLUR_HQ_DECODE_SCALE_BIAS},
				//{zonetool::iw6::CONST_SRC_CODE_STATIC_MOTIONBLUR_VELOCITY_PARMS, zonetool::h1::CONST_SRC_CODE_STATIC_MOTIONBLUR_VELOCITY_PARMS},
				{zonetool::iw6::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SWITCH_PARTITION_ARRAY_0}, //
				{zonetool::iw6::CONST_SRC_CODE_SHADOWMAP_SCALE, zonetool::h1::CONST_SRC_CODE_SHADOWMAP_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_ZNEAR, zonetool::h1::CONST_SRC_CODE_ZNEAR},
				{zonetool::iw6::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_LIGHTING_LOOKUP_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_INV_SCENE_PROJECTION, zonetool::h1::CONST_SRC_CODE_INV_SCENE_PROJECTION},
				{zonetool::iw6::CONST_SRC_CODE_RIM_LIGHT_0_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_DIR},
				{zonetool::iw6::CONST_SRC_CODE_RIM_LIGHT_0_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_0_COLOR},
				{zonetool::iw6::CONST_SRC_CODE_RIM_LIGHT_1_DIR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_DIR},
				{zonetool::iw6::CONST_SRC_CODE_RIM_LIGHT_1_COLOR, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_1_COLOR},
				{zonetool::iw6::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE, zonetool::h1::CONST_SRC_CODE_RIM_LIGHT_TECHNIQUE},
				//{zonetool::iw6::CONST_SCR_CODE_HDR_SKY_INTENSITY, zonetool::h1::CONST_SCR_CODE_HDR_SKY_INTENSITY},
				{zonetool::iw6::CONST_SRC_CODE_DEBUG_BUMPMAP, zonetool::h1::CONST_SRC_CODE_DEBUG_BUMPMAP},
				{zonetool::iw6::CONST_SRC_CODE_MATERIAL_COLOR, zonetool::h1::CONST_SRC_CODE_MATERIAL_COLOR},
				{zonetool::iw6::CONST_SRC_CODE_FOG, zonetool::h1::CONST_SRC_CODE_FOG},
				{zonetool::iw6::CONST_SRC_CODE_FOG_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_LINEAR},
				{zonetool::iw6::CONST_SRC_CODE_FOG_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_COLOR_GAMMA},
				{zonetool::iw6::CONST_SRC_CODE_FOG_SUN_CONSTS, zonetool::h1::CONST_SRC_CODE_FOG_SUN_CONSTS},
				{zonetool::iw6::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_LINEAR},
				{zonetool::iw6::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA, zonetool::h1::CONST_SRC_CODE_FOG_SUN_COLOR_GAMMA},
				{zonetool::iw6::CONST_SRC_CODE_FOG_SUN_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SUN_DIR},
				{zonetool::iw6::CONST_SRC_CODE_FOG_HEIGHT, zonetool::h1::CONST_SRC_CODE_FOG_HEIGHT_FOG}, //
				{zonetool::iw6::CONST_SRC_CODE_FOG_SKY_DIR, zonetool::h1::CONST_SRC_CODE_FOG_SKY_DIR},
				{zonetool::iw6::CONST_SRC_CODE_FOG_DEPTHHACK, zonetool::h1::CONST_SRC_CODE_FOG_DEPTHHACK},
				{zonetool::iw6::CONST_SRC_CODE_GLOW_SETUP, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP},
				{zonetool::iw6::CONST_SRC_CODE_GLOW_APPLY, zonetool::h1::CONST_SRC_CODE_GLOW_APPLY},
				{zonetool::iw6::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE, zonetool::h1::CONST_SRC_CODE_GLOW_SETUP_ALT_COLOR_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_BIAS, zonetool::h1::CONST_SRC_CODE_COLOR_BIAS},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_TINT_BASE, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_BASE},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_TINT_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_DELTA},
				{zonetool::iw6::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA, zonetool::h1::CONST_SRC_CODE_COLOR_TINT_QUADRATIC_DELTA},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_BIAS_AND_RANGE1, zonetool::h1::CONST_SRC_CODE_MCOLOR_BIAS_AND_RANGE1},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE1, zonetool::h1::CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE1},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT1, zonetool::h1::CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT1},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_BIAS2_AND_RANGE_SCALE, zonetool::h1::CONST_SRC_CODE_MCOLOR_BIAS2_AND_RANGE_SCALE},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE2, zonetool::h1::CONST_SRC_CODE_MCOLOR_CONTRAST_SCALE2},
				//{zonetool::iw6::CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT2, zonetool::h1::CONST_SRC_CODE_MCOLOR_CONTRAST_POW_AND_SAT2},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_DEPTH_SCALE_AND_BIAS, zonetool::h1::CONST_SRC_CODE_COLORIZATION_DEPTH_SCALE_AND_BIAS},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ALTERNATE, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ALTERNATE},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_ALTERNATE, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_ALTERNATE},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_ALTERNATE, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_ALTERNATE},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ALTERNATE_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_ALTERNATE_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_ALTERNATE_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_ALTERNATE_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_ALTERNATE_ARRAY, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_ALTERNATE_ARRAY},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_LOW_AND_HIGH_END_POINTS, zonetool::h1::CONST_SRC_CODE_COLORIZATION_LOW_AND_HIGH_END_POINTS},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_MID_END_POINTS, zonetool::h1::CONST_SRC_CODE_COLORIZATION_MID_END_POINTS},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_LOW, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_LOW},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_LOW, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_LOW},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_LOW, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_LOW},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_MID, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_MID},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_MID, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_MID},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_MID, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_MID},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_HIGH, zonetool::h1::CONST_SRC_CODE_COLORIZATION_POWER_AND_DESATURATION_HIGH},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_SCALE_HIGH, zonetool::h1::CONST_SRC_CODE_COLORIZATION_SCALE_HIGH},
				//{zonetool::iw6::CONST_SRC_CODE_COLORIZATION_BIAS_HIGH, zonetool::h1::CONST_SRC_CODE_COLORIZATION_BIAS_HIGH},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_TOE_STRENGTH_TOE_NUMERATOR, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_TOE_STRENGTH_TOE_NUMERATOR},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_TOE_STRENGTH_TOE_DENOMINATOR, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_TOE_STRENGTH_TOE_DENOMINATOR},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_TOE_ANGLE, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_TOE_ANGLE},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_LINEAR_STRENGTH, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_LINEAR_STRENGTH},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_LINEAR_ANGLE, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_LINEAR_ANGLE},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_TOE_SHOULDER_STRENGTH, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_TOE_SHOULDER_STRENGTH},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_WHITE_POINT, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_WHITE_POINT},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_EXPOSURE, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_EXPOSURE},
				//{zonetool::iw6::CONST_SRC_CODE_TONE_MAPPING_WHITE_ADAPTATION_RATES, zonetool::h1::CONST_SRC_CODE_TONE_MAPPING_WHITE_ADAPTATION_RATES},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C0_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C0_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C1_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C1_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C2_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C2_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C3_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C3_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C4_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C4_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C5_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C5_PS},
				//{zonetool::iw6::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C6_PS, zonetool::h1::CONST_SRC_CODE_MSAA_RESOLVE_INV_TONE_MAP_C6_PS},
				{zonetool::iw6::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS, zonetool::h1::CONST_SRC_CODE_OUTDOOR_FEATHER_PARMS},
				{zonetool::iw6::CONST_SRC_CODE_ENVMAP_PARMS, zonetool::h1::CONST_SRC_CODE_ENVMAP_PARMS},
				{zonetool::iw6::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST, zonetool::h1::CONST_SRC_CODE_SUN_SHADOWMAP_PIXEL_ADJUST_ARRAY_0}, //
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_1},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_2},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_0},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_1},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_2},
				{zonetool::iw6::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3, zonetool::h1::CONST_SRC_CODE_SPOT_SHADOWMAP_PIXEL_ADJUST_ARRAY_3},
				{zonetool::iw6::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION, zonetool::h1::CONST_SRC_CODE_COMPOSITE_FX_DISTORTION},
				{zonetool::iw6::CONST_SRC_CODE_POSTFX_FADE_EFFECT, zonetool::h1::CONST_SRC_CODE_POSTFX_FADE_EFFECT},
				//{zonetool::iw6::CONST_SRC_CODE_SNOWAMBIENTCOLOR, zonetool::h1::CONST_SRC_CODE_SNOWAMBIENTCOLOR},
				{zonetool::iw6::CONST_SRC_CODE_VIEWPORT_DIMENSIONS, zonetool::h1::CONST_SRC_CODE_VIEWPORT_DIMENSIONS},
				{zonetool::iw6::CONST_SRC_CODE_FRAMEBUFFER_READ, zonetool::h1::CONST_SRC_CODE_FRAMEBUFFER_READ},
				{zonetool::iw6::CONST_SRC_CODE_THERMAL_COLOR_OFFSET, zonetool::h1::CONST_SRC_CODE_THERMAL_COLOR_OFFSET},
				{zonetool::iw6::CONST_SRC_CODE_THERMAL_FADE_CONTROL, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_CONTROL},
				{zonetool::iw6::CONST_SRC_CODE_THERMAL_FADE_COLOR, zonetool::h1::CONST_SRC_CODE_THERMAL_FADE_COLOR},
				{zonetool::iw6::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS, zonetool::h1::CONST_SRC_CODE_PLAYLIST_POPULATION_PARAMS},
				{zonetool::iw6::CONST_SRC_CODE_TESSELLATION_PARMS, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS},
				{zonetool::iw6::CONST_SRC_CODE_TESSELLATION_PARMS2, zonetool::h1::CONST_SRC_CODE_TESSELLATION_PARMS2},
				//{zonetool::iw6::CONST_SRC_CODE_BLACK_LEVEL, zonetool::h1::CONST_SRC_CODE_BLACK_LEVEL},
				{zonetool::iw6::CONST_SRC_CODE_BASE_LIGHTING_COORDS, zonetool::h1::CONST_SRC_CODE_BASE_LIGHTING_COORDS},
				{zonetool::iw6::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT, zonetool::h1::CONST_SRC_CODE_LIGHT_PROBE_AMBIENT},
				{zonetool::iw6::CONST_SRC_CODE_NEARPLANE_ORG, zonetool::h1::CONST_SRC_CODE_NEARPLANE_ORG},
				{zonetool::iw6::CONST_SRC_CODE_NEARPLANE_DX, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DX},
				{zonetool::iw6::CONST_SRC_CODE_NEARPLANE_DY, zonetool::h1::CONST_SRC_CODE_NEARPLANE_DY},
				{zonetool::iw6::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_SCALE},
				{zonetool::iw6::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET, zonetool::h1::CONST_SRC_CODE_CLIP_SPACE_LOOKUP_OFFSET},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR0},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR1},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2, zonetool::h1::CONST_SRC_CODE_PARTICLE_CLOUD_SPARK_COLOR2},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM0},
				{zonetool::iw6::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1, zonetool::h1::CONST_SRC_CODE_PARTICLE_FOUNTAIN_PARM1},
				{zonetool::iw6::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS, zonetool::h1::CONST_SRC_CODE_LIT2D_BLOODSPATTER_FADESHARPNESS},
				{zonetool::iw6::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV, zonetool::h1::CONST_SRC_CODE_BLUR_SCENE_PIXEL_POS_TO_UV},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_0},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_1},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_2},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_3},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_4},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_5},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_6},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_7},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_8},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_9},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_10},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_11},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_12},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_13},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_14},
				{zonetool::iw6::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15, zonetool::h1::CONST_SRC_CODE_REACTIVEMOTION_CENTERS_15},
				{zonetool::iw6::CONST_SRC_CODE_DEPTH_FROM_CLIP, zonetool::h1::CONST_SRC_CODE_DEPTH_FROM_CLIP},
				{zonetool::iw6::CONST_SRC_CODE_CODE_MESH_ARG_0, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_0},
				{zonetool::iw6::CONST_SRC_CODE_CODE_MESH_ARG_1, zonetool::h1::CONST_SRC_CODE_CODE_MESH_ARG_1},
				{zonetool::iw6::CONST_SRC_CODE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_VIEW_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_VIEW_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_VIEW_PROJECTION_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_SHADOW_LOOKUP_MATRIX3},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_OUTDOOR_LOOKUP_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_WORLD_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_WORLD_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_TRANSPOSE_WORLD_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX, zonetool::h1::CONST_SRC_CODE_PREV_FRAME_INVERSE_TRANSPOSE_WORLD_MATRIX},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX0},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX1},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
				{zonetool::iw6::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2, zonetool::h1::CONST_SRC_CODE_INVERSE_TRANSPOSE_WORLD_VIEW_PROJECTION_MATRIX2},
			};

			const std::unordered_map <std::uint16_t, std::uint16_t> texture_src_code_map =
			{
				{zonetool::iw6::TEXTURE_SRC_CODE_BLACK, zonetool::h1::TEXTURE_SRC_CODE_BLACK},
				{zonetool::iw6::TEXTURE_SRC_CODE_WHITE, zonetool::h1::TEXTURE_SRC_CODE_WHITE},
				{zonetool::iw6::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP, zonetool::h1::TEXTURE_SRC_CODE_IDENTITY_NORMAL_MAP},
				{zonetool::iw6::TEXTURE_SRC_CODE_MODEL_LIGHTING, zonetool::h1::TEXTURE_SRC_CODE_MODEL_LIGHTING},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_PRIMARY},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY, zonetool::h1::TEXTURE_SRC_CODE_LIGHTMAP_SECONDARY},
				{zonetool::iw6::TEXTURE_SRC_CODE_SHADOWMAP_SUN, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SUN},
				{zonetool::iw6::TEXTURE_SRC_CODE_SHADOWMAP_SPOT, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT},
				{zonetool::iw6::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_1},
				{zonetool::iw6::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_2},
				{zonetool::iw6::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3, zonetool::h1::TEXTURE_SRC_CODE_SHADOWMAP_SPOT_3},
				{zonetool::iw6::TEXTURE_SRC_CODE_FEEDBACK, zonetool::h1::TEXTURE_SRC_CODE_FEEDBACK},
				{zonetool::iw6::TEXTURE_SRC_CODE_RESOLVED_POST_SUN, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_POST_SUN},
				{zonetool::iw6::TEXTURE_SRC_CODE_RESOLVED_SCENE, zonetool::h1::TEXTURE_SRC_CODE_RESOLVED_SCENE},
				//{zonetool::iw6::TEXTURE_SRC_CODE_HDR_SCENE_R11G11B10_MIPMAP, zonetool::h1::TEXTURE_SRC_CODE_HDR_SCENE_R11G11B10_MIPMAP},
				{zonetool::iw6::TEXTURE_SRC_CODE_POST_EFFECT_0, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_0},
				{zonetool::iw6::TEXTURE_SRC_CODE_POST_EFFECT_1, zonetool::h1::TEXTURE_SRC_CODE_POST_EFFECT_1},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_ATTENUATION, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION1},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS1},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION2},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS2},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_ATTENUATION3},
				{zonetool::iw6::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3, zonetool::h1::TEXTURE_SRC_CODE_LIGHT_CUCOLORIS3},
				{zonetool::iw6::TEXTURE_SRC_CODE_OUTDOOR, zonetool::h1::TEXTURE_SRC_CODE_OUTDOOR},
				{zonetool::iw6::TEXTURE_SRC_CODE_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ},
				{zonetool::iw6::TEXTURE_SRC_CODE_PROCESSED_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_PROCESSED_FLOATZ},
				{zonetool::iw6::TEXTURE_SRC_CODE_RAW_FLOATZ, zonetool::h1::TEXTURE_SRC_CODE_RAW_FLOATZ},
				{zonetool::iw6::TEXTURE_SRC_CODE_FLOATZ_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_FLOATZ_STENCIL},
				{zonetool::iw6::TEXTURE_SRC_CODE_RAW_STENCIL, zonetool::h1::TEXTURE_SRC_CODE_RAW_STENCIL},
				{zonetool::iw6::TEXTURE_SRC_CODE_HALF_PARTICLES, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES},
				{zonetool::iw6::TEXTURE_SRC_CODE_HALF_PARTICLES_Z, zonetool::h1::TEXTURE_SRC_CODE_HALF_PARTICLES_Z},
				{zonetool::iw6::TEXTURE_SRC_CODE_CASE_TEXTURE, zonetool::h1::TEXTURE_SRC_CODE_CASE_TEXTURE},
				{zonetool::iw6::TEXTURE_SRC_CODE_CINEMATIC_Y, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_Y},
				{zonetool::iw6::TEXTURE_SRC_CODE_CINEMATIC_CR, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CR},
				{zonetool::iw6::TEXTURE_SRC_CODE_CINEMATIC_CB, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_CB},
				{zonetool::iw6::TEXTURE_SRC_CODE_CINEMATIC_A, zonetool::h1::TEXTURE_SRC_CODE_CINEMATIC_A},
				{zonetool::iw6::TEXTURE_SRC_CODE_REFLECTION_PROBE, zonetool::h1::TEXTURE_SRC_CODE_REFLECTION_PROBE},
				{zonetool::iw6::TEXTURE_SRC_CODE_DALTONIZE_CLUT, zonetool::h1::TEXTURE_SRC_CODE_DALTONIZE_CLUT},
				//{zonetool::iw6::TEXTURE_SRC_CODE_MACROCLUT, zonetool::h1::TEXTURE_SRC_CODE_MACROCLUT},
				//{zonetool::iw6::TEXTURE_SRC_CODE_PIP_SCENE, zonetool::h1::TEXTURE_SRC_CODE_PIP_SCENE},
				{zonetool::iw6::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE, zonetool::h1::TEXTURE_SRC_CODE_BLUR_SCENE_SAVE},
				//{zonetool::iw6::TEXTURE_SRC_CODE_CHARACTER_SCENE, zonetool::h1::TEXTURE_SRC_CODE_CHARACTER_SCENE},
				{zonetool::iw6::TEXTURE_SRC_CODE_RANDOM_ROTATIONS, zonetool::h1::TEXTURE_SRC_CODE_RANDOM_ROTATIONS},
				{zonetool::iw6::TEXTURE_SRC_CODE_SSAO, zonetool::h1::TEXTURE_SRC_CODE_SSAO},
				{zonetool::iw6::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_0},
				{zonetool::iw6::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_1},
				{zonetool::iw6::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_2},
				{zonetool::iw6::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3, zonetool::h1::TEXTURE_SRC_CODE_POSTFX_TEXTURE_3},
				//{zonetool::iw6::TEXTURE_SRC_CODE_COLORIZATION_ALTERNATE_MASK, zonetool::h1::TEXTURE_SRC_CODE_COLORIZATION_ALTERNATE_MASK},
				//{zonetool::iw6::TEXTURE_SRC_CODE_COLOR_GRADING_CLUT, zonetool::h1::TEXTURE_SRC_CODE_COLOR_GRADING_CLUT},
				{zonetool::iw6::TEXTURE_SRC_CODE_DEPTH_BLUR_0, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_0},
				{zonetool::iw6::TEXTURE_SRC_CODE_DEPTH_BLUR_1, zonetool::h1::TEXTURE_SRC_CODE_DEPTH_BLUR_1},
				//{zonetool::iw6::TEXTURE_SRC_CODE_SCENE_LUMINANCE, zonetool::h1::TEXTURE_SRC_CODE_SCENE_LUMINANCE},
				{zonetool::iw6::TEXTURES_SRC_CODE_DEFAULT_DEPTH, zonetool::h1::TEXTURES_SRC_CODE_DEFAULT_DEPTH},
				//{zonetool::iw6::TEXTURE_SRC_CODE_DYNAMIC_IRIS_EXPOSURE, zonetool::h1::TEXTURE_SRC_CODE_DYNAMIC_IRIS_EXPOSURE},
				{zonetool::iw6::TEXTURE_SRC_CODE_SCENE_VELOCITY, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY},
				//{zonetool::iw6::TEXTURE_SRC_CODE_SCENE_VELOCITY_PASS0, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_PASS0},
				//{zonetool::iw6::TEXTURE_SRC_CODE_SCENE_VELOCITY_PASS1, zonetool::h1::TEXTURE_SRC_CODE_SCENE_VELOCITY_PASS1},
				{zonetool::iw6::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA, zonetool::h1::TEXTURE_SRC_CODE_GENERIC_FLOAT_DATA},
				//{zonetool::iw6::TEXTURE_SRC_CODE_APEX0_DATA, zonetool::h1::TEXTURE_SRC_CODE_APEX0_DATA},
				//{zonetool::iw6::TEXTURE_SRC_CODE_APEX1_DATA, zonetool::h1::TEXTURE_SRC_CODE_APEX1_DATA},
				//{zonetool::iw6::TEXTURE_SRC_CODE_APEX2_DATA, zonetool::h1::TEXTURE_SRC_CODE_APEX2_DATA},
				//{zonetool::iw6::TEXTURE_SRC_CODE_VOXEL_DATA, zonetool::h1::TEXTURE_SRC_CODE_VOXEL_DATA},
			};

			std::unordered_map <std::int32_t, std::int32_t> technique_index_map =
			{
				{zonetool::iw6::TECHNIQUE_DEPTH_PREPASS, zonetool::h1::TECHNIQUE_ZPREPASS},
				{zonetool::iw6::TECHNIQUE_BUILD_SHADOWMAP_DEPTH, zonetool::h1::TECHNIQUE_BUILD_SHADOWMAP_DEPTH},
				{zonetool::iw6::TECHNIQUE_UNLIT, zonetool::h1::TECHNIQUE_UNLIT},
				{zonetool::iw6::TECHNIQUE_EMISSIVE, zonetool::h1::TECHNIQUE_EMISSIVE},
				{zonetool::iw6::TECHNIQUE_EMISSIVE_DFOG, zonetool::h1::TECHNIQUE_EMISSIVE_DFOG},
				{zonetool::iw6::TECHNIQUE_EMISSIVE_SHADOW, zonetool::h1::TECHNIQUE_EMISSIVE_SHADOW},
				{zonetool::iw6::TECHNIQUE_EMISSIVE_SHADOW_DFOG, zonetool::h1::TECHNIQUE_EMISSIVE_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT, zonetool::h1::TECHNIQUE_LIT},
				{zonetool::iw6::TECHNIQUE_LIT_DIR, zonetool::h1::TECHNIQUE_LIT_DIR},
				{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT, zonetool::h1::TECHNIQUE_LIT_SPOT},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_LIT_OMNI, zonetool::h1::TECHNIQUE_LIT_OMNI},
				{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_OMNI, zonetool::h1::TECHNIQUE_LIT_DIR_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_DIR_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_OMNI, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_OMNI, zonetool::h1::TECHNIQUE_LIT_SPOT_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_OMNI, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_OMNI, zonetool::h1::TECHNIQUE_LIT_OMNI_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_OMNI_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW_OMNI, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW},
				{zonetool::iw6::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_LIT_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_LIT_DFOG, zonetool::h1::TECHNIQUE_LIT_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_DIR_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_LIGHT_SPOT, zonetool::h1::TECHNIQUE_LIGHT_SPOT},
				{zonetool::iw6::TECHNIQUE_LIGHT_OMNI, zonetool::h1::TECHNIQUE_LIGHT_OMNI},
				{zonetool::iw6::TECHNIQUE_LIGHT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_LIGHT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_LIGHT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_LIGHT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_FAKELIGHT_NORMAL, zonetool::h1::TECHNIQUE_FAKELIGHT_NORMAL},
				{zonetool::iw6::TECHNIQUE_FAKELIGHT_VIEW, zonetool::h1::TECHNIQUE_FAKELIGHT_VIEW},
				{zonetool::iw6::TECHNIQUE_SUNLIGHT_PREVIEW, zonetool::h1::TECHNIQUE_SUNLIGHT_PREVIEW},
				{zonetool::iw6::TECHNIQUE_CASE_TEXTURE, zonetool::h1::TECHNIQUE_CASE_TEXTURE},
				{zonetool::iw6::TECHNIQUE_WIREFRAME_SOLID, zonetool::h1::TECHNIQUE_WIREFRAME_SOLID},
				{zonetool::iw6::TECHNIQUE_WIREFRAME_SHADED, zonetool::h1::TECHNIQUE_WIREFRAME_SHADED},
				{zonetool::iw6::TECHNIQUE_THERMAL, zonetool::h1::TECHNIQUE_THERMAL},
				{zonetool::iw6::TECHNIQUE_VELOCITY_RIGID, zonetool::h1::TECHNIQUE_VELOCITY_RIGID},
				{zonetool::iw6::TECHNIQUE_VELOCITY_SKINNED, zonetool::h1::TECHNIQUE_VELOCITY_SKINNED},
				{zonetool::iw6::TECHNIQUE_DEBUG_BUMPMAP, zonetool::h1::TECHNIQUE_DEBUG_BUMPMAP},
				{zonetool::iw6::TECHNIQUE_INSTANCED_DEPTH_PREPASS, zonetool::h1::TECHNIQUE_INSTANCED_ZPREPASS},
				{zonetool::iw6::TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_DEPTH, zonetool::h1::TECHNIQUE_INSTANCED_BUILD_SHADOWMAP_DEPTH},
				{zonetool::iw6::TECHNIQUE_INSTANCED_UNLIT, zonetool::h1::TECHNIQUE_INSTANCED_UNLIT},
				{zonetool::iw6::TECHNIQUE_INSTANCED_EMISSIVE, zonetool::h1::TECHNIQUE_INSTANCED_EMISSIVE},
				{zonetool::iw6::TECHNIQUE_INSTANCED_EMISSIVE_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_EMISSIVE_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_EMISSIVE_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_EMISSIVE_SHADOW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_EMISSIVE_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_EMISSIVE_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT, zonetool::h1::TECHNIQUE_INSTANCED_LIT},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_SHADOW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_INSTANCED_LIT_SUN_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIGHT_SPOT, zonetool::h1::TECHNIQUE_INSTANCED_LIGHT_SPOT},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIGHT_OMNI, zonetool::h1::TECHNIQUE_INSTANCED_LIGHT_OMNI},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_INSTANCED_LIGHT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_INSTANCED_FAKELIGHT_NORMAL, zonetool::h1::TECHNIQUE_INSTANCED_FAKELIGHT_NORMAL},
				{zonetool::iw6::TECHNIQUE_INSTANCED_FAKELIGHT_VIEW, zonetool::h1::TECHNIQUE_INSTANCED_FAKELIGHT_VIEW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_SUNLIGHT_PREVIEW, zonetool::h1::TECHNIQUE_INSTANCED_SUNLIGHT_PREVIEW},
				{zonetool::iw6::TECHNIQUE_INSTANCED_CASE_TEXTURE, zonetool::h1::TECHNIQUE_INSTANCED_CASE_TEXTURE},
				{zonetool::iw6::TECHNIQUE_INSTANCED_WIREFRAME_SOLID, zonetool::h1::TECHNIQUE_INSTANCED_WIREFRAME_SOLID},
				{zonetool::iw6::TECHNIQUE_INSTANCED_WIREFRAME_SHADED, zonetool::h1::TECHNIQUE_INSTANCED_WIREFRAME_SHADED},
				{zonetool::iw6::TECHNIQUE_INSTANCED_THERMAL, zonetool::h1::TECHNIQUE_INSTANCED_THERMAL},
				{zonetool::iw6::TECHNIQUE_INSTANCED_VELOCITY_RIGID, zonetool::h1::TECHNIQUE_INSTANCED_VELOCITY_RIGID},
				{zonetool::iw6::TECHNIQUE_INSTANCED_VELOCITY_SKINNED, zonetool::h1::TECHNIQUE_INSTANCED_VELOCITY_SKINNED},
				{zonetool::iw6::TECHNIQUE_INSTANCED_DEBUG_BUMPMAP, zonetool::h1::TECHNIQUE_INSTANCED_DEBUG_BUMPMAP},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_DEPTH_PREPASS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_ZPREPASS},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_DEPTH, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_BUILD_SHADOWMAP_DEPTH},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_UNLIT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_UNLIT},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_EMISSIVE, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_EMISSIVE},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_EMISSIVE_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_SHADOW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIT_SUN_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIGHT_OMNI, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIGHT_OMNI},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_LIGHT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_NORMAL, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_NORMAL},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_VIEW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_FAKELIGHT_VIEW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_SUNLIGHT_PREVIEW, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_SUNLIGHT_PREVIEW},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_CASE_TEXTURE, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_CASE_TEXTURE},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SOLID, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SOLID},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SHADED, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_WIREFRAME_SHADED},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_THERMAL, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_THERMAL},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_VELOCITY_RIGID, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_VELOCITY_RIGID},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_VELOCITY_SKINNED, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_VELOCITY_SKINNED},
				{zonetool::iw6::TECHNIQUE_SUBDIV_PATCH_DEBUG_BUMPMAP, zonetool::h1::TECHNIQUE_SUBDIV_PATCH_DEBUG_BUMPMAP},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_DEPTH_PREPASS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_ZPREPASS},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_DEPTH, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_BUILD_SHADOWMAP_DEPTH},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_UNLIT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_UNLIT},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_EMISSIVE_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_SHADOW},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_SHADOW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DIR_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_SPOT_SHADOW_CUCOLORIS_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SPOT_SHADOW_CUCOLORIS_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_OMNI_SHADOW_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_DFOG},
				//{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_OMNI_SHADOW_OMNI_SHADOW_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_DFOG, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIT_SUN_DYNAMIC_BRANCHING_DFOG},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIGHT_OMNI, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIGHT_OMNI},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW_CUCOLORIS, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_LIGHT_SPOT_SHADOW_CUCOLORIS},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_NORMAL, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_NORMAL},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_VIEW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_FAKELIGHT_VIEW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_SUNLIGHT_PREVIEW, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_SUNLIGHT_PREVIEW},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_CASE_TEXTURE, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_CASE_TEXTURE},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SOLID, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SOLID},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SHADED, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_WIREFRAME_SHADED},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_THERMAL, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_THERMAL},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_VELOCITY_RIGID, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_VELOCITY_RIGID},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_VELOCITY_SKINNED, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_VELOCITY_SKINNED},
				{zonetool::iw6::TECHNIQUE_NO_DISPLACEMENT_DEBUG_BUMPMAP, zonetool::h1::TECHNIQUE_NO_DISPLACEMENT_DEBUG_BUMPMAP},
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
					//ZONETOOL_ERROR("Unable to map worldVertFormat %d for technique '%s'!", asset->worldVertFormat, asset->name);
					new_asset->worldVertFormat = asset->worldVertFormat;
				}

				new_asset->preDisplacementOnlyCount = asset->preDisplacementOnlyCount;

				std::unordered_map<std::uintptr_t, zonetool::h1::MaterialTechnique*> converted_asset_techniques;
				for (std::int32_t i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
				{
					if (!technique_index_map.contains(i))
					{
						continue;
					}
					const auto new_tech_index = technique_index_map.at(i);

					if (asset->techniques[i])
					{
						if (converted_asset_techniques.contains(reinterpret_cast<std::uintptr_t>(asset->techniques[i])))
						{
							new_asset->techniques[new_tech_index] = converted_asset_techniques[reinterpret_cast<std::uintptr_t>(asset->techniques[i])];
						}
						else
						{
							const auto size = sizeof(MaterialTechniqueHeader) + sizeof(MaterialPass) * asset->techniques[i]->hdr.passCount;
							new_asset->techniques[new_tech_index] = mem->ManualAlloc<zonetool::h1::MaterialTechnique>(size);

							auto* technique = asset->techniques[i];
							auto* new_technique = new_asset->techniques[new_tech_index];

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

								new_pass->stableArgSize += 128;

								if (pass->args)
								{
									auto arg_count = pass->perObjArgCount + pass->perPrimArgCount + pass->stableArgCount;
									new_pass->args = mem->Alloc<zonetool::h1::MaterialShaderArgument>(arg_count);
									std::memcpy(new_pass->args, pass->args, sizeof(MaterialShaderArgument) * arg_count); // same struct

									auto new_arg_index = 0;
									for (auto arg_index = 0; arg_index < arg_count; arg_index++)
									{
										auto* arg = &pass->args[arg_index];
										auto* new_arg = &new_pass->args[new_arg_index];

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
												new_arg->u.codeConst.index = zonetool::h1::CONST_SRC_NONE;
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
												ZONETOOL_ERROR("Unable to map code sampler %d for technique '%s'!", arg->u.codeSampler, asset->name);
												new_arg->u.codeConst.index = zonetool::h1::TEXTURE_SRC_CODE_BLACK;
											}
										}

										new_arg_index++;
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