#include <std_include.hpp>
#include "domainshader.hpp"

namespace zonetool::iw7
{
	static const char* parse_debug_name(const char* name, zone_memory* mem)
	{
		const auto& abbrev = get_shader_abbrev(domainshader);
		const auto path = utils::string::va("techsets\\dbg\\%s\\%s", abbrev.data(), name);

		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return name;
		}

		const auto data = file.read_bytes(file.size());
		file.close();

		return mem->duplicate_string({ data.begin(), data.end() });
	}

	MaterialDomainShader* domain_shader::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = get_shader_path(name, domainshader);

		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		const auto buffer_size = file.size();
		const auto buffer = file.read_bytes(buffer_size);

		auto* asset = mem->allocate<MaterialDomainShader>();
		asset->name = mem->duplicate_string(name);
		asset->prog.loadDef.programSize = static_cast<unsigned int>(buffer_size);
		asset->prog.loadDef.program = mem->allocate<unsigned char>(buffer_size);

		std::memcpy(asset->prog.loadDef.program, buffer.data(), buffer_size);

		file.close();

		asset->debugName = parse_debug_name(asset->name, mem);

		return asset;
	}

	void domain_shader::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).domainShader;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("domainshader \"%s\" not found.", name.data());
			}
		}
	}

	void domain_shader::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void domain_shader::load_depending(zone_base* zone)
	{
	}

	std::string domain_shader::name()
	{
		return this->name_;
	}

	std::int32_t domain_shader::type()
	{
		return ASSET_TYPE_DOMAINSHADER;
	}

	void domain_shader::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->debugName)
		{
			dest->debugName = buf->write_str(data->debugName);
		}

		buf->push_stream(XFILE_BLOCK_TEMP);
		if (data->prog.loadDef.program)
		{
			buf->align(3);
			buf->write(data->prog.loadDef.program, data->prog.loadDef.programSize);
			buf->clear_pointer(&dest->prog.loadDef.program);
		}
		buf->pop_stream();

		buf->pop_stream();
	}

	static void dump_debug_name(const std::string& name, const std::string& debug_name)
	{
		if (name == debug_name)
		{
			return;
		}

		const auto& abbrev = get_shader_abbrev(domainshader);
		const auto path = utils::string::va("techsets\\dbg\\%s\\%s", abbrev.data(), name.data());

		filesystem::file file(path);
		file.open("wb");
		file.write(debug_name.data(), debug_name.size());
		file.close();
	}

	void domain_shader::dump(MaterialDomainShader* asset)
	{
		const auto path = get_shader_path(asset->name, domainshader);

		filesystem::file file(path);
		file.open("wb");
		file.write(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		file.close();

		if (asset->debugName)
		{
			dump_debug_name(asset->name, asset->debugName);
		}
	}
}