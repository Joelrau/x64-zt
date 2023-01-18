#include <std_include.hpp>
#include "material.hpp"

#include "techset.hpp"

#define MATERIAL_DUMP_STRING(entry) \
	matdata[#entry] = std::string(asset->entry);

#define MATERIAL_DUMP(entry) \
	matdata[#entry] = asset->entry;

#define MATERIAL_DUMP_INFO(entry) \
	matdata[#entry] = asset->info.entry;

#define MATERIAL_DUMP_CONST_ARRAY(entry,size) \
	ordered_json carr##entry; \
	for (int i = 0; i < size; i++) \
	{ \
		ordered_json cent##entry; \
		std::string name = asset->constantTable[i].name; \
		name.resize(12); \
		cent##entry["name"] = name.data(); \
		cent##entry["nameHash"] = asset->entry[i].nameHash; \
		nlohmann::json centliteral##entry; \
		centliteral##entry[0] = asset->entry[i].literal[0]; \
		centliteral##entry[1] = asset->entry[i].literal[1]; \
		centliteral##entry[2] = asset->entry[i].literal[2]; \
		centliteral##entry[3] = asset->entry[i].literal[3]; \
		cent##entry["literal"] = centliteral##entry; \
		carr##entry[i] = cent##entry; \
	} \
	matdata[#entry] = carr##entry;

namespace zonetool::h1
{
	namespace
	{
		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}
	}

	MaterialTextureDef* IMaterial::prase_texture_table(json& matdata, ZoneMemory* mem)
	{
		auto mat = mem->Alloc<MaterialTextureDef>(matdata.size());

		for (std::size_t i = 0; i < matdata.size(); i++)
		{
			mat[i].nameStart = matdata[i]["firstCharacter"].get<char>();
			mat[i].nameEnd = matdata[i]["lastCharacter"].get<char>();
			mat[i].samplerState = matdata[i]["samplerState"].get<unsigned char>();
			mat[i].semantic = matdata[i]["semantic"].get<unsigned char>();
			mat[i].nameHash = matdata[i]["typeHash"].get<unsigned int>();

			std::string img = matdata[i]["image"].get<std::string>();
			mat[i].u.image = DB_FindXAssetHeader(ASSET_TYPE_IMAGE, img.data(), 1).image;
		}

		return mat;
	}

	Material* IMaterial::parse(std::string name, ZoneMemory* mem)
	{
		auto c_name = clean_name(name);

		auto path = "materials\\"s + c_name + ".json"s;
		auto file = filesystem::file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing material \"%s\"...", name.data());

		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();

		json matdata = json::parse(bytes);

		auto mat = mem->Alloc<Material>();
		mat->name = mem->StrDup(name);

		mat->info.gameFlags = matdata["gameFlags"].get<unsigned char>();
		mat->info.sortKey = matdata["sortKey"].get<unsigned char>();
		mat->info.renderFlags = matdata["renderFlags"].get<unsigned char>();

		mat->info.textureAtlasRowCount = matdata["textureAtlasRowCount"].get<unsigned char>();
		mat->info.textureAtlasColumnCount = matdata["textureAtlasColumnCount"].get<unsigned char>();
		mat->info.textureAtlasFrameBlend = matdata["textureAtlasFrameBlend"].get<unsigned char>();
		mat->info.textureAtlasAsArray = matdata["textureAtlasAsArray"].get<unsigned char>();

		mat->info.surfaceTypeBits = matdata["surfaceTypeBits"].get<SurfaceTypeBits>();
		//mat->info.hashIndex = matdata["hashIndex"].get<unsigned int>();

		//mat->stateFlags = matdata["stateFlags"].get<unsigned char>();
		mat->cameraRegion = matdata["cameraRegion"].get<unsigned char>();
		mat->materialType = matdata["materialType"].get<unsigned char>();
		mat->assetFlags = matdata["assetFlags"].get<unsigned char>();

		if (!matdata["drawSurf"].is_null())
		{
			mat->info.drawSurf.fields.objectId = matdata["drawSurf"]["objectId"].get<int>();
			mat->info.drawSurf.fields.reflectionProbeIndex = matdata["drawSurf"]["reflectionProbeIndex"].get<int>();
			mat->info.drawSurf.fields.hasGfxEntIndex = matdata["drawSurf"]["hasGfxEntIndex"].get<int>();
			mat->info.drawSurf.fields.customIndex = matdata["drawSurf"]["customIndex"].get<int>();
			mat->info.drawSurf.fields.materialSortedIndex = matdata["drawSurf"]["materialSortedIndex"].get<int>();
			mat->info.drawSurf.fields.tessellation = matdata["drawSurf"]["tessellation"].get<int>();
			mat->info.drawSurf.fields.prepass = matdata["drawSurf"]["prepass"].get<int>();
			mat->info.drawSurf.fields.useHeroLighting = matdata["drawSurf"]["useHeroLighting"].get<int>();
			mat->info.drawSurf.fields.sceneLightEnvIndex = matdata["drawSurf"]["sceneLightEnvIndex"].get<int>();
			mat->info.drawSurf.fields.viewModelRender = matdata["drawSurf"]["viewModelRender"].get<int>();
			mat->info.drawSurf.fields.surfType = matdata["drawSurf"]["surfType"].get<int>();
			mat->info.drawSurf.fields.primarySortKey = matdata["drawSurf"]["primarySortKey"].get<int>();
		}

		std::string techset = matdata["techniqueSet->name"];
		if (!techset.empty())
		{
			mat->techniqueSet = DB_FindXAssetHeader(ASSET_TYPE_TECHNIQUE_SET, techset.data(), 1).techniqueSet;
		}

		json textureTable = matdata["textureTable"];
		if (textureTable.size())
		{
			mat->textureTable = prase_texture_table(textureTable, mem);
		}
		mat->textureCount = static_cast<unsigned char>(textureTable.size());

		json constantTable = matdata["constantTable"];
		if (constantTable.size() > 0)
		{
			auto constant_def = mem->Alloc<MaterialConstantDef>(constantTable.size());
			for (int i = 0; i < constantTable.size(); i++)
			{
				strcat(constant_def[i].name, constantTable[i]["name"].get<std::string>().data());
				constant_def[i].nameHash = constantTable[i]["nameHash"].get<unsigned int>();
				constant_def[i].literal[0] = constantTable[i]["literal"][0].get<float>();
				constant_def[i].literal[1] = constantTable[i]["literal"][1].get<float>();
				constant_def[i].literal[2] = constantTable[i]["literal"][2].get<float>();
				constant_def[i].literal[3] = constantTable[i]["literal"][3].get<float>();
			}
			mat->constantTable = constant_def;
		}
		else
		{
			mat->constantTable = nullptr;
		}
		mat->constantCount = static_cast<unsigned char>(constantTable.size());

		if (mat->techniqueSet)
		{
			ITechset::parse_stateinfo(mat->techniqueSet->name, mat, mem);
			ITechset::parse_statebits(mat->techniqueSet->name, mat->stateBitsEntry, mem);
			ITechset::parse_statebitsmap(mat->techniqueSet->name, &mat->stateBitsTable, &mat->stateBitsCount,
				&this->depth_stenchil_state_bits,
				&this->blend_state_bits,
				mem);

			if (mat->constantCount)
			{
				ITechset::parse_constant_buffer_indexes(mat->techniqueSet->name, mat->constantBufferIndex, mem);
				ITechset::parse_constant_buffer_def_array(mat->techniqueSet->name, &mat->constantBufferTable, &mat->constantBufferCount, mem);
			}
		}

		auto max_state_index = 0;
		for (auto i = 0; i < TECHNIQUES_COUNT; i++)
		{
			if (mat->stateBitsEntry[i] == 255)
			{
				continue;
			}

			if (mat->stateBitsEntry[i] > max_state_index)
			{
				max_state_index = mat->stateBitsEntry[i];
			}
		}

		if (max_state_index >= mat->stateBitsCount)
		{
			ZONETOOL_FATAL("Material %s is referencing more statebit entries than it has!", mat->name);
		}

		if (max_state_index < mat->stateBitsCount - 1)
		{
			ZONETOOL_INFO("Material %s has %u statebits but only %u are used, removing unused statebits.", mat->name, mat->stateBitsCount, max_state_index + 1);
			mat->stateBitsCount = static_cast<unsigned char>(max_state_index + 1);
		}

		// fix sortKeys
		/*bool fixed = false;
		DB_EnumXAssets(ASSET_TYPE_MATERIAL, [mat, &fixed](XAssetHeader header)
		{
			if (fixed) return;

			const char* name = mat->techniqueSet->name;
			if (name[0] == ',') ++name;

			if (std::string(name) == header.material->techniqueSet->name)
			{
				mat->info.sortKey = header.material->info.sortKey;
				fixed = true;
			}
		}, false);*/

		return mat;
	}

	void IMaterial::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).material;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name_.data()))
			{
				ZONETOOL_WARNING("Missing material \"%s\", using default...", this->name_.data());
			}

			auto material = this->asset_;
			for (auto i = 0; i < material->stateBitsCount; i++)
			{
				XGfxGlobals* varXGfxGlobals = GetXGfxGlobalsForZone(material->stateBitsTable[i].zone);

				std::array<std::uint64_t, 10> temp_bits;
				for (auto j = 0; j < 10; j++)
				{
					temp_bits[j] = varXGfxGlobals->depthStencilStateBits[material->stateBitsTable[i].depthStencilState[j]];
				}
				this->depth_stenchil_state_bits.push_back(temp_bits);

				std::array<std::uint32_t, 3> temp_bits2;
				for (auto j = 0; j < 3; j++)
				{
					temp_bits2[j] = varXGfxGlobals->blendStateBits[material->stateBitsTable[i].blendState][j];
				}
				this->blend_state_bits.push_back(temp_bits2);
			}
		}
	}

	void IMaterial::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
		auto material = this->asset_;

		for (auto i = 0; i < material->stateBitsCount; i++)
		{
			for (auto j = 0; j < 10; j++)
			{
				material->stateBitsTable[i].depthStencilState[j] = buf->write_depthstencilstatebit(this->depth_stenchil_state_bits[i][j]);
			}
			material->stateBitsTable[i].blendState = buf->write_blendstatebits(this->blend_state_bits[i]);
		}
	}

	void IMaterial::load_depending(IZone* zone)
	{
		auto data = this->asset_;

		if (data->techniqueSet)
		{
			zone->add_asset_of_type(ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet->name);
		}

		for (auto i = 0; i < data->textureCount; i++)
		{
			if (data->textureTable[i].u.image)
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->textureTable[i].u.image->name);
				auto img = zone->find_asset(ASSET_TYPE_IMAGE, data->textureTable[i].u.image->name);
				if (img)
				{
					reinterpret_cast<GfxImage*>(img->pointer())->semantic = data->textureTable[i].semantic;
				}
			}
		}
	}

	std::string IMaterial::name()
	{
		return this->name_;
	}

	std::int32_t IMaterial::type()
	{
		return ASSET_TYPE_MATERIAL;
	}

	void IMaterial::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->techniqueSet)
		{
			dest->techniqueSet = reinterpret_cast<MaterialTechniqueSet*>(zone->get_asset_pointer(
				ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet->name));
		}

		if (data->textureTable)
		{
			buf->inc_stream(5, 2 * (2 * data->textureCount));

			buf->align(3);
			auto destmaps = buf->write(data->textureTable, data->textureCount);

			for (auto i = 0; i < data->textureCount; i++)
			{
				if (data->textureTable[i].semantic == 11)
				{
					ZONETOOL_ERROR("Watermaps are not supported.");
					destmaps[i].u.water = nullptr;
				}
				else
				{
					if (data->textureTable[i].u.image)
					{
						destmaps[i].u.image = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
							ASSET_TYPE_IMAGE, data->textureTable[i].u.image->name));
					}
				}
			}

			ZoneBuffer::clear_pointer(&dest->textureTable);
		}

		if (data->constantTable)
		{
			/*buf->align(15);
			dest->constantTable = buf->write(data->constantTable, data->constantCount);
			ZoneBuffer::clear_pointer(&dest->constantTable);*/
			dest->constantTable = buf->write_s(15, data->constantTable, data->constantCount);
		}

		if (data->stateBitsTable)
		{
			/*buf->align(3);
			dest->stateMap = buf->write(data->stateMap, data->stateBitsCount);
			ZoneBuffer::clear_pointer(&dest->stateMap);*/
			dest->stateBitsTable = buf->write_s(3, data->stateBitsTable, data->stateBitsCount);
		}

		if (data->constantBufferTable)
		{
			buf->align(3);
			auto destcbt = buf->write(data->constantBufferTable, data->constantBufferCount);

			for (int i = 0; i < data->constantBufferCount; i++)
			{
				auto cbt = &data->constantBufferTable[i];

				if (cbt->vsData)
				{
					destcbt[i].vsData = buf->write_s(15, cbt->vsData, cbt->vsDataSize);
				}
				if (cbt->hsData)
				{
					destcbt[i].hsData = buf->write_s(15, cbt->hsData, cbt->hsDataSize);
				}
				if (cbt->dsData)
				{
					destcbt[i].dsData = buf->write_s(15, cbt->dsData, cbt->dsDataSize);
				}
				if (cbt->psData)
				{
					destcbt[i].psData = buf->write_s(15, cbt->psData, cbt->psDataSize);
				}
				if (cbt->vsOffsetData)
				{
					destcbt[i].vsOffsetData = buf->write_s(1, cbt->vsOffsetData, cbt->vsOffsetDataSize);
				}
				if (cbt->hsOffsetData)
				{
					destcbt[i].hsOffsetData = buf->write_s(1, cbt->hsOffsetData, cbt->hsOffsetDataSize);
				}
				if (cbt->dsOffsetData)
				{
					destcbt[i].dsOffsetData = buf->write_s(1, cbt->dsOffsetData, cbt->dsOffsetDataSize);
				}
				if (cbt->psOffsetData)
				{
					destcbt[i].psOffsetData = buf->write_s(1, cbt->psOffsetData, cbt->psOffsetDataSize);
				}
			}

			ZoneBuffer::clear_pointer(&dest->constantBufferTable);
		}

		if (data->subMaterials)
		{
			buf->align(7);
			dest->subMaterials = buf->write(data->subMaterials, data->layerCount);
			for (int i = 0; i < data->layerCount; i++)
			{
				dest->subMaterials[i] = buf->write_str(data->subMaterials[i]);
			}
			ZoneBuffer::clear_pointer(&dest->subMaterials);
		}

		buf->pop_stream();
	}

	void IMaterial::dump(Material* asset)
	{
		// TODO: maybe add subMaterials?

		if (asset)
		{
			auto c_name = clean_name(asset->name);

			const auto path = "materials\\"s + c_name + ".json"s;
			auto file = filesystem::file(path);
			file.open("wb");

			if (asset && asset->techniqueSet)
			{
				ITechset::dump_stateinfo(asset->techniqueSet->name, asset);
				ITechset::dump_statebits(asset->techniqueSet->name, asset->stateBitsEntry);
				ITechset::dump_statebits_map(asset->techniqueSet->name, asset->stateBitsTable, asset->stateBitsCount);

				ITechset::dump_constant_buffer_indexes(asset->techniqueSet->name, asset->constantBufferIndex);
				ITechset::dump_constant_buffer_def_array(asset->techniqueSet->name, asset->constantBufferCount, asset->constantBufferTable);
			}

			ordered_json matdata;

			MATERIAL_DUMP_STRING(name);

			if (asset->techniqueSet)
			{
				MATERIAL_DUMP_STRING(techniqueSet->name);
			}

			MATERIAL_DUMP_INFO(gameFlags);
			MATERIAL_DUMP_INFO(sortKey);
			MATERIAL_DUMP_INFO(renderFlags);

			MATERIAL_DUMP_INFO(textureAtlasRowCount);
			MATERIAL_DUMP_INFO(textureAtlasColumnCount);
			MATERIAL_DUMP_INFO(textureAtlasFrameBlend);
			MATERIAL_DUMP_INFO(textureAtlasAsArray);

			MATERIAL_DUMP_INFO(surfaceTypeBits);
			//MATERIAL_DUMP_INFO(hashIndex);

			//MATERIAL_DUMP(stateFlags);
			MATERIAL_DUMP(cameraRegion);
			MATERIAL_DUMP(materialType);
			MATERIAL_DUMP(assetFlags);

			MATERIAL_DUMP_CONST_ARRAY(constantTable, asset->constantCount);

			ordered_json material_images;
			for (auto i = 0; i < asset->textureCount; i++)
			{
				ordered_json image;
				if (asset->textureTable[i].semantic == 11)
				{
					// Haven't tested water yet.
					MessageBoxA(nullptr, asset->textureTable[i].u.water->image->name, "water", 0);

					water_t* waterData = asset->textureTable[i].u.water;

					image["image"] = waterData->image->name;

					ordered_json waterdata;
					waterdata["floatTime"] = waterData->writable.floatTime;
					waterdata["M"] = waterData->M;
					waterdata["N"] = waterData->N;
					waterdata["Lx"] = waterData->Lx;
					waterdata["Lz"] = waterData->Lz;
					waterdata["gravity"] = waterData->gravity;
					waterdata["windvel"] = waterData->windvel;
					waterdata["winddir"][0] = waterData->winddir[0];
					waterdata["winddir"][1] = waterData->winddir[1];
					waterdata["amplitude"] = waterData->amplitude;

					ordered_json waterComplexData;
					ordered_json wTerm;

					for (int a = 0; a < waterData->M * waterData->N; a++)
					{
						ordered_json complexdata;
						ordered_json curWTerm;

						complexdata["real"] = waterData->H0X[a];
						complexdata["imag"] = waterData->H0Y[a];

						curWTerm[a] = waterData->wTerm[a];

						waterComplexData[a] = complexdata;
					}

					waterdata["complex"] = waterComplexData;
					waterdata["wTerm"] = wTerm;

					image["waterinfo"] = waterdata;
				}
				else
				{
					if (asset->textureTable[i].u.image && asset->textureTable[i].u.image->name)
					{
						image["image"] = asset->textureTable[i].u.image->name;
					}
					else
					{
						image["image"] = "";
					}
				}

				image["semantic"] = asset->textureTable[i].semantic;
				image["samplerState"] = asset->textureTable[i].samplerState;
				image["lastCharacter"] = asset->textureTable[i].nameEnd;
				image["firstCharacter"] = asset->textureTable[i].nameStart;
				image["typeHash"] = asset->textureTable[i].nameHash;

				// add image data to material
				material_images.push_back(image);
			}
			matdata["textureTable"] = material_images;

			auto str = matdata.dump(4);

			matdata.clear();

			file.write(str);

			file.close();
		}
	}
}