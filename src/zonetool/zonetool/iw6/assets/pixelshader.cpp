#include <std_include.hpp>
#include "pixelshader.hpp"

namespace zonetool::iw6
{
	MaterialPixelShader* pixel_shader::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "techsets\\" + name + ".pixelshader";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		//ZONETOOL_INFO("Parsing pixelshader \"%s\"...", name.data());

		auto* asset = read.read_single<MaterialPixelShader>();
		asset->name = read.read_string();
		asset->prog.loadDef.program = read.read_array<unsigned char>();
		read.close();

		return asset;
	}

	void pixel_shader::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name().data()).pixelShader;

			if (DB_IsXAssetDefault(XAssetType(this->type()), this->name().data()))
			{
				ZONETOOL_FATAL("pixelshader \"%s\" not found.", name.data());
			}
		}
	}

	void pixel_shader::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void pixel_shader::load_depending(zone_base* zone)
	{
	}

	std::string pixel_shader::name()
	{
		return this->name_;
	}

	std::int32_t pixel_shader::type()
	{
		return ASSET_TYPE_PIXELSHADER;
	}

	void pixel_shader::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->prog.loadDef.program)
		{
			dest->prog.loadDef.program = buf->write_s(3, data->prog.loadDef.program, data->prog.loadDef.programSize);
		}

		buf->pop_stream();
	}

	void pixel_shader::dump(MaterialPixelShader* asset)
	{
		const auto path = "techsets\\"s + asset->name + ".pixelshader"s;

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