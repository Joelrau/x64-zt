#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	struct constant_buffer_data
	{
		std::string vs;
		std::string ps;
		bool operator==(const constant_buffer_data&) const = default;
	};

	class material : public asset_interface
	{
	private:
		std::string name_;
		Material* asset_ = nullptr;

	public:
		Material* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, Material* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(Material* asset);
	};

	class technique_set : public asset_interface
	{
	private:
		std::string name_;
		MaterialTechniqueSet* asset_ = nullptr;

	public:
		MaterialTechniqueSet* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, MaterialTechniqueSet* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(MaterialTechniqueSet* asset);
	};

	std::size_t get_technique_count(const MaterialTechniqueSet* asset);
	std::optional<std::uint32_t> get_technique_type(const MaterialTechniqueSet* asset, std::size_t masked_index);

	template <typename T, XAssetType Type, shader_type ShaderType>
	class material_shader : public asset_interface
	{
	private:
		std::string name_;
		T* asset_ = nullptr;

	public:
		T* parse(const std::string& name, zone_memory* mem)
		{
			assetmanager::reader read(mem);
			if (!read.open(get_legacy_shader_path(name, ShaderType)))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing %s \"%s\"...", get_shader_name(ShaderType).data(), name.data());

			auto* asset = read.read_single<T>();
			asset->name = read.read_string();
			asset->debugName = read.read_string();
			asset->prog.loadDef.program = read.read_array<char>();
			read.close();
			return asset;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = name;

			if (this->referenced())
			{
				this->asset_ = mem->allocate<T>();
				this->asset_->name = mem->duplicate_string(name);
				return;
			}

			this->asset_ = this->parse(name, mem);
			if (!this->asset_)
			{
				ZONETOOL_FATAL("%s \"%s\" not found.", get_shader_name(ShaderType).data(), name.data());
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
		}

		void load_depending(zone_base* zone) override
		{
		}

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		static void read(zone_reader& reader, T* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->debugName);
			reader.read_array(asset->prog.loadDef.program, 3, asset->prog.loadDef.programSize);
			reader.pop_stream();
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);
			dest->name = buf->write_str(this->name());

			if (data->debugName)
			{
				dest->debugName = buf->write_str(data->debugName);
			}

			if (data->prog.loadDef.program)
			{
				buf->align(3);
				buf->write(data->prog.loadDef.program, data->prog.loadDef.programSize);
				buf->clear_pointer(&dest->prog.loadDef.program);
			}
			buf->pop_stream();
		}

		static void dump(T* asset)
		{
			assetmanager::dumper dump;
			if (!dump.open(get_legacy_shader_path(asset->name, ShaderType)))
			{
				return;
			}

			dump.dump_single(asset);
			dump.dump_string(asset->name);
			dump.dump_string(asset->debugName);
			dump.dump_array(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
			dump.close();
		}
	};

	using lib_shader = material_shader<MaterialLibShader, ASSET_TYPE_LIBSHADER, libshader>;
	using compute_shader = material_shader<ComputeShader, ASSET_TYPE_COMPUTESHADER, computeshader>;
	using vertex_shader = material_shader<MaterialVertexShader, ASSET_TYPE_VERTEXSHADER, vertexshader>;
	using hull_shader = material_shader<MaterialHullShader, ASSET_TYPE_HULLSHADER, hullshader>;
	using domain_shader = material_shader<MaterialDomainShader, ASSET_TYPE_DOMAINSHADER, domainshader>;
	using pixel_shader = material_shader<MaterialPixelShader, ASSET_TYPE_PIXELSHADER, pixelshader>;
}
