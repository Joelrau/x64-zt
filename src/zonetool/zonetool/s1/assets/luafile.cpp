#include <std_include.hpp>
#include "luafile.hpp"

namespace zonetool::s1
{
	LuaFile* ILuaFile::parse(const std::string& name, ZoneMemory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing luafile \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);

			auto* luafile = mem->Alloc<LuaFile>();
			luafile->name = mem->StrDup(name);

			luafile->len = static_cast<int>(size);
			luafile->buffer = mem->Alloc<char>(luafile->len);
			memcpy(
				const_cast<char*>(luafile->buffer),
				data.data(),
				data.size());

			file.close();

			return luafile;
		}

		return nullptr;
	}

	void ILuaFile::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).luaFile;
		}
	}

	void ILuaFile::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void ILuaFile::load_depending(IZone* zone)
	{
	}

	std::string ILuaFile::name()
	{
		return this->name_;
	}

	std::int32_t ILuaFile::type()
	{
		return ASSET_TYPE_LUA_FILE;
	}

	void ILuaFile::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<LuaFile>(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->buffer)
		{
			buf->align(15);
			buf->write(data->buffer, data->len);
			ZoneBuffer::clear_pointer(&dest->buffer);
		}

		buf->pop_stream();
	}

	void ILuaFile::dump(LuaFile* asset)
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