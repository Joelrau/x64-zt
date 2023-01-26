#include <std_include.hpp>
#include "techset.hpp"

#include "zonetool/s1/zonetool.hpp"

namespace zonetool::s1
{
	namespace converter::h1::techset
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
				for (int j = 0; j < 11; j++)
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
			auto* asset = converter::h1::convert_techset(s1_asset, mem);

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
}