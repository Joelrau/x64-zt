#include <std_include.hpp>
#include "ttf.hpp"

namespace zonetool::h1
{
	TTFDef* IFont::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing ttf \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);
			
			auto* font = mem->allocate<TTFDef>();
			font->name = mem->duplicate_string(name);

			font->fileLen = static_cast<int>(size - 1);
			font->file = mem->allocate<char>(font->fileLen + 1);
			memcpy(
				const_cast<char*>(font->file),
				data.data(),
				data.size());

			file.close();

			return font;
		}

		return nullptr;
	}

	void IFont::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).ttfDef;
		}
	}

	void IFont::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void IFont::load_depending(zone_base* zone)
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

	void IFont::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<TTFDef>(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->file)
		{
			buf->align(0);
			buf->write(data->file, data->fileLen + 1);
			zone_buffer::clear_pointer(&dest->file);
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