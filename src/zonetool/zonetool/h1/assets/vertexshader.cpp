#include <std_include.hpp>
#include "vertexshader.hpp"

namespace zonetool::h1
{
	namespace
	{
		MaterialVertexShader* parse_legacy(const std::string& name, zone_memory* mem)
		{
			const auto path = "techsets\\" + name + ".vertexshader";

			assetmanager::reader read(mem);
			if (!read.open(path))
			{
				return nullptr;
			}

			//ZONETOOL_INFO("Parsing vertexshader \"%s\"...", name.data());

			auto* asset = read.read_single<MaterialVertexShader>();
			asset->name = read.read_string();
			asset->prog.loadDef.program = read.read_array<unsigned char>();
			read.close();

			return asset;
		}
	}

	MaterialVertexShader* vertex_shader::parse(const std::string& name, zone_memory* mem)
	{
		auto* legacy_parsed = parse_legacy(name, mem);
		if (legacy_parsed) return legacy_parsed;

		const auto path = "techsets\\vs\\"s + name + ".cso"s;

		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		//ZONETOOL_INFO("Parsing vertexshader \"%s\"...", name.data());

		const auto buffer_size = file.size();
		const auto buffer = file.read_bytes(buffer_size);

		auto* asset = mem->allocate<MaterialVertexShader>();
		asset->name = mem->duplicate_string(name);
		asset->prog.loadDef.programSize = static_cast<unsigned int>(buffer_size);
		asset->prog.loadDef.program = mem->allocate<unsigned char>(buffer_size);
		memcpy(asset->prog.loadDef.program, buffer.data(), buffer_size);
		asset->prog.loadDef.microCodeCrc = shader::calc_crc32(asset->prog.loadDef.program, asset->prog.loadDef.programSize);

		file.close();

		return asset;
	}

	void vertex_shader::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).vertexShader;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("vertexshader \"%s\" not found.", name.data());
			}
		}
	}

	void vertex_shader::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void vertex_shader::load_depending(zone_base* zone)
	{
	}

	std::string vertex_shader::name()
	{
		return this->name_;
	}

	std::int32_t vertex_shader::type()
	{
		return ASSET_TYPE_VERTEXSHADER;
	}

	void vertex_shader::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->prog.loadDef.program)
		{
			dest->prog.loadDef.program = buf->write_s(3, data->prog.loadDef.program, data->prog.loadDef.programSize);
		}

		buf->pop_stream();
	}

	void vertex_shader::dump(MaterialVertexShader* asset)
	{
		const auto path = "techsets\\vs\\"s + asset->name + ".cso"s;

		filesystem::file file(path);
		file.open("wb");
		file.write(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		file.close();
	}
}