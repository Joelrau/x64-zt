#include <std_include.hpp>
#include "vertexshader.hpp"

namespace zonetool::iw6
{
	MaterialVertexShader* vertex_shader::parse(const std::string& name, zone_memory* mem)
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
		const auto path = "techsets\\"s + asset->name + ".vertexshader"s;

		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_array(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		write.close();
	}
}