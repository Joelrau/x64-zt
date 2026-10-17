#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <io/output_cache.hpp>
#include <utils/io.hpp>

#include <DirectXTex.h>

namespace zonetool::s2::converter::h1::gfximage
{
	namespace
	{
		constexpr DXGI_FORMAT image_formats[]
		{
			DXGI_FORMAT_UNKNOWN,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_A8_UNORM,
			DXGI_FORMAT_R8_UNORM,
			DXGI_FORMAT_R8G8_UNORM,
			DXGI_FORMAT_R8G8_SNORM,
			DXGI_FORMAT_BC1_UNORM,
			DXGI_FORMAT_BC1_UNORM,
			DXGI_FORMAT_BC2_UNORM,
			DXGI_FORMAT_BC2_UNORM,
			DXGI_FORMAT_BC3_UNORM,
			DXGI_FORMAT_BC3_UNORM,
			DXGI_FORMAT_BC4_UNORM,
			DXGI_FORMAT_BC5_UNORM,
			DXGI_FORMAT_BC5_SNORM,
			DXGI_FORMAT_BC6H_UF16,
			DXGI_FORMAT_BC6H_SF16,
			DXGI_FORMAT_BC7_UNORM,
			DXGI_FORMAT_BC7_UNORM,
			DXGI_FORMAT_R16G16B16A16_UNORM,
			DXGI_FORMAT_R16G16_UNORM,
			DXGI_FORMAT_R16G16_SNORM,
			DXGI_FORMAT_R16_UNORM,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			DXGI_FORMAT_R32G32_FLOAT,
			DXGI_FORMAT_R16G16_FLOAT,
			DXGI_FORMAT_R32_FLOAT,
			DXGI_FORMAT_R16_FLOAT,
			DXGI_FORMAT_R11G11B10_FLOAT,
			DXGI_FORMAT_R10G10B10A2_UNORM,
			DXGI_FORMAT_D16_UNORM,
			DXGI_FORMAT_D32_FLOAT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D24_UNORM_S8_UINT,
			DXGI_FORMAT_D32_FLOAT_S8X24_UINT,
			DXGI_FORMAT_B5G6R5_UNORM,
			DXGI_FORMAT_R8_UINT,
			DXGI_FORMAT_R16_UINT,
			DXGI_FORMAT_R32G32_UINT,
			DXGI_FORMAT_R32G32B32A32_UINT,
		};


		std::unordered_set<std::string> dumped_images;

		fingerprint get_image_fingerprint(const GfxImage* asset)
		{
			fingerprint result;
			result.add_value(cache_revision::s2_h1_image).add(asset->name).add_value(asset->imageFormat).add_value(asset->mapType)
				.add_value(asset->semantic).add_value(asset->category).add_value(asset->flags).add_value(asset->width)
				.add_value(asset->height).add_value(asset->depth).add_value(asset->numElements).add_value(asset->levelCount)
				.add_value(asset->streams);
			for (const auto& key : get_image_stream_keys(asset))
			{
				result.add_value(key);
			}

			return result;
		}

		std::string clean_name(std::string name)
		{
			std::replace(name.begin(), name.end(), '*', '_');
			return name;
		}

		DXGI_FORMAT convert_format(const DXGI_FORMAT format)
		{
			const auto index = static_cast<std::size_t>(format);
			return index < std::size(image_formats) ? image_formats[index] : DXGI_FORMAT_UNKNOWN;
		}

		zonetool::h1::IMG_TS convert_semantic(const std::uint8_t semantic)
		{
			if (semantic == TS_EMISSIVE_MAP)
			{
				return zonetool::h1::TS_COLOR_MAP;
			}

			if (semantic >= zonetool::h1::TS_COUNT || semantic == zonetool::h1::TS_WATER_MAP)
			{
				return zonetool::h1::TS_2D;
			}

			return static_cast<zonetool::h1::IMG_TS>(semantic);
		}

		bool is_packed_specular(const GfxImage* asset)
		{
			return asset->semantic == TS_SPECULAR_MAP && DirectX::IsCompressed(convert_format(asset->imageFormat));
		}

		void unpack_specular(const DirectX::Image& image)
		{
			for (auto y = 0u; y < image.height; y++)
			{
				const auto row = image.pixels + y * image.rowPitch;
				for (auto x = 0u; x < image.width; x++)
				{
					const auto pixel = row + x * 4;
					const auto occlusion = pixel[1] / 255.0f;
					const auto color = static_cast<std::uint8_t>(std::min(255.0f, pixel[0] * (32.0f / 49.0f) * occlusion + 0.5f));
					pixel[3] = pixel[2];
					pixel[0] = color;
					pixel[1] = color;
					pixel[2] = color;
				}
			}
		}

		std::optional<std::string> convert_specular_levels(const std::string& pixels, const DXGI_FORMAT format, std::size_t width, std::size_t height)
		{
			std::string result;
			auto offset = 0ull;
			while (offset < pixels.size())
			{
				std::size_t row_pitch{};
				std::size_t slice_pitch{};
				if (FAILED(DirectX::ComputePitch(format, width, height, row_pitch, slice_pitch)) || offset + slice_pitch > pixels.size())
				{
					return {};
				}

				const DirectX::Image source{width, height, format, row_pitch, slice_pitch,
					reinterpret_cast<std::uint8_t*>(const_cast<char*>(pixels.data() + offset))};

				DirectX::ScratchImage rgba;
				DirectX::ScratchImage compressed;
				if (FAILED(DirectX::Decompress(source, DXGI_FORMAT_R8G8B8A8_UNORM, rgba)))
				{
					return {};
				}

				unpack_specular(*rgba.GetImage(0, 0, 0));
				if (FAILED(DirectX::Compress(*rgba.GetImage(0, 0, 0), DXGI_FORMAT_BC3_UNORM, DirectX::TEX_COMPRESS_DEFAULT,
					DirectX::TEX_THRESHOLD_DEFAULT, compressed)))
				{
					return {};
				}

				result.append(reinterpret_cast<const char*>(compressed.GetPixels()), compressed.GetPixelsSize());
				offset += slice_pitch;
				width = std::max(1ull, width / 2);
				height = std::max(1ull, height / 2);
			}

			return result;
		}

		bool is_streamed(const GfxImage* asset)
		{
			return asset->streams[0].width != 0;
		}

		std::unordered_map<std::string, std::string> sky_cubes;

		std::optional<DirectX::ScratchImage> read_sky_pixels(GfxImage* asset)
		{
			const auto format = convert_format(asset->imageFormat);
			std::size_t width = asset->width;
			std::size_t height = asset->height;
			std::optional<std::string> pixels;
			if (is_streamed(asset))
			{
				const auto keys = get_image_stream_keys(asset);
				for (auto i = 0u; i < keys.size() && i < std::size(asset->streams); i++)
				{
					if (asset->streams[i].width)
					{
						pixels = read_image_stream(keys[i]);
						width = asset->streams[i].width;
						height = asset->streams[i].height;
					}
				}
			}
			else if (asset->pixelData)
			{
				pixels.emplace(reinterpret_cast<const char*>(asset->pixelData), asset->dataLen1);
			}

			std::size_t row_pitch{};
			std::size_t slice_pitch{};
			if (!pixels || FAILED(DirectX::ComputePitch(format, width, height, row_pitch, slice_pitch)) || pixels->size() < slice_pitch)
			{
				return {};
			}

			const DirectX::Image source{width, height, format, row_pitch, slice_pitch,
				reinterpret_cast<std::uint8_t*>(pixels->data())};

			DirectX::ScratchImage result;
			const auto hr = DirectX::IsCompressed(format)
				? DirectX::Decompress(source, DXGI_FORMAT_R32G32B32A32_FLOAT, result)
				: DirectX::Convert(source, DXGI_FORMAT_R32G32B32A32_FLOAT, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, result);
			if (FAILED(hr))
			{
				return {};
			}

			return result;
		}

		std::array<float, 3> sample_bilinear(const DirectX::Image& image, const float u, const float v)
		{
			const auto x = std::clamp(u * image.width - 0.5f, 0.0f, image.width - 1.0f);
			const auto y = std::clamp(v * image.height - 0.5f, 0.0f, image.height - 1.0f);
			const auto x0 = static_cast<std::size_t>(x);
			const auto y0 = static_cast<std::size_t>(y);
			const auto x1 = std::min(x0 + 1, image.width - 1);
			const auto y1 = std::min(y0 + 1, image.height - 1);
			const auto fx = x - x0;
			const auto fy = y - y0;

			const auto texel = [&](const std::size_t tx, const std::size_t ty)
			{
				return reinterpret_cast<const float*>(image.pixels + ty * image.rowPitch) + tx * 4;
			};

			std::array<float, 3> result{};
			for (auto c = 0; c < 3; c++)
			{
				const auto top = texel(x0, y0)[c] * (1 - fx) + texel(x1, y0)[c] * fx;
				const auto bottom = texel(x0, y1)[c] * (1 - fx) + texel(x1, y1)[c] * fx;
				result[c] = top * (1 - fy) + bottom * fy;
			}

			return result;
		}

		std::optional<DirectX::ScratchImage> compress_bc6h_cube(const DirectX::ScratchImage& cube, const std::size_t face_size)
		{
			constexpr std::size_t strip_height = 16;

			DirectX::ScratchImage compressed;
			if (FAILED(compressed.InitializeCube(DXGI_FORMAT_BC6H_UF16, face_size, face_size, 1, 1)))
			{
				return {};
			}

			struct strip
			{
				std::size_t face;
				std::size_t y;
				std::size_t height;
			};

			std::vector<strip> strips;
			for (auto face = 0u; face < 6; face++)
			{
				for (auto y = 0ull; y < face_size; y += strip_height)
				{
					strips.emplace_back(face, y, std::min(strip_height, face_size - y));
				}
			}

			std::atomic_size_t next_strip{};
			std::atomic_bool failed{};
			const auto compress_strips = [&]
			{
				for (auto index = next_strip++; index < strips.size(); index = next_strip++)
				{
					const auto& [face, y, height] = strips[index];
					const auto& source = *cube.GetImage(0, face, 0);
					const DirectX::Image part{face_size, height, source.format, source.rowPitch, source.rowPitch * height,
						source.pixels + y * source.rowPitch};

					DirectX::ScratchImage result;
					if (FAILED(DirectX::Compress(part, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, result)))
					{
						failed = true;
						continue;
					}

					const auto& target = *compressed.GetImage(0, face, 0);
					std::memcpy(target.pixels + y / 4 * target.rowPitch, result.GetPixels(), result.GetPixelsSize());
				}
			};

			std::vector<std::thread> threads;
			for (auto i = 0u; i < std::max(1u, std::thread::hardware_concurrency()); i++)
			{
				threads.emplace_back(compress_strips);
			}

			for (auto& thread : threads)
			{
				thread.join();
			}

			if (failed)
			{
				return {};
			}

			return compressed;
		}

		std::array<float, 3> cube_direction(const std::size_t face, const float u, const float v)
		{
			switch (face)
			{
			case 0: return {1.0f, -v, -u};
			case 1: return {-1.0f, -v, u};
			case 2: return {u, 1.0f, v};
			case 3: return {u, -1.0f, -v};
			case 4: return {u, -v, 1.0f};
			default: return {-u, -v, -1.0f};
			}
		}

		// S2 sky_c.hlsl rotates the view direction by skyRotationParms, then samples a 2D image with uv = 0.5 + 0.5 * dir.xy / (1 + |0.080169 + 0.919831 * dir.z|)
		std::optional<DirectX::ScratchImage> make_sky_cube(const DirectX::ScratchImage& source, const float rotation_cos, const float rotation_sin)
		{
			const auto& image = *source.GetImage(0, 0, 0);
			const auto face_size = std::min<std::size_t>(1024, std::max<std::size_t>(1, image.width / 2));

			DirectX::ScratchImage cube;
			if (FAILED(cube.InitializeCube(DXGI_FORMAT_R32G32B32A32_FLOAT, face_size, face_size, 1, 1)))
			{
				return {};
			}

			for (auto face = 0u; face < 6; face++)
			{
				const auto& target = *cube.GetImage(0, face, 0);
				for (auto y = 0u; y < face_size; y++)
				{
					const auto row = reinterpret_cast<float*>(target.pixels + y * target.rowPitch);
					for (auto x = 0u; x < face_size; x++)
					{
						const auto u = (x + 0.5f) / face_size * 2.0f - 1.0f;
						const auto v = (y + 0.5f) / face_size * 2.0f - 1.0f;
						const auto dir = cube_direction(face, u, v);
						const auto length = std::sqrt(dir[0] * dir[0] + dir[1] * dir[1] + dir[2] * dir[2]);
						const auto sx = (dir[0] * rotation_sin - dir[1] * rotation_cos) / length;
						const auto sy = (dir[1] * rotation_sin + dir[0] * rotation_cos) / length;
						const auto sz = dir[2] / length;
						const auto scale = 0.5f / (1.0f + std::abs(0.080169f + 0.919831f * sz));
						const auto color = sample_bilinear(image, 0.5f + sx * scale, 0.5f + sy * scale);
						std::memcpy(row + x * 4, color.data(), sizeof(color));
						row[x * 4 + 3] = 1.0f;
					}
				}
			}

			return compress_bc6h_cube(cube, face_size);
		}

		zonetool::h1::GfxImage convert(GfxImage* asset)
		{
			zonetool::h1::GfxImage image{};
			image.imageFormat = convert_format(asset->imageFormat);
			image.mapType = static_cast<zonetool::h1::MapType>(asset->mapType);
			image.semantic = convert_semantic(asset->semantic);
			image.category = asset->category;
			image.flags = asset->flags & 0x7F;
			image.width = asset->width;
			image.height = asset->height;
			image.depth = asset->depth;
			image.numElements = asset->numElements;
			image.levelCount = asset->levelCount;
			image.name = asset->name;

			static_assert(sizeof(image.streams) == sizeof(asset->streams));
			std::memcpy(image.streams, asset->streams, sizeof(image.streams));

			if (is_streamed(asset))
			{
				image.streamed = 1;
				for (const auto& stream : asset->streams)
				{
					if (stream.width)
					{
						image.dataLen2 = stream.levelCountAndSize.pixelSize;
					}
				}
			}
			else
			{
				image.dataLen1 = asset->dataLen1;
				image.dataLen2 = asset->dataLen1;
				image.pixelData = asset->pixelData;
			}

			return image;
		}

		void dump_header(const std::string& path, zonetool::h1::GfxImage* image)
		{
			assetmanager::dumper write;
			if (!write.open(path))
			{
				return;
			}

			write.dump_single(image);
			write.dump_string(image->name);
			if (image->pixelData)
			{
				write.dump_array(image->pixelData, image->dataLen1);
			}

			write.close();
		}

		// stream pixelSize is cumulative over the streams, dataLen2 is the total
		template <std::size_t N>
		void convert_specular_streams(GfxImage* asset, zonetool::h1::GfxImage* image, std::array<std::optional<std::string>, N>& streams)
		{
			std::array<std::optional<std::string>, N> converted;
			for (auto i = 0u; i < N; i++)
			{
				if (!streams[i])
				{
					continue;
				}

				converted[i] = convert_specular_levels(*streams[i], image->imageFormat, asset->streams[i].width, asset->streams[i].height);
				if (!converted[i])
				{
					ZONETOOL_WARNING("Failed to convert specular image \"%s\"", asset->name);
					return;
				}
			}

			auto total = 0u;
			for (auto i = 0u; i < N; i++)
			{
				if (converted[i])
				{
					total += static_cast<unsigned int>(converted[i]->size());
					image->streams[i].levelCountAndSize.pixelSize = static_cast<int>(total);
					streams[i] = std::move(converted[i]);
				}
			}

			image->imageFormat = DXGI_FORMAT_BC3_UNORM;
			image->dataLen2 = total;
		}

		void dump_streams(GfxImage* asset, zonetool::h1::GfxImage* image)
		{
			const auto keys = get_image_stream_keys(asset);
			std::array<std::optional<std::string>, std::extent_v<decltype(GfxImage::streams)>> streams;
			for (auto i = 0u; i < keys.size() && i < streams.size(); i++)
			{
				if (!asset->streams[i].width)
				{
					continue;
				}

				streams[i] = read_image_stream(keys[i]);
				if (!streams[i])
				{
					ZONETOOL_WARNING("Stream %u of image \"%s\" is not in the image files", i, asset->name);
				}
			}

			if (is_packed_specular(asset))
			{
				convert_specular_streams(asset, image, streams);
			}

			const auto name = clean_name(asset->name);
			for (auto i = 0u; i < streams.size(); i++)
			{
				if (streams[i])
				{
					const auto path = std::format("{}streamed_images\\{}_stream{}.pixels", filesystem::get_dump_path(), name, i);
					utils::io::write_file(path, *streams[i], false);
				}
			}
		}
	}

	void dump(GfxImage* asset)
	{
		dumped_images.insert(asset->name);
		add_csv_row("image", asset->name);

		auto image = convert(asset);
		const auto name = clean_name(asset->name);

		if (image.streamed)
		{
			const auto path = "streamed_images\\" + name + ".h1Image";
			const auto output = filesystem::get_dump_path() + path;
			const auto fingerprint = get_image_fingerprint(asset).value();
			if (is_dump_current(output, fingerprint))
			{
				return;
			}

			dump_streams(asset, &image);
			dump_header(path, &image);
			update_dump_cache(output, fingerprint);
			return;
		}

		std::optional<std::string> pixels;
		if (is_packed_specular(asset) && image.pixelData)
		{
			pixels = convert_specular_levels({reinterpret_cast<const char*>(image.pixelData), image.dataLen1}, image.imageFormat, image.width, image.height);
			if (pixels)
			{
				image.imageFormat = DXGI_FORMAT_BC3_UNORM;
				image.pixelData = reinterpret_cast<unsigned char*>(pixels->data());
				image.dataLen1 = static_cast<unsigned int>(pixels->size());
				image.dataLen2 = image.dataLen1;
			}
		}

		dump_header("images\\" + name + ".h1Image", &image);
	}

	std::string dump_sky_cube(GfxImage* asset, const float rotation_cos, const float rotation_sin, const int rotation_degrees)
	{
		const auto name = std::format("{}_cube{}", clean_name(asset->name), rotation_degrees);
		if (dumped_images.contains(name))
		{
			return name;
		}

		const auto path = "images\\" + name + ".h1Image";
		const auto output = filesystem::get_dump_path() + path;
		const auto fingerprint = get_image_fingerprint(asset).add_value(rotation_cos).add_value(rotation_sin).value();
		if (is_dump_current(output, fingerprint))
		{
			dumped_images.insert(name);
			sky_cubes.emplace(asset->name, name);
			add_csv_row("image", name);
			return name;
		}

		const auto pixels = read_sky_pixels(asset);
		if (!pixels)
		{
			ZONETOOL_WARNING("Failed to read sky image \"%s\"", asset->name);
			return asset->name;
		}

		auto cube = make_sky_cube(*pixels, rotation_cos, rotation_sin);
		if (!cube)
		{
			ZONETOOL_WARNING("Failed to convert sky image \"%s\"", asset->name);
			return asset->name;
		}

		zonetool::h1::GfxImage image{};
		image.imageFormat = DXGI_FORMAT_BC6H_UF16;
		image.mapType = zonetool::h1::MAPTYPE_CUBE;
		image.semantic = zonetool::h1::TS_COLOR_MAP;
		image.category = 3;
		image.flags = 2;
		image.width = static_cast<unsigned short>(cube->GetMetadata().width);
		image.height = static_cast<unsigned short>(cube->GetMetadata().height);
		image.depth = 1;
		image.numElements = 1;
		image.levelCount = 1;
		image.dataLen1 = static_cast<unsigned int>(cube->GetPixelsSize());
		image.dataLen2 = image.dataLen1;
		image.pixelData = cube->GetPixels();
		image.name = name.data();

		dump_header(path, &image);
		update_dump_cache(output, fingerprint);
		dumped_images.insert(name);
		sky_cubes.emplace(asset->name, name);
		add_csv_row("image", name);
		return name;
	}

	std::optional<std::string> find_sky_cube(const std::string& name)
	{
		const auto cube = sky_cubes.find(name);
		if (cube == sky_cubes.end())
		{
			return {};
		}

		return cube->second;
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_images.contains(name);
	}
}
