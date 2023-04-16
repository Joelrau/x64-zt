#pragma once

#include "../../utils/utils.hpp"
#include "../interfaces/asset.hpp"
#include "../interfaces/zone.hpp"

#include <zlib.h>

#define ZONETOOL_BRANDING "Compiled using x64 ZoneTool."

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class rawfile : public asset_interface
	{
	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			auto file = filesystem::file(name);
			file.open("rb");

			if (!file.get_fp())
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing rawfile \"%s\"...", name.data());

			const auto size = file.size();
			auto data = file.read_bytes(size);

			S* rawfile = mem->allocate<S>();
			rawfile->name = mem->duplicate_string(name);

			zone_buffer buf(data);
			const auto compressed = buf.compress_zlib();

			rawfile->len = static_cast<int>(size);
			rawfile->compressedLen = static_cast<int>(compressed.size());
			rawfile->buffer = mem->allocate<char>(compressed.size());
			std::memcpy(
				const_cast<char*>(rawfile->buffer),
				compressed.data(),
				compressed.size());

			file.close();

			return rawfile;
		}

		void init(const std::string& name, zone_memory* mem) override
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

				this->asset_ = mem->allocate<S>();
				this->asset_->name = mem->duplicate_string(name);
				this->asset_->buffer = mem->duplicate_string(buffer.data());
				this->asset_->len = static_cast<int>(str.size());
				this->asset_->compressedLen = static_cast<int>(buffer.size());
			}
			else if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name_.data()).rawfile;
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
		}

		void load_depending(zone_base* zone) override
		{
		}

		void* pointer() override { return asset_; }

		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write<S>(data);

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

		template <typename S>
		static void dump(S* asset)
		{
			auto file = filesystem::file(asset->name);
			file.open("wb");

			if (asset->compressedLen > 0)
			{
				std::vector<std::uint8_t> uncompressed;
				uncompressed.resize(asset->len);

				uncompress(uncompressed.data(), reinterpret_cast<uLongf*>(&asset->len), 
					(Bytef*)(asset->buffer), asset->compressedLen);

				file.write(uncompressed.data(), uncompressed.size(), 1);
			}
			else if (asset->len > 0)
			{
				file.write(asset->buffer, asset->len, 1);
			}

			file.close();
		}
	};
}