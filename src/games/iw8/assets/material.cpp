#include <std_include.hpp>
#include "material.hpp"

namespace zonetool::iw8
{
	namespace
	{
		std::size_t argument_count(const MaterialTechnique* technique)
		{
			return technique->perPrimArgCount + technique->perObjArgCount + technique->stableArgCount + technique->neverArgCount;
		}

		void read_technique(zone_reader& reader, MaterialTechnique* technique)
		{
			reader.read_string(technique->name);
			reader.read_array(technique->pipelineStates, 7, technique->pipelineStateInputLayoutCount * technique->renderTargetFormatCount);
			reader.read_array(technique->stateBitsTable, 0, 1);
			reader.read_asset(ASSET_TYPE_VERTEXSHADER, technique->vertexShader);
			reader.read_asset(ASSET_TYPE_HULLSHADER, technique->hullShader);
			reader.read_asset(ASSET_TYPE_DOMAINSHADER, technique->domainShader);
			reader.read_asset(ASSET_TYPE_PIXELSHADER, technique->pixelShader);
			reader.read_array(technique->renderTargetFormats, 0, technique->renderTargetFormatCount);
			reader.read_array(technique->args, 1, argument_count(technique));
		}

		void read_constant_buffers(zone_reader& reader, Material* asset)
		{
			if (const auto buffers = reader.read_array(asset->constantBufferTable, 15, asset->constantBufferCount))
			{
				for (auto i = 0; i < asset->constantBufferCount; i++)
				{
					auto& buffer = buffers[i];
					reader.read_array(buffer.vsData, 15, buffer.vsDataSize);
					reader.read_array(buffer.hsData, 15, buffer.hsDataSize);
					reader.read_array(buffer.dsData, 15, buffer.dsDataSize);
					reader.read_array(buffer.psData, 15, buffer.psDataSize);
				}
			}
		}
	}

	std::size_t get_technique_count(const MaterialTechniqueSet* asset)
	{
		std::size_t count = 0;
		for (const auto mask : asset->techniqueMask.mask)
		{
			count += std::popcount(mask);
		}

		return count;
	}

	std::optional<std::uint32_t> get_technique_type(const MaterialTechniqueSet* asset, const std::size_t masked_index)
	{
		std::size_t index = 0;
		for (auto type = 0u; type < TECHNIQUE_COUNT; type++)
		{
			if (!(asset->techniqueMask.mask[type / 64] & (1ull << (type % 64))))
			{
				continue;
			}

			if (index++ == masked_index)
			{
				return type;
			}
		}

		return {};
	}

	void material::read(zone_reader& reader, Material* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->packedAtlasData, 0, asset->packedAtlasDataSize);
		reader.read_asset(ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet);

		if (const auto textures = reader.read_array(asset->textureTable, 7, asset->textureCount))
		{
			for (auto i = 0; i < asset->textureCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, textures[i].image);
			}
		}

		reader.read_array(asset->constantTable, 15, asset->constantCount);
		reader.read_asset(ASSET_TYPE_DECAL_VOLUME_MATERIAL, asset->decalVolumeMaterial);
		reader.read_array(asset->constantBufferIndex, 0, TECHNIQUE_COUNT);
		read_constant_buffers(reader, asset);

		if (const auto sub_materials = reader.read_array(asset->subMaterials, 7, asset->layerCount))
		{
			for (auto i = 0; i < asset->layerCount; i++)
			{
				reader.read_string(sub_materials[i]);
			}
		}
		reader.pop_stream();
	}

	void technique_set::read(zone_reader& reader, MaterialTechniqueSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto count = get_technique_count(asset);
		if (const auto techniques = reader.read_array(asset->maskedTechniques, 7, count))
		{
			for (std::size_t i = 0; i < count; i++)
			{
				if (const auto technique = reader.read_single(techniques[i], 7))
				{
					read_technique(reader, technique);
				}
			}
		}
		reader.pop_stream();
	}

	namespace
	{
		std::string to_hex(const void* data, const std::size_t size)
		{
			std::string result;
			for (auto i = 0u; i < size; i++)
			{
				result += std::format("{:02X}", static_cast<const unsigned char*>(data)[i]);
			}

			return result;
		}

		template <typename T>
		const char* shader_name(const T* shader)
		{
			return shader ? shader->name : "";
		}
	}

	void material::dump(Material* asset)
	{
		ordered_json data;
		data["techniqueSet"] = asset->techniqueSet ? asset->techniqueSet->name : "";
		data["contents"] = asset->contents;
		data["surfaceFlags"] = asset->surfaceFlags;
		data["maxDisplacement"] = asset->maxDisplacement;
		data["materialType"] = asset->materialType;
		data["cameraRegion"] = asset->cameraRegion;
		data["sortKey"] = asset->sortKey;
		data["flags"] = asset->flags;
		data["textureAtlasRowCount"] = asset->textureAtlasRowCount;
		data["textureAtlasColumnCount"] = asset->textureAtlasColumnCount;
		data["drawSurf"] = to_hex(&asset->drawSurf, sizeof(asset->drawSurf));
		data["packedAtlasData"] = asset->packedAtlasData ? to_hex(asset->packedAtlasData, asset->packedAtlasDataSize) : "";

		data["textureTable"] = ordered_json::array();
		for (auto i = 0; i < asset->textureCount; i++)
		{
			const auto& texture = asset->textureTable[i];
			data["textureTable"].push_back({{"index", texture.index}, {"image", texture.image ? texture.image->name : ""}});
		}

		data["constantTable"] = ordered_json::array();
		for (auto i = 0; i < asset->constantCount; i++)
		{
			const auto& constant = asset->constantTable[i];
			data["constantTable"].push_back({{"index", constant.index}, {"literal", constant.literal}});
		}

		data["constantBufferIndex"] = ordered_json::array();
		for (auto i = 0; asset->constantBufferIndex && i < TECHNIQUE_COUNT; i++)
		{
			data["constantBufferIndex"].push_back(asset->constantBufferIndex[i]);
		}

		data["constantBufferTable"] = ordered_json::array();
		for (auto i = 0; i < asset->constantBufferCount; i++)
		{
			const auto& buffer = asset->constantBufferTable[i];
			data["constantBufferTable"].push_back(
			{
				{"vsData", buffer.vsData ? to_hex(buffer.vsData, buffer.vsDataSize) : ""},
				{"hsData", buffer.hsData ? to_hex(buffer.hsData, buffer.hsDataSize) : ""},
				{"dsData", buffer.dsData ? to_hex(buffer.dsData, buffer.dsDataSize) : ""},
				{"psData", buffer.psData ? to_hex(buffer.psData, buffer.psDataSize) : ""},
			});
		}

		data["subMaterials"] = ordered_json::array();
		for (auto i = 0; asset->subMaterials && i < asset->layerCount; i++)
		{
			data["subMaterials"].push_back(asset->subMaterials[i] ? asset->subMaterials[i] : "");
		}

		filesystem::file file("materials\\"s + asset->name + ".json");
		file.open("wb");
		file.write(data.dump(4));
		file.close();
	}

	void technique_set::dump(MaterialTechniqueSet* asset)
	{
		ordered_json data;
		data["flags"] = asset->flags;
		data["materialType"] = asset->materialType;
		data["techniques"] = ordered_json::array();

		for (auto i = 0u; i < get_technique_count(asset); i++)
		{
			const auto technique = asset->maskedTechniques[i];
			if (!technique)
			{
				continue;
			}

			ordered_json entry;
			entry["type"] = get_technique_type(asset, i).value_or(TECHNIQUE_COUNT);
			entry["name"] = technique->name ? technique->name : "";
			entry["flags"] = technique->flags;
			entry["vertexShader"] = shader_name(technique->vertexShader);
			entry["hullShader"] = shader_name(technique->hullShader);
			entry["domainShader"] = shader_name(technique->domainShader);
			entry["pixelShader"] = shader_name(technique->pixelShader);
			entry["stateBits"] = to_hex(&technique->stateBits, sizeof(technique->stateBits));
			entry["argCounts"] = {technique->perPrimArgCount, technique->perObjArgCount, technique->stableArgCount, technique->neverArgCount};
			entry["argSizes"] = {technique->perPrimArgSize, technique->perObjArgSize, technique->stableArgSize};
			entry["customBufferFlags"] = technique->customBufferFlags;
			entry["customSamplerFlags"] = technique->customSamplerFlags;

			entry["args"] = ordered_json::array();
			for (auto o = 0u; o < argument_count(technique); o++)
			{
				const auto& arg = technique->args[o];
				entry["args"].push_back({arg.type, arg.shader, arg.dest, arg.index});
			}

			data["techniques"].push_back(entry);
		}

		filesystem::file file("techsets\\"s + asset->name + ".techset.json");
		file.open("wb");
		file.write(data.dump(4));
		file.close();
	}
}
