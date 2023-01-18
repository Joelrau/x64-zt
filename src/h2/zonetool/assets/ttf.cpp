#include <std_include.hpp>
#include "ttf.hpp"

namespace zonetool::h2
{
	TTFDef* IFont::parse(const std::string& name, ZoneMemory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing ttf \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);
			
			auto* font = mem->Alloc<TTFDef>();
			font->name = mem->StrDup(name);

			font->fileLen = static_cast<int>(size - 1);
			font->file = mem->Alloc<char>(font->fileLen + 1);
			memcpy(
				const_cast<char*>(font->file),
				data.data(),
				data.size());

			file.close();

			return font;
		}

		return nullptr;
	}

	void IFont::init(const std::string& name, ZoneMemory* mem)
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
			this->asset_ = DB_FindXAssetHeader_Safe(XAssetType(this->type()), this->name_.data()).ttfDef;
		}
	}

	void IFont::prepare(ZoneBuffer* buf, ZoneMemory* mem)
	{
	}

	void IFont::load_depending(IZone* zone)
	{
	}

	std::string IFont::name()
	{
		return this->name_;
	}

	std::int32_t IFont::type()
	{
		return ASSET_TYPE_TTF;
	}

	void IFont::write(IZone* zone, ZoneBuffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<TTFDef>(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->file)
		{
			buf->align(0);
			buf->write(data->file, data->fileLen + 1);
			ZoneBuffer::clear_pointer(&dest->file);
		}

		buf->pop_stream();
	}

	void IFont::dump(TTFDef* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");

		file.write(asset->file, asset->fileLen + 1, 1);

		file.close();
	}
}