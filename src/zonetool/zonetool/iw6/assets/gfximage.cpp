#include <std_include.hpp>
#include "gfximage.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#include "zonetool/utils/iwi.hpp"

#include <utils/io.hpp>
#include <utils/cryptography.hpp>
#include <utils/flags.hpp>

namespace zonetool::iw6
{
	namespace
	{
		bool is_map_image(const std::string& name)
		{
			return (name.size() >= 6)
				? ((name.substr(0, 6) == "*light" || name.substr(0, 6) == "*refle" ||
					name == "$outdoor")
					? true
					: false)
				: false;
		}

		std::string clean_name(const std::string& name)
		{
			auto new_name = name;

			for (auto i = 0u; i < name.size(); i++)
			{
				switch (new_name[i])
				{
				case '*':
					new_name[i] = '_';
					break;
				}
			}

			return new_name;
		}
	}

	namespace
	{
		void add_loaded_image_flags(GfxImage* image)
		{
			if (image->levelCount <= 1)
			{
				image->flags |= 0x2;
			}

			if (image->mapType == MAPTYPE_1D)
			{
				//image->flags |= IMG_DISK_FLAG_MAPTYPE_1D;
			}
			else if (image->mapType == MAPTYPE_3D)
			{
				//image->flags |= IMG_DISK_FLAG_MAPTYPE_3D;
			}
			else if (image->mapType == MAPTYPE_CUBE)
			{
				//image->flags |= IMG_DISK_FLAG_MAPTYPE_CUBE;
			}
			else if (image->numElements > 1)
			{
				image->mapType = MAPTYPE_ARRAY;
			}
		}
	}

	namespace iwi
	{
		GfxImage* parse(const std::string& name, zone_memory* mem)
		{
			::iwi::GfxImage img_ = {};
			memset(&img_, 0, sizeof(::iwi::GfxImage));

			const auto ret_img_ = ::iwi::parse_iwi(name, mem, &img_);

			if (ret_img_)
			{
				auto* image = mem->allocate<GfxImage>();
				image->name = ret_img_->name;
				image->imageFormat = ret_img_->imageFormat;
				image->mapType = static_cast<MapType>(ret_img_->mapType);
				image->dataLen1 = ret_img_->dataLen;
				image->dataLen2 = ret_img_->dataLen;
				image->width = ret_img_->width;
				image->height = ret_img_->height;
				image->depth = ret_img_->depth;
				image->numElements = ret_img_->numElements;
				image->levelCount = ret_img_->levelCount;
				image->pixelData = ret_img_->pixelData;

				image->streamed = false;

				image->semantic = 2; // material changes this
				image->category = 3;

				add_loaded_image_flags(image);

				return image;
			}

			return nullptr;
		}
	}

	namespace directxtex
	{
		bool load_image(const std::string& name, DirectX::ScratchImage* image)
		{
			std::string c_name = clean_name(name);
			c_name = utils::string::va("images\\%s", c_name.data());
			if (filesystem::file(c_name + ".dds").exists())
			{
				// DDS Found
				c_name.append(".dds");
			}
			else if (filesystem::file(c_name + ".tga").exists())
			{
				// TGA Found
				c_name.append(".tga");
			}
			else
			{
				// No image found
				return false;
			}
			std::string path = filesystem::get_file_path(c_name) + c_name;

			std::wstring wname = utils::string::convert(path);

			HRESULT hr = E_FAIL;
			if (c_name.ends_with(".dds"))
			{
				hr = LoadFromDDSFile(wname.data(), DirectX::DDS_FLAGS_NONE, nullptr, *image);
			}
			else if (c_name.ends_with(".tga"))
			{
				hr = LoadFromTGAFile(wname.data(), nullptr, *image);
			}

			if (SUCCEEDED(hr))
			{
				return true;
			}
			return false;
		}

		GfxImage* parse(const std::string& name, zone_memory* mem)
		{
			DirectX::ScratchImage image;

			if (load_image(name, &image))
			{
				ZONETOOL_INFO("Parsing custom image \"%s\"", name.data());

				auto* pixels = image.GetPixels();
				auto pixels_size = image.GetPixelsSize();
				const auto& metadata = image.GetMetadata();

				auto* gfx_image = mem->allocate<GfxImage>();

				gfx_image->imageFormat = metadata.format;
				gfx_image->mapType = static_cast<MapType>(metadata.dimension);
				gfx_image->semantic = 2; // material changes this
				gfx_image->category = 3;
				gfx_image->width = static_cast<unsigned short>(metadata.width);
				gfx_image->height = static_cast<unsigned short>(metadata.height);
				gfx_image->depth = static_cast<unsigned short>(metadata.depth);
				gfx_image->numElements = static_cast<unsigned short>(metadata.arraySize);
				gfx_image->levelCount = static_cast<unsigned char>(metadata.mipLevels);
				gfx_image->streamed = 0;
				gfx_image->dataLen1 = static_cast<int>(pixels_size);
				gfx_image->dataLen2 = static_cast<int>(pixels_size);
				gfx_image->pixelData = mem->allocate<unsigned char>(pixels_size);
				memcpy(gfx_image->pixelData, pixels, pixels_size);
				gfx_image->name = mem->duplicate_string(name);

				if (metadata.IsCubemap())
				{
					gfx_image->mapType = MAPTYPE_CUBE;
					gfx_image->numElements = 1;
				}

				add_loaded_image_flags(gfx_image);

				return gfx_image;
			}

			return nullptr;
		}
	}

	GfxImage* gfx_image::parse_custom(const std::string& name, zone_memory* mem)
	{
		GfxImage* image = nullptr;
		image = directxtex::parse(name, mem);
		if (!image)
		{
			image = iwi::parse(name, mem);
			if (image)
			{
				this->is_iwi = true;
			}
		}

		return image;
	}

	std::optional<std::string> get_streamed_image_pixels(const std::string& name, int stream)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.pixels", 
			clean_name(name).data(), stream);
		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		
		if (utils::io::file_exists(full_path))
		{
			return {utils::io::read_file(full_path)};
		}

		return {};
	}

	bool get_streamed_image_dds(const std::string& name, int stream, DirectX::ScratchImage& image)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.dds",
			clean_name(name).data(), stream);

		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		const auto full_path_w = utils::string::convert(full_path);
		const auto result = DirectX::LoadFromDDSFile(full_path_w.data(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		return SUCCEEDED(result);
	}

	std::optional<std::string> get_streamed_image_dds(const std::string& name, int stream)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.dds",
			clean_name(name).data(), stream);

		DirectX::ScratchImage image{};

		const auto full_path = filesystem::get_file_path(image_path) + image_path;
		const auto full_path_w = utils::string::convert(full_path);
		const auto result = DirectX::LoadFromDDSFile(full_path_w.data(), DirectX::DDS_FLAGS_NONE, nullptr, image);
		if (!SUCCEEDED(result))
		{
			return {};
		}

		const auto pixel_data = image.GetPixels();
		const auto size = image.GetPixelsSize();

		return {{reinterpret_cast<const char*>(pixel_data), size}};
	}

	std::optional<std::string> get_streamed_image_pixels_path(const std::string& name, int stream)
	{
		const auto image_path = utils::string::va("streamed_images\\%s_stream%i.pixels",
			clean_name(name).data(), stream);
		const auto full_path = filesystem::get_file_path(image_path) + image_path;

		if (utils::io::file_exists(full_path))
		{
			return {full_path};
		}

		return {};
	}

	std::optional<std::string> get_streamed_image_data(const std::string& name, int stream)
	{
		const auto pixels = get_streamed_image_pixels(name, stream);
		if (pixels.has_value())
		{
			return pixels;
		}

		const auto dds = get_streamed_image_dds(name, stream);
		if (dds.has_value())
		{
			return dds;
		}

		return {};
	}

	GfxImage* gfx_image::parse_streamed_image(const std::string& name, zone_memory* mem)
	{
		const auto asset_path = utils::string::va("streamed_images\\%s.iw6Image", clean_name(name).data());

		assetmanager::reader read(mem);
		if (!read.open(asset_path))
		{
			return nullptr;
		}

		GfxImage* asset = read.read_single<GfxImage>();
		asset->name = read.read_string();

		ZONETOOL_INFO("Parsing streamed image \"%s\"...", name.data());

		this->custom_streamed_image = true;

		for (auto i = 0; i < 4; i++)
		{
			const auto result = get_streamed_image_pixels_path(name, i);
			if (!result.has_value())
			{
				continue;
			}

			const auto& path = result.value();
			this->image_stream_blocks_paths[i] = {path};
		}

		read.close();

		return asset;
	}

	GfxImage* gfx_image::parse(const std::string& name, zone_memory* mem)
	{
		auto path = "images\\" + clean_name(name) + ".iw6Image";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing image \"%s\"...", name.data());

		auto asset = read.read_single<GfxImage>();
		asset->name = read.read_string();
		if (asset->pixelData)
		{
			asset->pixelData = read.read_array<unsigned char>();
		}

		if (asset->streamed)
		{
			// read stream file
			for (auto i = 0u; i < 4; i++)
			{
				this->image_stream_files[i] = read.read_single<XStreamFile>();
			}
		}

		read.close();

		//memset(&asset->texture, 0, sizeof(asset->texture));

		return asset;
	}

	void gfx_image::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = this->parse_streamed_image(name, mem);
		if (this->asset_)
		{
			return;
		}

		this->asset_ = parse_custom(name.data(), mem);
		if (!this->asset_)
		{
			ZONETOOL_WARNING("Image \"%s\" not found, it will probably look messed up ingame!", name.data());

			static unsigned char default_pixel_data[4] = { 255, 0, 0, 255 };
			auto* image = mem->allocate<GfxImage>();
			image->imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			image->mapType = MAPTYPE_2D;
			image->semantic = 0;
			image->category = 1;
			image->flags = 0;
			image->dataLen1 = sizeof(default_pixel_data);
			image->dataLen2 = sizeof(default_pixel_data);
			image->width = 1;
			image->height = 1;
			image->pixelData = default_pixel_data;
			image->depth = 1;
			image->numElements = 1;
			image->levelCount = 1;
			image->streamed = 0;
			image->name = mem->duplicate_string(this->name_);

			this->asset_ = image;
		}
	}

	void gfx_image::init(void* asset, zone_memory* mem)
	{
		this->asset_ = reinterpret_cast<GfxImage*>(asset);
		this->name_ = this->asset_->name;

		auto parsed = this->parse(this->name_, mem);
		if (parsed)
		{
			this->asset_ = parsed;
		}
	}

	void gfx_image::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void gfx_image::load_depending(zone_base* zone)
	{
	}

	std::string gfx_image::name()
	{
		return this->name_;
	}

	std::int32_t gfx_image::type()
	{
		return ASSET_TYPE_IMAGE;
	}

	void gfx_image::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->semantic != 11)
		{
			buf->push_stream(XFILE_BLOCK_TEMP);
			if (data->pixelData)
			{
				buf->align(3);
				buf->write_stream(data->pixelData, data->dataLen1);
				buf->clear_pointer(&dest->pixelData);
			}
			buf->pop_stream();
		}
		else
		{
			buf->push_stream(XFILE_BLOCK_TEMP);
			if (data->pixelData)
			{
				buf->align(3);
				buf->write_stream(data->pixelData, 1);
				buf->clear_pointer(&dest->pixelData);
			}
			buf->pop_stream();
		}

		buf->pop_stream();

		if (data->streamed)
		{
			// add stream file to header
			for (auto i = 0u; i < 4; i++)
			{
				buf->write_streamfile(reinterpret_cast<std::uintptr_t>(this->image_stream_files[i]));
			}
		}
	}

	void dump_streamed_image(GfxImage* image, bool is_self = false, bool dump_dds = false)
	{
		for (auto i = 0u; i < 4; i++)
		{
			const auto stream_file = &stream_files[*stream_file_index + i];

			std::string filename = utils::string::va("imagefile%d.pak", stream_file->fileIndex);
			if (is_self)
			{
				filename = utils::string::va("%s.pak", filesystem::get_fastfile().data());
			}
			const auto folder = filesystem::get_zone_path(filename);

			const auto imagefile_path = utils::string::va("%s%s", folder.data(), filename.data());
			std::ifstream imagefile;
			imagefile.open(imagefile_path, std::ios::binary);

			if (!imagefile.is_open() || stream_file->offset == 0 || stream_file->offsetEnd == 0)
			{
				continue;
			}

			std::string buffer;
			const auto size = stream_file->offsetEnd - stream_file->offset;
			buffer.resize(size);
			imagefile.seekg(stream_file->offset);
			imagefile.read(buffer.data(), size);

			try
			{
				const auto get_pixel_size_for_stream = [](GfxImage* image, int stream) -> int
				{
					if (stream > 0)
					{
						return image->streams[stream].levelCountAndSize.pixelSize - image->streams[stream - 1].levelCountAndSize.pixelSize;
					}

					return image->streams[stream].levelCountAndSize.pixelSize;
				};
				const auto pixel_size = get_pixel_size_for_stream(image, i);

				std::string pixel_data;
				pixel_data.resize(pixel_size);

				uncompress((Bytef*)pixel_data.data(), (uLongf*)&pixel_size, (Bytef*)buffer.data(), (uLongf)buffer.size());

				const auto name = clean_name(image->name);

				{
					std::string parent_path = filesystem::get_dump_path() + "streamed_images\\";
					std::string raw_path = utils::string::va("%s%s_stream%i.pixels", parent_path.data(), name.data(), i);
					utils::io::write_file(raw_path, pixel_data, false);
				}

				if (dump_dds)
				{
					DirectX::Image img = {};

					img.width = image->streams[i].width;
					img.height = image->streams[i].height;
					img.pixels = reinterpret_cast<uint8_t*>(pixel_data.data());
					img.format = DXGI_FORMAT(image->imageFormat);

					size_t row_pitch{};
					size_t slice_pitch{};

					DirectX::ComputePitch(img.format, img.width, img.height, row_pitch, slice_pitch);

					img.rowPitch = row_pitch;
					img.slicePitch = slice_pitch;

					const auto parent_path = filesystem::get_dump_path() + "streamed_images\\";
					const std::string spath = utils::string::va("%s\\%s_stream%i.dds", parent_path.data(),
						name.data(), i);
					const std::wstring wpath(spath.begin(), spath.end());
					if (!std::filesystem::exists(parent_path))
					{
						std::filesystem::create_directories(parent_path);
					}

					auto result = DirectX::SaveToDDSFile(img, DirectX::DDS_FLAGS_NONE, wpath.data());
					if (FAILED(result))
					{
						ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
					}
				}
			}
			catch (...)
			{
				ZONETOOL_ERROR("Failed to dump streamed image \"%s\"", image->name);
			}
		}

		{
			const auto path = "streamed_images\\"s + clean_name(image->name) + ".h1Image"s;
			assetmanager::dumper write;
			if (!write.open(path))
			{
				return;
			}

			write.dump_single(image);
			write.dump_string(image->name);
			write.close();
		}
	}

	void dump_image_dds(GfxImage* image)
	{
		if (image->streamed)
		{
			dump_streamed_image(image, stream_files[*stream_file_index].fileIndex == 69, true);
			return;
		}

		auto* data = image->pixelData;
		std::size_t data_used = 0;

		const auto sides = image->mapType == MAPTYPE_CUBE ? 6 : 1;

		std::vector<DirectX::Image> images{};
		for (int idx = 0; idx < image->numElements; idx++)
		{
			for (int s = 0; s < sides; s++)
			{
				for (int d = 0; d < image->depth; d++)
				{
					for (int i = 0; i < image->levelCount; i++)
					{
						DirectX::Image img{};
						img.pixels = data;

						img.width = std::max(1, image->width >> i);
						img.height = std::max(1, image->height >> i);
						img.format = DXGI_FORMAT(image->imageFormat);

						size_t rowPitch = 0;
						size_t slicePitch = 0;
						DirectX::ComputePitch(img.format, img.width, img.height, rowPitch, slicePitch);

						img.rowPitch = rowPitch;
						img.slicePitch = slicePitch;

						images.push_back(img);

						data += slicePitch;
						data_used += slicePitch;
					}
				}
			}
		}

		if (data_used != image->dataLen1)
		{
			ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", image->name);
			return;
		}

		DirectX::TexMetadata mdata{};
		mdata.width = image->width;
		mdata.height = image->height;
		mdata.depth = image->depth;
		mdata.arraySize = image->numElements * sides;
		mdata.mipLevels = image->levelCount;
		mdata.format = DXGI_FORMAT(image->imageFormat);
		mdata.dimension = image->mapType > 4 ? DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D : (DirectX::TEX_DIMENSION)image->mapType;

		if (image->mapType == MAPTYPE_CUBE)
		{
			mdata.miscFlags |= DirectX::TEX_MISC_FLAG::TEX_MISC_TEXTURECUBE;
		}

		std::string parent_path = filesystem::get_dump_path() + "images\\";
		std::string spath = parent_path + clean_name(image->name) + ".dds";
		std::wstring wpath(spath.begin(), spath.end());

		if (!std::filesystem::exists(parent_path))
		{
			std::filesystem::create_directories(parent_path);
		}

		auto result = DirectX::SaveToDDSFile(images.data(), images.size(), mdata, DirectX::DDS_FLAGS_NONE, wpath.data());
		if (FAILED(result))
		{
			ZONETOOL_WARNING("Failed to dump image \"%s\"", spath.data());
		}
	}

	void gfx_image::dump(GfxImage* asset)
	{
		if (utils::flags::has_flag("dds"))
		{
			dump_image_dds(asset);
		}

		if (asset->streamed)
		{
			if (stream_files[*stream_file_index].fileIndex == 69)
			{
				dump_streamed_image(asset, true);
				return;
			}

			if (utils::flags::has_flag("dump_streamed_image"))
			{
				dump_streamed_image(asset, false);
				return;
			}
		}

		auto path = "images\\"s + clean_name(asset->name) + ".iw6Image"s;
		assetmanager::dumper write;
		if (!write.open(path))
		{
			return;
		}

		if (asset->semantic == 11)
		{
			ZONETOOL_WARNING("GfxImage (%s) was not dumped succesfully!", asset->name);
			write.close();
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);

		if (asset->pixelData)
		{
			write.dump_array(asset->pixelData, asset->dataLen1);
		}

		if (asset->streamed)
		{
			for (auto i = 0u; i < 4; i++)
			{
				const auto stream_file = &stream_files[*stream_file_index + i];
				write.dump_single(stream_file);
			}
		}
		write.close();
	}
}