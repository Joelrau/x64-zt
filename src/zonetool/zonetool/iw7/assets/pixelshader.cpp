#include <std_include.hpp>
#include "pixelshader.hpp"

#include <utils/cryptography.hpp>

namespace zonetool::iw7
{
	MaterialPixelShader* pixel_shader::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = get_shader_path(name, pixelshader);

		filesystem::file file(path);
		file.open("rb");
		if (!file.get_fp())
		{
			return nullptr;
		}

		const auto buffer_size = file.size();
		const auto buffer = file.read_bytes(buffer_size);

		auto* asset = mem->allocate<MaterialPixelShader>();
		asset->name = mem->duplicate_string(name);
		asset->prog.loadDef.programSize = static_cast<unsigned int>(buffer_size);
		asset->prog.loadDef.program = mem->allocate<unsigned char>(buffer_size);

		std::memcpy(asset->prog.loadDef.program, buffer.data(), buffer_size);

		asset->prog.loadDef.microCodeCrc = utils::cryptography::crc32::compute(asset->prog.loadDef.program, asset->prog.loadDef.programSize);

		file.close();

		asset->debugName = shader::parse_debug_name(asset->name, mem);

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
			ZONETOOL_FATAL("pixelshader \"%s\" not found.", name.data());
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

	void pixel_shader::dump(MaterialPixelShader* asset)
	{
		const auto path = get_shader_path(asset->name, pixelshader);

		filesystem::file file(path);
		file.open("wb");
		file.write(asset->prog.loadDef.program, asset->prog.loadDef.programSize);
		file.close();

		if (asset->debugName)
		{
			shader::dump_debug_name(asset->name, asset->debugName);
		}
	}
}
