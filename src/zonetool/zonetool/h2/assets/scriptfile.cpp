#include <std_include.hpp>
#include "scriptfile.hpp"
#include "zlib.h"

namespace zonetool::h2
{
	ScriptFile* scriptfile::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(utils::string::va("%s.gscbin", name.data()));
		file.open("rb");
		auto* fp = file.get_fp();

		if (fp)
		{
			auto* asset = mem->allocate<ScriptFile>();

			std::string m_name;
			file.read_string(&m_name);
			asset->name = mem->duplicate_string(m_name);

			file.read(&asset->compressedLen);
			file.read(&asset->len);
			file.read(&asset->bytecodeLen);

			asset->buffer = mem->allocate<char>(asset->compressedLen);
			asset->bytecode = mem->allocate<char>(asset->bytecodeLen);
			file.read(asset->buffer, asset->compressedLen);
			file.read(asset->bytecode, asset->bytecodeLen);

			return asset;
		}

		return nullptr;
	}

	void scriptfile::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header(XAssetType(this->type()), this->name_.data(), 1).scriptfile;
		}
	}

	void scriptfile::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void scriptfile::load_depending(zone_base* zone)
	{
	}

	std::string scriptfile::name()
	{
		return this->name_;
	}

	std::int32_t scriptfile::type()
	{
		return ASSET_TYPE_SCRIPTFILE;
	}

	void scriptfile::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<ScriptFile>(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		buf->push_stream(XFILE_BLOCK_SCRIPT);

		if (data->buffer)
		{
			buf->write(data->buffer, data->compressedLen);
			zone_buffer::clear_pointer(&dest->buffer);
		}

		if (data->bytecode)
		{
			buf->write(data->bytecode, data->bytecodeLen);
			zone_buffer::clear_pointer(&dest->bytecode);
		}

		buf->pop_stream();

		buf->pop_stream();
	}

	void scriptfile::dump(ScriptFile* asset)
	{
		auto file = filesystem::file(utils::string::va("%s.gscbin", asset->name));
		file.open("wb");
		auto* fp = file.get_fp();

		if (fp)
		{
			file.write_string(asset->name);
			file.write(&asset->compressedLen);
			file.write(&asset->len);
			file.write(&asset->bytecodeLen);
			file.write(asset->buffer, asset->compressedLen);
			file.write(asset->bytecode, asset->bytecodeLen);

			file.close();
		}
	}
}