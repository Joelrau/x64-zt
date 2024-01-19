#include <std_include.hpp>
#include "luafile.hpp"

namespace zonetool::s1
{
	LuaFile* lua_file::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing luafile \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);

			auto* luafile = mem->allocate<LuaFile>();
			luafile->name = mem->duplicate_string(name);

			luafile->len = static_cast<int>(size);
			luafile->buffer = mem->allocate<char>(luafile->len);
			memcpy(
				const_cast<char*>(luafile->buffer),
				data.data(),
				data.size());

			file.close();

			return luafile;
		}

		return nullptr;
	}

	void lua_file::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).luaFile;
		}
	}

	void lua_file::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void lua_file::load_depending(zone_base* zone)
	{
	}

	std::string lua_file::name()
	{
		return this->name_;
	}

	std::int32_t lua_file::type()
	{
		return ASSET_TYPE_LUA_FILE;
	}

	void lua_file::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<LuaFile>(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->buffer)
		{
			buf->align(15);
			buf->write(data->buffer, data->len);
			buf->clear_pointer(&dest->buffer);
		}

		buf->pop_stream();
	}

	void lua_file::dump(LuaFile* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");

		if (asset->len > 0)
		{
			file.write(asset->buffer, asset->len, 1);
		}

		file.close();
	}
}