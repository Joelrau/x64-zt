#include <std_include.hpp>
#include "material.hpp"

namespace zonetool::s4
{
	namespace
	{
		std::string material_path(std::string name)
		{
			std::ranges::replace(name, '*', '_');
			return "materials\\"s + name;
		}

		template <typename Walk>
		void walk_material(Walk walk, Material& asset)
		{
			walk.array(asset.packedAtlasData, asset.packedAtlasDataSize, 0);
			walk.asset(ASSET_TYPE_TECHNIQUE_SET, asset.techniqueSet);

			if (const auto textures = walk.array(asset.textureTable, asset.textureCount, 7))
			{
				for (auto i = 0; i < asset.textureCount; i++)
				{
					walk.asset(ASSET_TYPE_IMAGE, textures[i].image);
				}
			}

			walk.array(asset.constantTable, asset.constantCount, 15);
			walk.asset(ASSET_TYPE_MATERIAL_STANDARD, asset.materialStandard);
			walk.asset(ASSET_TYPE_DECAL_VOLUME_MATERIAL, asset.decalVolumeMaterial);

			if (const auto table = walk.array(asset.imageTable, 1, 15))
			{
				for (auto& image : table->images)
				{
					walk.asset(ASSET_TYPE_IMAGE, image);
				}
			}

			if (const auto sub_materials = walk.array(asset.subMaterials, asset.layerCount, 7))
			{
				for (auto i = 0; i < asset.layerCount; i++)
				{
					walk.string(sub_materials[i]);
				}
			}
		}

		template <typename Walk>
		void walk_technique(Walk walk, MaterialTechnique& technique)
		{
			walk.string(technique.name);
			walk.array(technique.pipelineStates, technique.pipelineStateInputLayoutCount * technique.renderTargetFormatCount, 7);
			walk.array(technique.inputLayout, 1, 0);
			walk.asset(ASSET_TYPE_VERTEXSHADER, technique.vertexShader);
			walk.asset(ASSET_TYPE_HULLSHADER, technique.hullShader);
			walk.asset(ASSET_TYPE_DOMAINSHADER, technique.domainShader);
			walk.asset(ASSET_TYPE_PIXELSHADER, technique.pixelShader);
			walk.array(technique.renderTargetFormats, technique.renderTargetFormatCount, 0);
			walk.array(technique.args, technique.argCount, 1);
		}

		template <typename Walk>
		void walk_technique_set(Walk walk, MaterialTechniqueSet& asset)
		{
			const auto count = get_technique_count(&asset);
			if (const auto techniques = walk.array(asset.maskedTechniques, count, 7))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					if (const auto technique = walk.array(techniques[i], 1, 7))
					{
						walk_technique(walk, *technique);
					}
				}
			}
		}

		template <typename Walk>
		void walk_compute_program(Walk walk, ComputeShaderProgram*& field)
		{
			if (const auto program = walk.array(field, 1, 7))
			{
				walk.string(program->name);
				walk.string(program->debugName);
				walk.array(program->loadDef.program, program->loadDef.programSize, 3);
			}
		}

		template <typename Walk>
		void walk_compute_shader(Walk walk, ComputeShader& asset)
		{
			walk.string(asset.debugName);
			for (auto& program : asset.programs)
			{
				walk_compute_program(walk, program);
			}

			if (const auto variants = walk.array(asset.variants, asset.variantCount, 7))
			{
				for (auto i = 0u; i < asset.variantCount; i++)
				{
					walk_compute_program(walk, variants[i]);
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
		for (auto type = 0u; type < std::size(asset->techniqueMask.mask) * 64; type++)
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
		walk_material(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void material::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_material(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void material::dump(Material* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(material_path(asset->name)))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_material(dump_walk{dump}, *asset);
		dump.close();
	}

	Material* material::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(material_path(name)))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing material \"%s\"...", name.data());

		auto* asset = read.read_single<Material>();
		asset->name = read.read_string();
		walk_material(parse_walk{read}, *asset);
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
	}

	void material::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void material::load_depending(zone_base* zone)
	{
		if (!this->referenced())
		{
			walk_material(depend_walk{zone}, *this->asset_);
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

	void technique_set::read(zone_reader& reader, MaterialTechniqueSet* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_technique_set(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void technique_set::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_technique_set(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void technique_set::dump(MaterialTechniqueSet* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("techsets\\"s + asset->name + ".techset"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_technique_set(dump_walk{dump}, *asset);
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
		walk_technique_set(parse_walk{read}, *asset);
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
		if (!this->referenced())
		{
			walk_technique_set(depend_walk{zone}, *this->asset_);
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

	void material_standard::read(zone_reader& reader, MaterialStandard* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}

	void material_standard::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		buf->pop_stream();
	}

	void material_standard::dump(MaterialStandard* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("materialstandard\\"s + asset->name + ".materialstandardb"s))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.close();
	}

	MaterialStandard* material_standard::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("materialstandard\\"s + name + ".materialstandardb"s))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing materialstandard \"%s\"...", name.data());

		auto* asset = read.read_single<MaterialStandard>();
		asset->name = read.read_string();
		read.close();
		return asset;
	}

	void material_standard::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<MaterialStandard>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Materialstandard \"%s\" not found.", name.data());
		}
	}

	std::string material_standard::name()
	{
		return this->name_;
	}

	std::int32_t material_standard::type()
	{
		return ASSET_TYPE_MATERIAL_STANDARD;
	}

	void compute_shader::read(zone_reader& reader, ComputeShader* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_compute_shader(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void compute_shader::write(zone_base* zone, zone_buffer* buf)
	{
		auto* dest = buf->write(this->asset_);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_compute_shader(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void compute_shader::dump(ComputeShader* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open(get_legacy_shader_path(asset->name, computeshader)))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_compute_shader(dump_walk{dump}, *asset);
		dump.close();
	}

	ComputeShader* compute_shader::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(get_legacy_shader_path(name, computeshader)))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing computeshader \"%s\"...", name.data());

		auto* asset = read.read_single<ComputeShader>();
		asset->name = read.read_string();
		walk_compute_shader(parse_walk{read}, *asset);
		read.close();
		return asset;
	}

	void compute_shader::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ComputeShader>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Computeshader \"%s\" not found.", name.data());
		}
	}

	std::string compute_shader::name()
	{
		return this->name_;
	}

	std::int32_t compute_shader::type()
	{
		return ASSET_TYPE_COMPUTESHADER;
	}
}
