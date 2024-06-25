#include <std_include.hpp>
#include "material.hpp"

#include "techset.hpp"

#include "zonetool/utils/iwi.hpp"

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

		void copy_constant_table_to_cbt(Material* mat)
		{
			for (auto i = 0u; i < mat->constantBufferCount; i++)
			{
				const auto cbt = &mat->constantBufferTable[i];
				for (auto o = 0u; o < mat->constantCount; o++)
				{
#define COPY_CONSTANT_TABLE_VALUES(__data__, __offset_data__) \
						if (cbt->__offset_data__ && cbt->__offset_data__[o] != 0xFFFF) \
						{ \
							const auto constant = reinterpret_cast<float*>(&cbt->__data__[cbt->__offset_data__[o]]); \
							for (auto j = 0; j < 4; j++) \
							{ \
								constant[j] = mat->constantTable[o].literal[j]; \
							} \
						} \

					COPY_CONSTANT_TABLE_VALUES(vsData, vsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(hsData, hsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(dsData, dsOffsetData);
					COPY_CONSTANT_TABLE_VALUES(psData, psOffsetData);
				}
			}
		}
	}

	std::unordered_map<GfxImage*, std::string> material::fixed_nml_images_map;

	MaterialTextureDef* material::parse_texture_table(json& matdata, zone_memory* mem)
	{
		auto mat = mem->allocate<MaterialTextureDef>(matdata.size());

		for (std::size_t i = 0; i < matdata.size(); i++)
		{
			mat[i].nameStart = matdata[i]["firstCharacter"].get<char>();
			mat[i].nameEnd = matdata[i]["lastCharacter"].get<char>();
			mat[i].samplerState = matdata[i]["samplerState"].get<unsigned char>();
			mat[i].semantic = matdata[i]["semantic"].get<unsigned char>();
			mat[i].nameHash = matdata[i]["typeHash"].get<unsigned int>();

			std::string img = matdata[i]["image"].get<std::string>();
			mat[i].u.image = db_find_x_asset_header(ASSET_TYPE_IMAGE, img.data(), 1).image;
		}

		return mat;
	}

	Material* material::parse(std::string name, zone_memory* mem)
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

		auto mat = mem->allocate<Material>();
		mat->name = mem->duplicate_string(name);

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
			mat->techniqueSet = db_find_x_asset_header(ASSET_TYPE_TECHNIQUE_SET, techset.data(), 1).techniqueSet;
		}

		json textureTable = matdata["textureTable"];
		if (textureTable.size())
		{
			mat->textureTable = parse_texture_table(textureTable, mem);
		}
		mat->textureCount = static_cast<unsigned char>(textureTable.size());

		json constantTable = matdata["constantTable"];
		if (constantTable.size() > 0)
		{
			auto constant_def = mem->allocate<MaterialConstantDef>(constantTable.size());
			for (int i = 0; i < constantTable.size(); i++)
			{
				strcat(constant_def[i].name, constantTable[i]["name"].get<std::string>().data());
				constant_def[i].nameHash = constantTable[i]["nameHash"].get<unsigned int>();
				constant_def[i].literal[0] = constantTable[i]["literal"][0].is_null() ? std::numeric_limits<float>::quiet_NaN() : constantTable[i]["literal"][0].get<float>();
				constant_def[i].literal[1] = constantTable[i]["literal"][1].is_null() ? std::numeric_limits<float>::quiet_NaN() : constantTable[i]["literal"][1].get<float>();
				constant_def[i].literal[2] = constantTable[i]["literal"][2].is_null() ? std::numeric_limits<float>::quiet_NaN() : constantTable[i]["literal"][2].get<float>();
				constant_def[i].literal[3] = constantTable[i]["literal"][3].is_null() ? std::numeric_limits<float>::quiet_NaN() : constantTable[i]["literal"][3].get<float>();
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
			techset::parse_stateinfo(mat->techniqueSet->name, c_name.data(), mat, mem);
			techset::parse_statebits(mat->techniqueSet->name, c_name.data(), mat->stateBitsEntry, mem);
			techset::parse_statebitsmap(mat->techniqueSet->name, c_name.data(), &mat->stateBitsTable, &mat->stateBitsCount,
				&this->depth_stenchil_state_bits,
				&this->blend_state_bits,
				mem);

			if (mat->constantCount)
			{
				techset::parse_constant_buffer_indexes(mat->techniqueSet->name, c_name.data(), mat->constantBufferIndex, mem);
				techset::parse_constant_buffer_def_array(mat->techniqueSet->name, c_name.data(), &mat->constantBufferTable, &mat->constantBufferCount, mem);
				copy_constant_table_to_cbt(mat);
			}
		}

		auto max_state_index = 0;
		for (auto i = 0; i < MaterialTechniqueType::TECHNIQUE_COUNT; i++)
		{
			if (mat->stateBitsEntry[i] == 0xFF)
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

		return mat;
	}

	void material::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).material;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name_.data()))
			{
				ZONETOOL_WARNING("Missing material \"%s\", using default...", this->name_.data());
			}

			auto material = this->asset_;
			for (auto i = 0; i < material->stateBitsCount; i++)
			{
				const auto var_x_gfx_globals = get_x_gfx_globals_for_zone<XGfxGlobals>(material->stateBitsTable[i].zone);

				std::array<std::uint64_t, 10> temp_bits;
				for (auto j = 0; j < 10; j++)
				{
					temp_bits[j] = var_x_gfx_globals->depthStencilStateBits[material->stateBitsTable[i].depthStencilState[j]];
				}
				this->depth_stenchil_state_bits.push_back(temp_bits);

				std::array<std::uint32_t, 3> temp_bits2;
				for (auto j = 0; j < 3; j++)
				{
					temp_bits2[j] = var_x_gfx_globals->blendStateBits[material->stateBitsTable[i].blendState][j];
				}
				this->blend_state_bits.push_back(temp_bits2);
			}
		}
	}

	void material::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto material = this->asset_;

		for (auto i = 0; i < material->stateBitsCount; i++)
		{
			for (auto j = 0; j < 10; j++)
			{
				material->stateBitsTable[i].depthStencilState[j] = buf->write_depthstencilstatebit(this->depth_stenchil_state_bits[i][j]);
			}

			std::array<std::uint32_t, 4> temp_bits{ this->blend_state_bits[i][0], this->blend_state_bits[i][1], this->blend_state_bits[i][2], 0 };
			material->stateBitsTable[i].blendState = buf->write_blendstatebits(temp_bits);
		}
	}

	void material::load_depending(zone_base* zone)
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
				auto img = static_cast<gfx_image*>(zone->find_asset(ASSET_TYPE_IMAGE, data->textureTable[i].u.image->name));
				if (img)
				{
					auto* image = reinterpret_cast<GfxImage*>(img->pointer());
					image->semantic = data->textureTable[i].semantic;

					if (image->semantic == TS_NORMAL_MAP && img->is_iwi && !material::fixed_nml_images_map.contains(image))
					{
						iwi::parse::GfxImage img_{};
						img_.name = image->name;
						img_.imageFormat = image->imageFormat;
						img_.mapType = static_cast<iwi::parse::MapType>(image->mapType);
						img_.dataLen = image->dataLen1;
						img_.width = image->width;
						img_.height = image->height;
						img_.depth = image->depth;
						img_.numElements = image->numElements;
						img_.levelCount = image->levelCount;
						img_.pixelData = image->pixelData;

						if (iwi::fixup_normal_map(&img_))
						{
							material::fixed_nml_images_map[image] = image->name;

							image->name = img_.name;
							image->imageFormat = img_.imageFormat;
							image->mapType = static_cast<MapType>(img_.mapType);
							image->dataLen1 = img_.dataLen;
							image->dataLen2 = img_.dataLen;
							image->width = img_.width;
							image->height = img_.height;
							image->depth = img_.depth;
							image->numElements = img_.numElements;
							image->levelCount = img_.levelCount;
							image->pixelData = img_.pixelData;

							image->flags |= img_.levelCount > 1 ? 0 : IMAGE_FLAG_NOMIPMAPS;
						}
					}
				}
			}
		}
	}

	std::string material::name()
	{
		return this->name_;
	}

	std::int32_t material::type()
	{
		return ASSET_TYPE_MATERIAL;
	}

	void material::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

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

			buf->clear_pointer(&dest->textureTable);
		}

		if (data->constantTable)
		{
			/*buf->align(15);
			dest->constantTable = buf->write(data->constantTable, data->constantCount);
			buf->clear_pointer(&dest->constantTable);*/
			dest->constantTable = buf->write_s(15, data->constantTable, data->constantCount);
		}

		if (data->stateBitsTable)
		{
			/*buf->align(3);
			dest->stateMap = buf->write(data->stateMap, data->stateBitsCount);
			buf->clear_pointer(&dest->stateMap);*/
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

			buf->clear_pointer(&dest->constantBufferTable);
		}

		if (data->subMaterials)
		{
			buf->align(7);
			dest->subMaterials = buf->write(data->subMaterials, data->layerCount);
			for (int i = 0; i < data->layerCount; i++)
			{
				dest->subMaterials[i] = buf->write_str(data->subMaterials[i]);
			}
			buf->clear_pointer(&dest->subMaterials);
		}

		buf->pop_stream();
	}

	void material::dump(Material* asset)
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
				techset::dump_stateinfo(asset->techniqueSet->name, c_name.data(), asset);
				techset::dump_statebits(asset->techniqueSet->name, c_name.data(), asset->stateBitsEntry);
				techset::dump_statebits_map(asset->techniqueSet->name, c_name.data(), asset->stateBitsTable, asset->stateBitsCount);

				techset::dump_constant_buffer_indexes(asset->techniqueSet->name, c_name.data(), asset->constantBufferIndex);
				techset::dump_constant_buffer_def_array(asset->techniqueSet->name, c_name.data(), asset->constantBufferCount, asset->constantBufferTable);
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
					ZONETOOL_ERROR("Water texture found in material %s", asset->name);
					image["image"] = "";
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