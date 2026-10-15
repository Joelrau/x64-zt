#pragma once

namespace zonetool::s2
{
	std::optional<std::string> read_image_stream(const XStreamFileKey& key);

	constexpr std::size_t image_stream_count = 4;

	class gfx_image : public asset_base<GfxImage, ASSET_TYPE_IMAGE>
	{
	public:
		static void read(zone_reader& reader, GfxImage* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);

			if (asset->semantic == TS_RESIDENT)
			{
				reader.read_array(asset->pixelData, 0, asset->dataLen1);
			}
			else
			{
				reader.push_stream(XFILE_BLOCK_TEMP);
				if (const auto data = reader.read_aliased_array(asset->pixelData, 0, asset->dataLen1))
				{
					asset->pixelData = reader.duplicate(data, asset->dataLen1);
				}
				reader.pop_stream();
			}

			reader.pop_stream();
		}

		GfxImage* parse(const std::string& name, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(GfxImage* asset);

		std::span<const XStreamFileKey> stream_keys() const;

	private:
		GfxImage* parse_custom(const std::string& name, zone_memory* mem);

		std::array<XStreamFileKey, image_stream_count> parsed_stream_keys_{};
		bool has_parsed_stream_keys_ = false;
	};

	class clut : public asset_base<Clut, ASSET_TYPE_CLUT>
	{
	public:
		static void read(zone_reader& reader, Clut* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_array(asset->data, 0, 4 * asset->width * asset->height * asset->depth);
			reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
			reader.read_string(asset->name);
			reader.pop_stream();
		}

		Clut* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_CLUT, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing clut \"%s\"...", name.data());

			auto* asset = read.read_single<Clut>();
			asset->name = read.read_string();
			asset->data = read.read_array<char>();
			asset->image = read.read_asset<GfxImage>();
			read.close();

			return asset;
		}

		void load_depending(zone_base* zone) override
		{
			add_dependency(zone, ASSET_TYPE_IMAGE, this->asset_->image);
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			if (data->data)
			{
				buf->align(0);
				buf->write(data->data, 4 * data->width * data->height * data->depth);
				buf->clear_pointer(&dest->data);
			}
			dest->image = asset_reference(zone, ASSET_TYPE_IMAGE, data->image);
			dest->name = this->write_name(buf);

			buf->pop_stream();
		}

		static void dump(Clut* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_CLUT, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->data, 4 * asset->width * asset->height * asset->depth);
			write.dump_asset(asset->image);
			write.close();
		}
	};
}
