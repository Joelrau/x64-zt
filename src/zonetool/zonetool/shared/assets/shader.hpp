#pragma once

namespace zonetool
{
	enum shader_type
	{
		none,
		pixelshader,
		vertexshader,
		computeshader,
		hullshader,
		domainshader,
		shader_count,
	};

	namespace
	{
		std::unordered_map<shader_type, std::string> shader_names
		{
			{pixelshader, "pixelshader"},
			{vertexshader, "vertexshader"},
			{computeshader, "computeshader"},
			{hullshader, "hullshader"},
			{domainshader, "domainshader"},
		};

		std::unordered_map<shader_type, std::string> shader_abbreviations
		{
			{pixelshader, "ps"},
			{vertexshader, "vs"},
			{computeshader, "cs"},
			{hullshader, "hs"},
			{domainshader, "ds"},
		};

		std::string get_shader_name(const shader_type type)
		{
			return shader_names.at(type);
		}

		std::string get_shader_abbrev(const shader_type type)
		{
			return shader_abbreviations.at(type);
		}

		std::string get_legacy_shader_path(const std::string& name, const shader_type type)
		{
			const auto& shader_name = shader_names.at(type);
			return utils::string::va("techsets\\%s.%s", name.data(), shader_name.data());
		}

		std::string get_shader_path(const std::string& name, const shader_type type)
		{
			const auto& abbrev = shader_abbreviations.at(type);
			return utils::string::va("techsets\\%s\\%s.cso", abbrev.data(), name.data());
		}

		template <typename T>
		T* parse_legacy(const std::string& name, zone_memory* mem, const shader_type type)
		{
			const auto path = get_legacy_shader_path(name, type);

			assetmanager::reader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			auto* asset = read.read_single<T>();
			asset->name = read.read_string();
			asset->prog.loadDef.program = read.read_array<unsigned char>();
			read.close();

			return asset;
		}
	}

	template <ASSET_TEMPLATE, shader_type ShaderType>
	class shader : public asset_interface
	{
	static_assert(ShaderType < shader_count);

	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			const auto legacy_parsed = parse_legacy<S>(name, mem, ShaderType);
			if (legacy_parsed)
			{
				return legacy_parsed;
			}

			const auto path = get_shader_path(name, ShaderType);

			filesystem::file file(path);
			file.open("rb");
			if (!file.get_fp())
			{
				return nullptr;
			}

			const auto buffer_size = file.size();
			const auto buffer = file.read_bytes(buffer_size);

			auto* asset = mem->allocate<S>();
			asset->name = mem->duplicate_string(name);
			asset->prog.loadDef.programSize = static_cast<unsigned int>(buffer_size);
			asset->prog.loadDef.program = mem->allocate<unsigned char>(buffer_size);

			std::memcpy(asset->prog.loadDef.program, buffer.data(), buffer_size);

			if constexpr (ShaderType == vertexshader || ShaderType == pixelshader)
			{
				asset->prog.loadDef.microCodeCrc = ::shader::calc_crc32(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
			}

			file.close();

			return asset;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			const auto shader_name = get_shader_name(ShaderType);

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
				this->asset_ = reinterpret_cast<S*>(db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).data);

				if (DB_IsXAssetDefault(this->type(), this->name().data()))
				{
					ZONETOOL_FATAL("%s \"%s\" not found.", shader_name.data(), name.data());
				}
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

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto data = this->asset_;
			auto dest = buf->write(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			if constexpr (ShaderType != vertexshader)
			{
				buf->push_stream(Streams::XFILE_BLOCK_TEMP);
			}

			if (data->prog.loadDef.program)
			{
				buf->align(3);
				buf->write(data->prog.loadDef.program, data->prog.loadDef.programSize);
				buf->clear_pointer(&dest->prog.loadDef.program);
			}

			if constexpr (ShaderType != vertexshader)
			{
				buf->pop_stream();
			}

			buf->pop_stream();
		}

		template <typename S>
		static void dump(S* asset)
		{
			const auto path = get_shader_path(asset->name, ShaderType);

			filesystem::file file(path);
			file.open("wb");
			file.write(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
			file.close();
		}
	};
}