#include "std_include.hpp"
#include "clut.hpp"

#pragma warning( push )
#pragma warning( disable : 4459 )
#include <DirectXTex.h>
#pragma warning( pop )

#define min(a,b)            (((a) < (b)) ? (a) : (b))
#define max(a,b)            (((a) > (b)) ? (a) : (b))

namespace zonetool::h1
{
	void dump_clut_ass(Clut* asset)
	{
		const auto path = "clut\\"s + asset->name + ".clut";

		assetmanager::dumper dumper;
		if (!dumper.open(path))
		{
			return;
		}

		dumper.dump_single(asset);
		dumper.dump_string(asset->name);
		dumper.dump_array(asset->pixels, 4 * asset->width * asset->height * asset->depth);

		dumper.close();
	}

	namespace directx
	{
		bool load_image(const std::string& name, DirectX::ScratchImage* image)
		{
			std::string c_name = name;
			c_name = utils::string::va("clut\\%s", c_name.data());

			// append suffixes
			if (filesystem::file(c_name + ".dds").exists())
				c_name.append(".dds"); // DDS Found
			else if (filesystem::file(c_name + ".tga").exists())
				c_name.append(".tga"); // TGA Found
			else if (filesystem::file(c_name + ".png").exists())
				c_name.append(".png"); // PNG Found
			else
				return false; // No image found

			std::string path = filesystem::get_file_path(c_name) + c_name;

			std::wstring wname = utils::string::convert(path);

			HRESULT hr = E_FAIL;
			if (c_name.ends_with(".dds"))
				hr = LoadFromDDSFile(wname.data(), DirectX::DDS_FLAGS_NONE, nullptr, *image);
			else if (c_name.ends_with(".tga"))
				hr = LoadFromTGAFile(wname.data(), nullptr, *image);
			else if (c_name.ends_with(".png"))
				hr = LoadFromWICFile(wname.data(), DirectX::WIC_FLAGS_NONE, nullptr, *image);

			return SUCCEEDED(hr);
		}

		Clut* parse(const std::string& name, zone_memory* mem)
		{
			DirectX::ScratchImage image;

			if (load_image(name, &image))
			{
				ZONETOOL_INFO("Parsing clut image \"%s\"", name.data());

				auto* pixels = image.GetPixels();
				auto pixels_size = image.GetPixelsSize();
				const auto& metadata = image.GetMetadata();

				assert(metadata.format == DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);

				auto* clut = mem->allocate<Clut>();
				clut->width = static_cast<unsigned short>(metadata.width);
				clut->height = static_cast<unsigned short>(metadata.height);
				clut->depth = static_cast<unsigned short>(metadata.depth);
				assert(metadata.arraySize == 1);
				assert(metadata.mipLevels == 1);
				clut->pixels = mem->allocate<unsigned char>(pixels_size);
				memcpy(clut->pixels, pixels, pixels_size);
				clut->name = mem->duplicate_string(name);

				return clut;
			}

			return nullptr;
		}

		void dump_clut_image_dds(Clut* asset)
		{
			const auto width = asset->width * asset->depth;
			const auto height = asset->height;
			const auto depth = asset->depth;
			unsigned char* pixels = reinterpret_cast<unsigned char*>(asset->pixels);
			const auto dataLen = 4 * width * height * depth;

			auto* data = pixels;
			std::size_t data_used = 0;

			const auto sides = 1;

			std::vector<DirectX::Image> images{};
			for (int idx = 0; idx < 1; idx++)
			{
				for (int s = 0; s < sides; s++)
				{
					for (unsigned int d = 0; d < depth; d++)
					{
						for (int i = 0; i < 1; i++)
						{
							DirectX::Image img{};
							img.pixels = data;

							img.width = max(1, width >> i);
							img.height = max(1, height >> i);
							img.format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;

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

			if (data_used != dataLen)
			{
				ZONETOOL_WARNING("Failed to dump image \"%s.dds\"", asset->name);
				dump_clut_ass(asset);
				return;
			}

			DirectX::TexMetadata mdata{};
			mdata.width = width;
			mdata.height = height;
			mdata.depth = depth;
			mdata.arraySize = 1 * sides;
			mdata.mipLevels = 1;
			mdata.format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM;
			mdata.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;

			std::string parent_path = filesystem::get_dump_path() + "clut\\";
			std::string spath = parent_path + asset->name + ".dds";
			std::wstring wpath(spath.begin(), spath.end());

			if (!std::filesystem::exists(parent_path))
			{
				std::filesystem::create_directories(parent_path);
			}

			auto result = DirectX::SaveToDDSFile(images.data(), images.size(), mdata, DirectX::DDS_FLAGS_NONE, wpath.data());
			if (FAILED(result))
			{
				ZONETOOL_WARNING("Failed to dump image \"%s\"", spath.data());
				dump_clut_ass(asset);
			}
		}
	}

	Clut* clut::parse(const std::string& name, zone_memory* mem)
	{
		if (auto* asset = directx::parse(name, mem))
		{
			return asset;
		}

		assetmanager::reader read(mem);

		const auto path = "clut\\"s + name + ".clut"s;
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing clut \"%s\"...", name.data());

		const auto asset = read.read_single<Clut>();
		asset->name = read.read_string();
		asset->pixels = read.read_array<unsigned char>();

		return asset;
	}

	void clut::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).clut;
		}
	}

	void clut::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void clut::load_depending(zone_base* zone)
	{
	}

	std::string clut::name()
	{
		return this->name_;
	}

	std::int32_t clut::type()
	{
		return ASSET_TYPE_CLUT;
	}

	void clut::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		if (data->pixels)
		{
			buf->align(0);
			buf->write(data->pixels, 4 * data->width * data->height * data->depth);
			buf->clear_pointer(&dest->pixels);
		}

		dest->name = buf->write_str(this->name());

		buf->pop_stream();
	}

	void clut::dump(Clut* asset)
	{
		directx::dump_clut_image_dds(asset);
	}
}