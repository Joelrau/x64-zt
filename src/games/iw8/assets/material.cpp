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

		void write_constant_buffer_data(zone_buffer* buf, const char* source, char*& target, const unsigned int size)
		{
			if (source)
			{
				buf->align(15);
				buf->write(source, size);
				buf->clear_pointer(&target);
			}
		}

		const MaterialConstantDef* find_constant(const Material* asset, const unsigned short index)
		{
			for (auto i = 0; asset->constantTable && i < asset->constantCount; i++)
			{
				if (asset->constantTable[i].index == index)
				{
					return &asset->constantTable[i];
				}
			}

			return nullptr;
		}

		void place_constant(std::string& data, const unsigned short dest, const MaterialConstantDef* constant)
		{
			const auto offset = dest * sizeof(vec4_t);
			if (data.size() < offset + sizeof(vec4_t))
			{
				data.resize(offset + sizeof(vec4_t));
			}

			if (constant)
			{
				std::memcpy(data.data() + offset, constant->literal, sizeof(vec4_t));
			}
		}

		char* duplicate_data(const std::string& data, unsigned int& size, zone_memory* mem)
		{
			size = static_cast<unsigned int>(data.size());
			if (data.empty())
			{
				return nullptr;
			}

			const auto result = mem->allocate<char>(data.size());
			std::memcpy(result, data.data(), data.size());
			return result;
		}

		void generate_constant_buffers(Material* asset, const MaterialTechniqueSet* techset, zone_memory* mem)
		{
			std::vector<constant_buffer_data> buffers;
			asset->constantBufferIndex = mem->allocate<unsigned char>(TECHNIQUE_COUNT);
			std::memset(asset->constantBufferIndex, 0xFF, TECHNIQUE_COUNT);

			for (std::size_t i = 0; techset->maskedTechniques && i < get_technique_count(techset); i++)
			{
				const auto* technique = techset->maskedTechniques[i];
				const auto type = get_technique_type(techset, i);
				if (!technique || !technique->args || !type)
				{
					continue;
				}

				constant_buffer_data data;
				for (std::size_t arg = 0; arg < argument_count(technique); arg++)
				{
					const auto& argument = technique->args[arg];
					if (argument.type != MTL_ARG_MATERIAL_CONST)
					{
						continue;
					}

					const auto* constant = find_constant(asset, argument.index);
					if (argument.shader & MTL_SHADER_STAGE_VS)
					{
						place_constant(data.vs, argument.dest, constant);
					}
					if (argument.shader & MTL_SHADER_STAGE_PS)
					{
						place_constant(data.ps, argument.dest, constant);
					}
				}

				if (data.vs.empty() && data.ps.empty())
				{
					continue;
				}

				const auto found = std::ranges::find(buffers, data);
				asset->constantBufferIndex[*type] = static_cast<unsigned char>(found - buffers.begin());
				if (found == buffers.end())
				{
					buffers.emplace_back(std::move(data));
				}
			}

			asset->constantBufferCount = static_cast<unsigned char>(buffers.size());
			asset->constantBufferTable = buffers.empty() ? nullptr : mem->allocate<MaterialConstantBufferDef>(buffers.size());
			for (std::size_t i = 0; i < buffers.size(); i++)
			{
				auto& buffer = asset->constantBufferTable[i];
				buffer.vsData = duplicate_data(buffers[i].vs, buffer.vsDataSize, mem);
				buffer.psData = duplicate_data(buffers[i].ps, buffer.psDataSize, mem);
			}
		}

		template <typename T>
		void write_shader(zone_base* zone, T*& target, const T* shader, const XAssetType type)
		{
			if (shader)
			{
				target = static_cast<T*>(zone->get_asset_pointer(type, shader->name));
			}
		}

		void write_technique(zone_base* zone, zone_buffer* buf, const MaterialTechnique* technique, MaterialTechnique* dest)
		{
			if (technique->name)
			{
				dest->name = buf->write_str(technique->name);
			}

			write_shared_array(buf, dest->pipelineStates, technique->pipelineStateInputLayoutCount * technique->renderTargetFormatCount, 7);
			write_shared_array(buf, dest->stateBitsTable, 1, 0);

			write_shader(zone, dest->vertexShader, technique->vertexShader, ASSET_TYPE_VERTEXSHADER);
			write_shader(zone, dest->hullShader, technique->hullShader, ASSET_TYPE_HULLSHADER);
			write_shader(zone, dest->domainShader, technique->domainShader, ASSET_TYPE_DOMAINSHADER);
			write_shader(zone, dest->pixelShader, technique->pixelShader, ASSET_TYPE_PIXELSHADER);

			write_shared_array(buf, dest->renderTargetFormats, technique->renderTargetFormatCount, 0);
			write_shared_array(buf, dest->args, argument_count(technique), 1);
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

	Material* material::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("materials\\"s + name))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing material \"%s\"...", name.data());

		auto* asset = read.read_single<Material>();
		asset->name = read.read_string();
		asset->packedAtlasData = read.read_array<char>();
		asset->techniqueSet = read.read_asset<MaterialTechniqueSet>();

		asset->textureTable = read.read_array<MaterialTextureDef>();
		for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
		{
			asset->textureTable[i].image = read.read_asset<GfxImage>();
		}

		asset->constantTable = read.read_array<MaterialConstantDef>();
		asset->decalVolumeMaterial = read.read_asset<GfxDecalVolumeMaterial>();
		asset->constantBufferIndex = read.read_array<unsigned char>();

		asset->constantBufferTable = read.read_array<MaterialConstantBufferDef>();
		for (auto i = 0; asset->constantBufferTable && i < asset->constantBufferCount; i++)
		{
			auto& buffer = asset->constantBufferTable[i];
			buffer.vsData = read.read_array<char>();
			buffer.hsData = read.read_array<char>();
			buffer.dsData = read.read_array<char>();
			buffer.psData = read.read_array<char>();
		}

		asset->subMaterials = read.read_array<const char*>();
		for (auto i = 0; asset->subMaterials && i < asset->layerCount; i++)
		{
			asset->subMaterials[i] = read.read_string();
		}

		read.close();
		return asset;
	}

	void material::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<Material>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Material \"%s\" not found.", name.data());
		}

		if (this->asset_->constantTable && !this->asset_->constantBufferTable && this->asset_->techniqueSet)
		{
			const std::string techset_name = this->asset_->techniqueSet->name;
			auto* techset = static_cast<MaterialTechniqueSet*>(db_find_asset(ASSET_TYPE_TECHNIQUE_SET, techset_name));
			if (!techset)
			{
				techset = technique_set().parse(techset_name, mem);
			}

			if (!techset)
			{
				ZONETOOL_FATAL("Material \"%s\" has no constant buffers and techset \"%s\" is not loaded.", name.data(), techset_name.data());
			}

			generate_constant_buffers(this->asset_, techset, mem);
		}
	}

	void material::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void material::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		if (data->techniqueSet)
		{
			zone->add_asset_of_type(ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet->name);
		}

		for (auto i = 0; data->textureTable && i < data->textureCount; i++)
		{
			if (data->textureTable[i].image)
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->textureTable[i].image->name);
			}
		}

		if (data->decalVolumeMaterial)
		{
			zone->add_asset_of_type(ASSET_TYPE_DECAL_VOLUME_MATERIAL, data->decalVolumeMaterial->name);
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
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->packedAtlasData)
		{
			buf->write(data->packedAtlasData, data->packedAtlasDataSize);
			buf->clear_pointer(&dest->packedAtlasData);
		}

		if (data->techniqueSet)
		{
			dest->techniqueSet = static_cast<MaterialTechniqueSet*>(zone->get_asset_pointer(ASSET_TYPE_TECHNIQUE_SET, data->techniqueSet->name));
		}

		if (data->textureTable)
		{
			buf->align(7);
			auto* textures = buf->write(data->textureTable, data->textureCount);
			for (auto i = 0; i < data->textureCount; i++)
			{
				if (data->textureTable[i].image)
				{
					textures[i].image = static_cast<GfxImage*>(zone->get_asset_pointer(ASSET_TYPE_IMAGE, data->textureTable[i].image->name));
				}
			}
			buf->clear_pointer(&dest->textureTable);
		}

		if (data->constantTable)
		{
			buf->align(15);
			buf->write(data->constantTable, data->constantCount);
			buf->clear_pointer(&dest->constantTable);
		}

		if (data->decalVolumeMaterial)
		{
			dest->decalVolumeMaterial = static_cast<GfxDecalVolumeMaterial*>(zone->get_asset_pointer(ASSET_TYPE_DECAL_VOLUME_MATERIAL, data->decalVolumeMaterial->name));
		}

		if (data->constantBufferIndex)
		{
			buf->write(data->constantBufferIndex, TECHNIQUE_COUNT);
			buf->clear_pointer(&dest->constantBufferIndex);
		}

		if (data->constantBufferTable)
		{
			buf->align(15);
			auto* buffers = buf->write(data->constantBufferTable, data->constantBufferCount);
			for (auto i = 0; i < data->constantBufferCount; i++)
			{
				const auto& buffer = data->constantBufferTable[i];
				write_constant_buffer_data(buf, buffer.vsData, buffers[i].vsData, buffer.vsDataSize);
				write_constant_buffer_data(buf, buffer.hsData, buffers[i].hsData, buffer.hsDataSize);
				write_constant_buffer_data(buf, buffer.dsData, buffers[i].dsData, buffer.dsDataSize);
				write_constant_buffer_data(buf, buffer.psData, buffers[i].psData, buffer.psDataSize);
			}
			buf->clear_pointer(&dest->constantBufferTable);
		}

		if (data->subMaterials)
		{
			buf->align(7);
			auto* sub_materials = buf->write(data->subMaterials, data->layerCount);
			for (auto i = 0; i < data->layerCount; i++)
			{
				if (data->subMaterials[i])
				{
					sub_materials[i] = buf->write_str(data->subMaterials[i]);
				}
			}
			buf->clear_pointer(&dest->subMaterials);
		}
		buf->pop_stream();
	}

	void material::dump(Material* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("materials\\"s + asset->name))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->packedAtlasData, asset->packedAtlasDataSize);
		dump.dump_asset(asset->techniqueSet);

		dump.dump_array(asset->textureTable, asset->textureCount);
		for (auto i = 0; asset->textureTable && i < asset->textureCount; i++)
		{
			dump.dump_asset(asset->textureTable[i].image);
		}

		dump.dump_array(asset->constantTable, asset->constantCount);
		dump.dump_asset(asset->decalVolumeMaterial);
		dump.dump_array(asset->constantBufferIndex, TECHNIQUE_COUNT);

		dump.dump_array(asset->constantBufferTable, asset->constantBufferCount);
		for (auto i = 0; asset->constantBufferTable && i < asset->constantBufferCount; i++)
		{
			const auto& buffer = asset->constantBufferTable[i];
			dump.dump_array(buffer.vsData, buffer.vsDataSize);
			dump.dump_array(buffer.hsData, buffer.hsDataSize);
			dump.dump_array(buffer.dsData, buffer.dsDataSize);
			dump.dump_array(buffer.psData, buffer.psDataSize);
		}

		dump.dump_array(asset->subMaterials, asset->layerCount);
		for (auto i = 0; asset->subMaterials && i < asset->layerCount; i++)
		{
			dump.dump_string(asset->subMaterials[i]);
		}
		dump.close();
	}

	MaterialTechniqueSet* technique_set::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("techsets\\"s + name + ".techset"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing techset \"%s\"...", name.data());

		auto* asset = read.read_single<MaterialTechniqueSet>();
		asset->name = read.read_string();
		asset->maskedTechniques = read.read_array<MaterialTechnique*>();

		const auto count = get_technique_count(asset);
		for (std::size_t i = 0; asset->maskedTechniques && i < count; i++)
		{
			auto* technique = read.read_single<MaterialTechnique>();
			asset->maskedTechniques[i] = technique;
			if (!technique)
			{
				continue;
			}

			technique->name = read.read_string();
			technique->pipelineStates = read.read_array<Packed128>();
			technique->stateBitsTable = read.read_array<GfxStateBits>();
			technique->vertexShader = read.read_asset<MaterialVertexShader>();
			technique->hullShader = read.read_asset<MaterialHullShader>();
			technique->domainShader = read.read_asset<MaterialDomainShader>();
			technique->pixelShader = read.read_asset<MaterialPixelShader>();
			technique->renderTargetFormats = read.read_array<MaterialRenderTargetFormat>();
			technique->args = read.read_array<MaterialShaderArgument>();
		}

		read.close();
		return asset;
	}

	void technique_set::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<MaterialTechniqueSet>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Techset \"%s\" not found.", name.data());
		}
	}

	void technique_set::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void technique_set::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		for (std::size_t i = 0; data->maskedTechniques && i < get_technique_count(data); i++)
		{
			const auto* technique = data->maskedTechniques[i];
			if (!technique)
			{
				continue;
			}

			if (technique->vertexShader)
			{
				zone->add_asset_of_type(ASSET_TYPE_VERTEXSHADER, technique->vertexShader->name);
			}

			if (technique->hullShader)
			{
				zone->add_asset_of_type(ASSET_TYPE_HULLSHADER, technique->hullShader->name);
			}

			if (technique->domainShader)
			{
				zone->add_asset_of_type(ASSET_TYPE_DOMAINSHADER, technique->domainShader->name);
			}

			if (technique->pixelShader)
			{
				zone->add_asset_of_type(ASSET_TYPE_PIXELSHADER, technique->pixelShader->name);
			}
		}
	}

	std::string technique_set::name()
	{
		return this->name_;
	}

	std::int32_t technique_set::type()
	{
		return ASSET_TYPE_TECHNIQUE_SET;
	}

	void technique_set::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->maskedTechniques)
		{
			const auto count = get_technique_count(data);
			buf->align(7);
			auto* techniques = buf->write(data->maskedTechniques, count);
			for (std::size_t i = 0; i < count; i++)
			{
				auto* technique = data->maskedTechniques[i];
				if (!technique)
				{
					continue;
				}

				buf->align(7);
				auto* dest_technique = buf->write(technique);
				write_technique(zone, buf, technique, dest_technique);
				buf->clear_pointer(&techniques[i]);
			}
			buf->clear_pointer(&dest->maskedTechniques);
		}
		buf->pop_stream();
	}

	void technique_set::dump(MaterialTechniqueSet* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("techsets\\"s + asset->name + ".techset"s))
		{
			return;
		}

		const auto count = get_technique_count(asset);
		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->maskedTechniques, static_cast<std::uint32_t>(count));

		for (std::size_t i = 0; asset->maskedTechniques && i < count; i++)
		{
			const auto* technique = asset->maskedTechniques[i];
			dump.dump_single(technique);
			if (!technique)
			{
				continue;
			}

			dump.dump_string(technique->name);
			dump.dump_array(technique->pipelineStates, technique->pipelineStateInputLayoutCount * technique->renderTargetFormatCount);
			dump.dump_array(technique->stateBitsTable, 1);
			dump.dump_asset(technique->vertexShader);
			dump.dump_asset(technique->hullShader);
			dump.dump_asset(technique->domainShader);
			dump.dump_asset(technique->pixelShader);
			dump.dump_array(technique->renderTargetFormats, technique->renderTargetFormatCount);
			dump.dump_array(technique->args, static_cast<std::uint32_t>(argument_count(technique)));
		}
		dump.close();
	}
}
