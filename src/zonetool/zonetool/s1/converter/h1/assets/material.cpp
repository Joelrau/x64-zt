#include <std_include.hpp>
#include "zonetool/s1/converter/h1/include.hpp"
#include "material.hpp"

//#include "zonetool/h1/assets/material.hpp"

#include "techset.hpp"

#include <utils/string.hpp>

namespace zonetool::s1
{
	namespace converter::h1
	{
		namespace material
		{
			namespace ree
			{
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

				void dump(Material* s1_asset, ZoneMemory* mem)
				{
					auto* asset = converter::h1::material::convert(s1_asset, mem);

					// TODO: maybe add subMaterials?

					if (asset)
					{
						auto c_name = clean_name(asset->name);

						const auto path = "materials\\"s + c_name + ".json"s;
						auto file = filesystem::file(path);
						file.open("wb");

						if (asset && asset->techniqueSet)
						{
							converter::h1::techset::ree::dump_stateinfo(asset->techniqueSet->name, asset);
							converter::h1::techset::ree::dump_statebits(asset->techniqueSet->name, asset->stateBitsEntry);
							converter::h1::techset::ree::dump_statebits_map(asset->techniqueSet->name, asset->stateBitsTable, asset->stateBitsCount);

							converter::h1::techset::ree::dump_constant_buffer_indexes(asset->techniqueSet->name, asset->constantBufferIndex);
							converter::h1::techset::ree::dump_constant_buffer_def_array(asset->techniqueSet->name, asset->constantBufferCount, asset->constantBufferTable);
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
								// unsupported
								MessageBoxA(nullptr, asset->textureTable[i].u.water->image->name, "water", 0);
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

			zonetool::h1::Material* convert(Material* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::Material>();

				REINTERPRET_CAST_SAFE(name);

				new_asset->info.gameFlags = asset->info.gameFlags; // convert?
				new_asset->info.sortKey = asset->info.sortKey; // convert?
				new_asset->info.textureAtlasRowCount = asset->info.textureAtlasRowCount;
				new_asset->info.textureAtlasColumnCount = asset->info.textureAtlasColumnCount;
				new_asset->info.textureAtlasFrameBlend = asset->info.textureAtlasFrameBlend;
				new_asset->info.textureAtlasAsArray = asset->info.textureAtlasAsArray;
				new_asset->info.renderFlags = asset->info.renderFlags; // convert?
				COPY_VALUE_CAST(info.drawSurf); // todo?
				COPY_VALUE_CAST(info.surfaceTypeBits); // todo?
				new_asset->info.hashIndex = asset->info.hashIndex;

				std::memset(new_asset->stateBitsEntry, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->stateBitsEntry, asset->stateBitsEntry, MaterialTechniqueType::TECHNIQUE_COUNT);

				new_asset->textureCount = asset->textureCount;
				new_asset->constantCount = asset->constantCount;
				new_asset->stateBitsCount = asset->stateBitsCount;
				new_asset->stateFlags = asset->stateFlags; // convert?
				new_asset->cameraRegion = asset->cameraRegion; // convert?
				new_asset->materialType = asset->materialType; // convert?
				new_asset->layerCount = asset->layerCount;
				new_asset->assetFlags = asset->assetFlags; // convert?

				new_asset->techniqueSet = converter::h1::techset::convert(asset->techniqueSet, mem);
				REINTERPRET_CAST_SAFE(textureTable);
				REINTERPRET_CAST_SAFE(constantTable);
				REINTERPRET_CAST_SAFE(stateBitsTable);

				std::memset(new_asset->constantBufferIndex, 0xFF, zonetool::h1::MaterialTechniqueType::TECHNIQUE_COUNT);
				std::memcpy(new_asset->constantBufferIndex, asset->constantBufferIndex, MaterialTechniqueType::TECHNIQUE_COUNT);

				REINTERPRET_CAST_SAFE(constantBufferTable);
				new_asset->constantBufferCount = asset->constantBufferCount;

				REINTERPRET_CAST_SAFE(subMaterials);

				return new_asset;
			}

			void dump(Material* asset, ZoneMemory* mem)
			{
				ree::dump(asset, mem);
			}
		}
	}
}