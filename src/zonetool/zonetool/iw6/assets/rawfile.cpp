#include <std_include.hpp>
#include "rawfile.hpp"

#include <zlib.h>

#define ZONETOOL_BRANDING "Compiled using IW6 ZoneTool."

namespace zonetool::iw6
{
	RawFile* rawfile::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing rawfile \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);

			auto* rawfile = mem->allocate<RawFile>();
			rawfile->name = mem->duplicate_string(name);

			zone_buffer buf(data);
			auto compressed = buf.compress_zlib();

			rawfile->len = static_cast<int>(size);
			rawfile->compressedLen = static_cast<int>(compressed.size());
			rawfile->buffer = mem->allocate<char>(compressed.size());
			memcpy(
				const_cast<char*>(rawfile->buffer),
				compressed.data(),
				compressed.size());

			file.close();

			return rawfile;
		}

		return nullptr;
	}

	void rawfile::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (name == filesystem::get_fastfile())
		{
			std::string str = ZONETOOL_BRANDING;
			zone_buffer buf(std::vector<uint8_t>(str.begin(), str.end()));
			auto compressed = buf.compress_zlib();
			std::string buffer(compressed.begin(), compressed.end());

			this->asset_ = mem->allocate<RawFile>();
			this->asset_->name = mem->duplicate_string(name);
			this->asset_->buffer = mem->duplicate_string(buffer.data());
			this->asset_->len = static_cast<int>(str.size());
			this->asset_->compressedLen = static_cast<int>(buffer.size());
		}
		else if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).rawfile;
		}
	}

	void rawfile::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void rawfile::load_depending(zone_base* zone)
	{
	}

	std::string rawfile::name()
	{
		return this->name_;
	}

	std::int32_t rawfile::type()
	{
		return ASSET_TYPE_RAWFILE;
	}

	void rawfile::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write<RawFile>(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->buffer)
		{
			buf->align(0);
			buf->write(data->buffer, data->compressedLen ? data->compressedLen : data->len + 1);
			zone_buffer::clear_pointer(&dest->buffer);
		}

		buf->pop_stream();
	}

	void rawfile::dump(RawFile* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");

		if (asset->compressedLen > 0)
		{
			std::vector<std::uint8_t> uncompressed;
			uncompressed.resize(asset->len);

			uncompress(uncompressed.data(), (uLongf*)&asset->len, (Bytef*)asset->buffer, asset->compressedLen);

			file.write(uncompressed.data(), uncompressed.size(), 1);
		}
		else if (asset->len > 0)
		{
			file.write(asset->buffer, asset->len, 1);
		}

		file.close();
	}
}