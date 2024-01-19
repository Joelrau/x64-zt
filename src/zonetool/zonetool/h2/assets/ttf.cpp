#include <std_include.hpp>
#include "ttf.hpp"

namespace zonetool::h2
{
	TTFDef* font_def::parse(const std::string& name, zone_memory* mem)
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

	void font_def::init(const std::string& name, zone_memory* mem)
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

	void font_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void font_def::load_depending(zone_base* zone)
	{
	}

	std::string font_def::name()
	{
		return this->name_;
	}

	std::int32_t font_def::type()
	{
		return ASSET_TYPE_TTF;
	}

	void font_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<TTFDef>(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->file)
		{
			buf->align(0);
			buf->write(data->file, data->fileLen + 1);
			buf->clear_pointer(&dest->file);
		}

		buf->pop_stream();
	}

	void font_def::dump(TTFDef* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");

		file.write(asset->file, asset->fileLen + 1, 1);

		file.close();
	}
}