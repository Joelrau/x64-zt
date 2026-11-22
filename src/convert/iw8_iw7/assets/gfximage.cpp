#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <DirectXTex.h>

#include <io/output_cache.hpp>
#include <utils/flags.hpp>
#include <utils/io.hpp>

namespace zonetool::iw8::converter::iw7::gfximage
{
	namespace
	{
		using pixel_transform = std::function<std::optional<std::string>(std::string_view pixels, std::uint32_t first_level, std::uint32_t level_count)>;
		using channel_tables = std::array<std::array<std::uint8_t, 256>, 4>;
		using compress_strip = std::pair<std::size_t, std::size_t>;
		using color = std::array<float, 3>;

		std::string clean_name(std::string name)
		{
			std::ranges::replace(name, '*', '_');
			return name;
		}

		zonetool::iw7::MapType convert_map_type(const std::uint32_t flags)
		{
			switch (flags & IMG_DISK_FLAG_MAPTYPE_MASK)
			{
			case IMG_DISK_FLAG_MAPTYPE_CUBE:
				return zonetool::iw7::MAPTYPE_CUBE;
			case IMG_DISK_FLAG_MAPTYPE_3D:
				return zonetool::iw7::MAPTYPE_3D;
			case IMG_DISK_FLAG_MAPTYPE_1D:
				return zonetool::iw7::MAPTYPE_1D;
			case IMG_DISK_FLAG_MAPTYPE_ARRAY:
				return zonetool::iw7::MAPTYPE_ARRAY;
			case IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY:
				return zonetool::iw7::MAPTYPE_CUBE_ARRAY;
			default:
				return zonetool::iw7::MAPTYPE_2D;
			}
		}

		zonetool::iw7::TextureSemantic convert_semantic(const TextureSemantic semantic)
		{
			switch (semantic)
			{
			case TS_FUNCTION:
				return zonetool::iw7::TS_FUNCTION;
			case TS_COLOR_MAP:
				return zonetool::iw7::TS_COLOR_MAP;
			case TS_NORMAL_MAP:
			case TS_CARD_IMPOSTER_NORMAL:
				return zonetool::iw7::TS_NORMAL_MAP;
			case TS_NORMAL_OCCLUSION_GLOSS_MAP:
				return zonetool::iw7::TS_NORMAL_OCCLUSSION_GLOSS_MAP;
			default:
				return zonetool::iw7::TS_2D;
			}
		}

		zonetool::iw7::GfxImage convert(const GfxImage* asset)
		{
			zonetool::iw7::GfxImage image{};
			image.imageFormat = get_dxgi_format(asset->format);
			image.mapType = convert_map_type(asset->flags);
			image.semantic = convert_semantic(asset->semantic);
			image.category = static_cast<zonetool::iw7::GfxImageCategory>(asset->category);
			image.width = asset->width;
			image.height = asset->height;
			image.depth = asset->depth;
			image.numElements = asset->numElements;
			image.levelCount = asset->levelCount;
			image.name = asset->name;
			return image;
		}

		std::array<float, 3> get_cube_direction(const std::size_t face, const float s, const float t)
		{
			switch (face)
			{
			case 0:
				return {1.0f, -t, -s};
			case 1:
				return {-1.0f, -t, s};
			case 2:
				return {s, 1.0f, t};
			case 3:
				return {s, -1.0f, -t};
			case 4:
				return {s, -t, 1.0f};
			default:
				return {-s, -t, -1.0f};
			}
		}

		std::array<float, 2> encode_octahedron(const std::array<float, 3>& direction) // TODO: check the IW8 octahedron axes in the game
		{
			const auto length = std::abs(direction[0]) + std::abs(direction[1]) + std::abs(direction[2]);
			auto x = direction[0] / length;
			auto y = direction[1] / length;
			if (direction[2] < 0.0f)
			{
				const auto folded_x = (1.0f - std::abs(y)) * (x >= 0.0f ? 1.0f : -1.0f);
				const auto folded_y = (1.0f - std::abs(x)) * (y >= 0.0f ? 1.0f : -1.0f);
				x = folded_x;
				y = folded_y;
			}

			return {x * 0.5f + 0.5f, y * 0.5f + 0.5f};
		}

		DirectX::XMVECTOR sample_bilinear(const DirectX::Image& image, const std::array<float, 2>& uv)
		{
			const auto x = std::clamp(uv[0] * image.width - 0.5f, 0.0f, static_cast<float>(image.width - 1));
			const auto y = std::clamp(uv[1] * image.height - 0.5f, 0.0f, static_cast<float>(image.height - 1));
			const auto x0 = static_cast<std::size_t>(x);
			const auto y0 = static_cast<std::size_t>(y);
			const auto x1 = std::min(x0 + 1, image.width - 1);
			const auto y1 = std::min(y0 + 1, image.height - 1);

			const auto texel = [&](const std::size_t column, const std::size_t row)
			{
				const auto* line = reinterpret_cast<const DirectX::XMFLOAT4*>(image.pixels + row * image.rowPitch);
				return DirectX::XMLoadFloat4(&line[column]);
			};

			const auto top = DirectX::XMVectorLerp(texel(x0, y0), texel(x1, y0), x - x0);
			const auto bottom = DirectX::XMVectorLerp(texel(x0, y1), texel(x1, y1), x - x0);
			return DirectX::XMVectorLerp(top, bottom, y - y0);
		}

		void write_cube_face(const DirectX::Image& source, const DirectX::Image& face_image, const std::size_t face)
		{
			for (auto row = 0u; row < face_image.height; row++)
			{
				auto* line = reinterpret_cast<DirectX::XMFLOAT4*>(face_image.pixels + row * face_image.rowPitch);
				for (auto column = 0u; column < face_image.width; column++)
				{
					const auto s = (column + 0.5f) * 2.0f / face_image.width - 1.0f;
					const auto t = (row + 0.5f) * 2.0f / face_image.height - 1.0f;
					DirectX::XMStoreFloat4(&line[column], sample_bilinear(source, encode_octahedron(get_cube_direction(face, s, t))));
				}
			}
		}

		std::optional<DirectX::ScratchImage> compress(const DirectX::ScratchImage& source, const DXGI_FORMAT format, const DirectX::TEX_COMPRESS_FLAGS flags)
		{
			auto metadata = source.GetMetadata();
			metadata.format = format;

			DirectX::ScratchImage compressed;
			if (FAILED(compressed.Initialize(metadata)))
			{
				return {};
			}

			std::vector<compress_strip> strips;
			for (auto index = 0u; index < source.GetImageCount(); index++)
			{
				for (auto row = 0u; row < source.GetImages()[index].height; row += 64)
				{
					strips.emplace_back(index, row);
				}
			}

			std::atomic_size_t next_strip{};
			std::atomic_bool failed{};
			const auto compress_images = [&]
			{
				for (auto strip = next_strip++; strip < strips.size(); strip = next_strip++)
				{
					const auto [index, row] = strips[strip];
					const auto& image = source.GetImages()[index];
					auto part = image;
					part.height = std::min<std::size_t>(64, image.height - row);
					part.slicePitch = image.rowPitch * part.height;
					part.pixels = image.pixels + image.rowPitch * row;

					DirectX::ScratchImage result;
					if (FAILED(DirectX::Compress(part, format, flags, DirectX::TEX_THRESHOLD_DEFAULT, result)))
					{
						failed = true;
						continue;
					}

					const auto& target = compressed.GetImages()[index];
					std::memcpy(target.pixels + target.rowPitch * (row / 4), result.GetPixels(), result.GetPixelsSize());
				}
			};

			std::vector<std::jthread> threads(utils::flags::get_thread_count());
			for (auto& thread : threads)
			{
				thread = std::jthread(compress_images);
			}

			threads.clear();
			if (failed)
			{
				return {};
			}

			return compressed;
		}

		std::uint32_t get_stream_first_level(const GfxImage* asset, const int stream)
		{
			return asset->levelCount - asset->streams[stream].levelCount;
		}

		std::uint32_t get_stream_level_count(const GfxImage* asset, const int stream)
		{
			const auto smaller_levels = stream > 0 ? asset->streams[stream - 1].levelCount : 0u;
			return asset->streams[stream].levelCount - smaller_levels;
		}

		void dump_header(const std::string& path, zonetool::iw7::GfxImage* image)
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

		bool dump_streamed(const GfxImage* asset, zonetool::iw7::GfxImage& image, const pixel_transform& transform)
		{
			const auto name = clean_name(image.name);

			image.streamed = 1;
			std::uint32_t total_size{};
			for (auto i = 0; i < static_cast<int>(std::size(asset->streams)); i++)
			{
				const auto& stream = asset->streams[i];
				if (!stream.xpakEntry.key)
				{
					break;
				}

				const auto first_level = get_stream_first_level(asset, i);
				const auto level_count = get_stream_level_count(asset, i);
				const auto pixels = get_image_stream_pixels(asset, i);
				const auto tight_pixels = pixels ? remove_level_padding(asset, *pixels, first_level, level_count) : std::nullopt;
				const auto converted = tight_pixels ? transform(*tight_pixels, first_level, level_count) : std::nullopt;
				if (!converted)
				{
					ZONETOOL_WARNING("Cannot convert stream %d of image \"%s\"", i, image.name);
					return false;
				}

				total_size += static_cast<std::uint32_t>(converted->size());
				image.streams[i].width = stream.width;
				image.streams[i].height = stream.height;
				image.streams[i].levelCountAndSize.levelCount = stream.levelCount;
				image.streams[i].levelCountAndSize.pixelSize = total_size;

				const auto path = std::format("{}streamed_images\\{}_stream{}.pixels", filesystem::get_dump_path(), name, i);
				if (!utils::io::write_file(path, *converted, false))
				{
					ZONETOOL_WARNING("Cannot write stream %d of image \"%s\" to \"%s\"", i, image.name, path.data());
					return false;
				}
			}

			image.dataLen2 = total_size;
			dump_header("streamed_images\\" + name + ".iw7Image", &image);
			return true;
		}

		bool dump_resident(const GfxImage* asset, zonetool::iw7::GfxImage& image, const pixel_transform& transform)
		{
			const auto pixels = get_image_pixels(asset);
			const auto tight_pixels = pixels ? remove_level_padding(asset, *pixels, 0, asset->levelCount) : std::nullopt;
			auto converted = tight_pixels ? transform(*tight_pixels, 0, asset->levelCount) : std::nullopt;
			if (!converted)
			{
				ZONETOOL_WARNING("Cannot convert the pixels of image \"%s\"", image.name);
				return false;
			}

			image.pixelData = reinterpret_cast<unsigned char*>(converted->data());
			image.dataLen1 = static_cast<unsigned int>(converted->size());
			image.dataLen2 = image.dataLen1;
			dump_header("images\\" + clean_name(image.name) + ".iw7Image", &image);
			return true;
		}

		std::uint64_t get_fingerprint(const GfxImage* asset, const zonetool::iw7::GfxImage& image)
		{
			fingerprint result;
			result.add_value(cache_revision::iw8_iw7_image).add(image.name).add_value(image.imageFormat).add_value(image.semantic)
				.add_value(asset->format).add_value(asset->flags).add_value(asset->width).add_value(asset->height).add_value(asset->depth)
				.add_value(asset->numElements).add_value(asset->levelCount);
			for (const auto& stream : asset->streams)
			{
				result.add_value(stream.xpakEntry.key);
			}

			return result.value();
		}

		bool dump_pixels(const GfxImage* asset, zonetool::iw7::GfxImage& image, const pixel_transform& transform)
		{
			const auto streamed = (asset->flags & IMG_DISK_FLAG_STREAMED) != 0;
			const auto output = std::format("{}{}\\{}.iw7Image", filesystem::get_dump_path(), streamed ? "streamed_images" : "images", clean_name(image.name));
			const auto fingerprint = get_fingerprint(asset, image);
			if (is_dump_current(output, fingerprint))
			{
				return true;
			}

			const auto dumped = streamed ? dump_streamed(asset, image, transform) : dump_resident(asset, image, transform);
			if (dumped)
			{
				update_dump_cache(output, fingerprint);
			}

			return dumped;
		}

		float srgb_to_linear(const float srgb)
		{
			return srgb <= 0.04045f ? srgb / 12.92f : std::pow((srgb + 0.055f) / 1.055f, 2.4f);
		}

		float srgb_to_linear(const int value)
		{
			return srgb_to_linear(value / 255.0f);
		}

		float linear_to_srgb(const float value)
		{
			const auto linear = std::clamp(value, 0.0f, 1.0f);
			return linear <= 0.0031308f ? linear * 12.92f : 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f;
		}

		float pq_to_linear(const float value)
		{
			const auto power = std::pow(std::clamp(value, 0.0f, 1.0f), 1.0f / 78.84375f);
			return std::pow(std::max(power - 0.8359375f, 0.0f) / (18.8515625f - 18.6875f * power), 1.0f / 0.1593017578125f);
		}

		float linear_to_pq(const float value)
		{
			const auto power = std::pow(std::clamp(value, 0.0f, 1.0f), 0.1593017578125f);
			return std::pow((0.8359375f + 18.8515625f * power) / (1.0f + 18.6875f * power), 78.84375f);
		}

		float tonemap(const float value) // ACES fit (Narkowicz), stands in for the IW8 display mapping
		{
			const auto x = std::max(value, 0.0f);
			return x * (2.51f * x + 0.03f) / (x * (2.43f * x + 0.59f) + 0.14f);
		}

		float inverse_tonemap(const float value)
		{
			const auto display = std::clamp(value, 0.0f, 1.0f);
			const auto a = 2.43f * display - 2.51f;
			const auto b = 0.59f * display - 0.03f;
			const auto c = 0.14f * display;
			return (-b - std::sqrt(b * b - 4.0f * a * c)) / (2.0f * a);
		}

		color transform(const std::array<color, 3>& matrix, const color& value)
		{
			color result{};
			for (auto row = 0u; row < result.size(); row++)
			{
				result[row] = matrix[row][0] * value[0] + matrix[row][1] * value[1] + matrix[row][2] * value[2];
			}

			return result;
		}

		color sample_clut(const std::uint16_t* texels, const std::size_t size, const color& coordinate)
		{
			std::array<std::size_t, 3> base{};
			color fraction{};
			for (auto axis = 0u; axis < base.size(); axis++)
			{
				const auto position = std::clamp(coordinate[axis], 0.0f, 1.0f) * (size - 1);
				base[axis] = std::min(static_cast<std::size_t>(position), size - 2);
				fraction[axis] = position - base[axis];
			}

			color result{};
			for (auto corner = 0u; corner < 8; corner++)
			{
				auto weight = 1.0f;
				std::array<std::size_t, 3> position{};
				for (auto axis = 0u; axis < position.size(); axis++)
				{
					const auto upper = (corner >> axis) & 1;
					position[axis] = base[axis] + upper;
					weight *= upper ? fraction[axis] : 1.0f - fraction[axis];
				}

				const auto* texel = &texels[((position[2] * size + position[1]) * size + position[0]) * 4];
				for (auto channel = 0u; channel < result.size(); channel++)
				{
					result[channel] += weight * texel[channel] / 65535.0f;
				}
			}

			return result;
		}

		color grade_display_color(const std::uint16_t* texels, const std::size_t size, const color& display)
		{
			const std::array<color, 3> bt709_to_bt2020{{{0.6274f, 0.3293f, 0.0433f}, {0.0691f, 0.9195f, 0.0114f}, {0.0164f, 0.0880f, 0.8956f}}};
			const std::array<color, 3> bt2020_to_bt709{{{1.6605f, -0.5876f, -0.0728f}, {-0.1246f, 1.1329f, -0.0083f}, {-0.0182f, -0.1006f, 1.1187f}}};

			color scene{};
			for (auto channel = 0u; channel < scene.size(); channel++)
			{
				scene[channel] = inverse_tonemap(srgb_to_linear(display[channel]));
			}

			auto encoded = transform(bt709_to_bt2020, scene);
			for (auto& value : encoded)
			{
				value = linear_to_pq(value / 1024.0f);
			}

			auto graded = sample_clut(texels, size, encoded);
			for (auto& value : graded)
			{
				value = pq_to_linear(value) * 1024.0f;
			}

			auto result = transform(bt2020_to_bt709, graded);
			for (auto& value : result)
			{
				value = linear_to_srgb(tonemap(value));
			}

			return result;
		}

		std::uint8_t to_squared_gamma(const int value)
		{
			return static_cast<std::uint8_t>(std::lround(std::sqrt(srgb_to_linear(value)) * 255.0f));
		}

		std::uint8_t to_byte(const float value)
		{
			return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
		}

		std::uint8_t to_range(const int value, const unsigned char scale, const unsigned char offset)
		{
			return to_byte(value / 255.0f * (scale + 1) / 256.0f + offset / 256.0f);
		}

		std::uint8_t to_normal_scale(const int value, const unsigned char scale)
		{
			return to_byte((value / 255.0f - 0.5f) * (scale + 1) / 256.0f + 0.5f);
		}

		std::optional<DirectX::ScratchImage> decode_rgba8(const DXGI_FORMAT format, const std::size_t width, const std::size_t height,
			const std::uint32_t level_count, const std::string_view pixels)
		{
			DirectX::ScratchImage source;
			if (FAILED(source.Initialize2D(DirectX::MakeLinear(format), width, height, 1, level_count)) || source.GetPixelsSize() != pixels.size())
			{
				return {};
			}

			std::memcpy(source.GetPixels(), pixels.data(), pixels.size());
			if (source.GetMetadata().format == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				return source;
			}

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(format)
				? DirectX::Decompress(source.GetImages(), source.GetImageCount(), source.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM, decoded)
				: DirectX::Convert(source.GetImages(), source.GetImageCount(), source.GetMetadata(), DXGI_FORMAT_R8G8B8A8_UNORM,
					DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				return {};
			}

			return decoded;
		}

		DXGI_FORMAT get_repacked_format(const GfxImage* asset)
		{
			return asset->width % 4 == 0 && asset->height % 4 == 0 ? DXGI_FORMAT_BC7_UNORM : DXGI_FORMAT_R8G8B8A8_UNORM;
		}

		std::optional<std::string> repack(const GfxImage* asset, const std::string_view pixels, const std::uint32_t first_level,
			const std::uint32_t level_count, const channel_tables& tables)
		{
			const auto width = std::max(1, asset->width >> first_level);
			const auto height = std::max(1, asset->height >> first_level);
			auto decoded = decode_rgba8(get_dxgi_format(asset->format), width, height, level_count, pixels);
			if (!decoded)
			{
				return {};
			}

			for (auto i = 0u; i < decoded->GetImageCount(); i++)
			{
				const auto& image = decoded->GetImages()[i];
				for (auto row = 0u; row < image.height; row++)
				{
					auto* texel = image.pixels + row * image.rowPitch;
					for (auto column = 0u; column < image.width; column++, texel += 4)
					{
						for (auto channel = 0u; channel < tables.size(); channel++)
						{
							texel[channel] = tables[channel][texel[channel]];
						}
					}
				}
			}

			if (get_repacked_format(asset) != DXGI_FORMAT_BC7_UNORM)
			{
				return std::string(reinterpret_cast<const char*>(decoded->GetPixels()), decoded->GetPixelsSize());
			}

			const auto compressed = compress(*decoded, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				return {};
			}

			return std::string(reinterpret_cast<const char*>(compressed->GetPixels()), compressed->GetPixelsSize());
		}

		std::optional<std::string> dump_repacked(const GfxImage* asset, const std::string& name, const zonetool::iw7::TextureSemantic semantic,
			const channel_tables& tables)
		{
			if (!mark_dumped(ASSET_TYPE_IMAGE, name))
			{
				return name;
			}

			auto image = convert(asset);
			if (image.imageFormat == DXGI_FORMAT_UNKNOWN || image.mapType != zonetool::iw7::MAPTYPE_2D)
			{
				return {};
			}

			image.name = name.data();
			image.imageFormat = get_repacked_format(asset);
			image.semantic = semantic;
			const auto dumped = dump_pixels(asset, image, [&](const std::string_view pixels, const std::uint32_t first_level, const std::uint32_t level_count)
			{
				return repack(asset, pixels, first_level, level_count, tables);
			});

			return dumped ? std::optional(name) : std::nullopt;
		}

		std::optional<std::string> transcode_bc1_srgb(const GfxImage* asset, const std::string_view pixels, const std::uint32_t first_level,
			const std::uint32_t level_count)
		{
			const auto width = std::max(1, asset->width >> first_level);
			const auto height = std::max(1, asset->height >> first_level);
			const auto decoded = decode_rgba8(DXGI_FORMAT_BC1_UNORM_SRGB, width, height, level_count, pixels);
			const auto compressed = decoded ? compress(*decoded, DXGI_FORMAT_BC3_UNORM, DirectX::TEX_COMPRESS_DEFAULT) : std::nullopt;
			if (!compressed)
			{
				return {};
			}

			return std::string(reinterpret_cast<const char*>(compressed->GetPixels()), compressed->GetPixelsSize());
		}

		std::optional<DirectX::ScratchImage> decode_raw(const GfxImageRaw& raw, const DXGI_FORMAT target_format)
		{
			const auto format = get_dxgi_format(raw.desc.format);
			DirectX::ScratchImage source;
			if (format == DXGI_FORMAT_UNKNOWN || !raw.pixels || FAILED(source.Initialize2D(format, raw.desc.width, raw.desc.height, 1, 1))
				|| source.GetPixelsSize() != raw.desc.totalSize)
			{
				return {};
			}

			std::memcpy(source.GetPixels(), raw.pixels, source.GetPixelsSize());

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(format)
				? DirectX::Decompress(*source.GetImage(0, 0, 0), target_format, decoded)
				: DirectX::Convert(*source.GetImage(0, 0, 0), target_format, DirectX::TEX_FILTER_DEFAULT, DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				return {};
			}

			return decoded;
		}

		bool has_size(const DirectX::ScratchImage& image, const GfxImageDesc& desc)
		{
			return image.GetMetadata().width == desc.width && image.GetMetadata().height == desc.height;
		}

		template <typename T>
		T* get_row(const DirectX::Image& image, const std::size_t row)
		{
			return reinterpret_cast<T*>(image.pixels + row * image.rowPitch);
		}

		float get_octahedron_z(const std::array<std::uint8_t, 2>& encoded)
		{
			const auto u = encoded[0] / 255.0f * 2.0f - 1.0f;
			const auto v = encoded[1] / 255.0f * 2.0f - 1.0f;
			const auto x = (u + v) * 0.5f;
			const auto y = (u - v) * 0.5f;
			const auto z = 1.0f - std::abs(x) - std::abs(y);
			return z / std::sqrt(x * x + y * y + z * z);
		}

		void dump_lightmap_image(const std::string& name, const DXGI_FORMAT format, const std::size_t width, const std::size_t height, const std::string_view pixels)
		{
			zonetool::iw7::GfxImage image{};
			image.imageFormat = format;
			image.flags = zonetool::iw7::IMG_DISK_FLAG_NOPICMIP | zonetool::iw7::IMG_DISK_FLAG_NOMIPMAPS | zonetool::iw7::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS
				| zonetool::iw7::IMG_DISK_FLAG_CLAMP_U | zonetool::iw7::IMG_DISK_FLAG_CLAMP_V;
			image.mapType = zonetool::iw7::MAPTYPE_2D;
			image.semantic = zonetool::iw7::TS_FUNCTION;
			image.category = zonetool::iw7::IMG_CATEGORY_LIGHTMAP;
			image.width = static_cast<unsigned short>(width);
			image.height = static_cast<unsigned short>(height);
			image.depth = 1;
			image.numElements = 1;
			image.levelCount = 1;
			image.name = name.data();
			image.pixelData = reinterpret_cast<unsigned char*>(const_cast<char*>(pixels.data()));
			image.dataLen1 = static_cast<unsigned int>(pixels.size());
			image.dataLen2 = image.dataLen1;
			dump_header("images\\" + clean_name(name) + ".iw7Image", &image);
		}

		std::string dump_default_reveal()
		{
			const std::string name = "~layer_reveal_ff80";
			if (!mark_dumped(ASSET_TYPE_IMAGE, name))
			{
				return name;
			}

			std::string pixels;
			for (auto i = 0; i < 16; i++)
			{
				pixels.append("\xFF\x80\x00\xFF", 4);
			}

			zonetool::iw7::GfxImage image{};
			image.imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			image.flags = zonetool::iw7::IMG_DISK_FLAG_NOPICMIP | zonetool::iw7::IMG_DISK_FLAG_NOMIPMAPS;
			image.mapType = zonetool::iw7::MAPTYPE_2D;
			image.semantic = zonetool::iw7::TS_ALPHA_REVEAL_THICKNESS_MAP;
			image.category = zonetool::iw7::IMG_CATEGORY_LOAD_FROM_FILE;
			image.width = 4;
			image.height = 4;
			image.depth = 1;
			image.numElements = 1;
			image.levelCount = 1;
			image.name = name.data();
			image.pixelData = reinterpret_cast<unsigned char*>(pixels.data());
			image.dataLen1 = static_cast<unsigned int>(pixels.size());
			image.dataLen2 = image.dataLen1;
			dump_header("images\\" + name + ".iw7Image", &image);
			return name;
		}
	}

	void dump(GfxImage* asset)
	{
		if (!mark_dumped(ASSET_TYPE_IMAGE, asset->name))
		{
			return;
		}

		auto image = convert(asset);
		if (image.imageFormat == DXGI_FORMAT_UNKNOWN)
		{
			ZONETOOL_WARNING("Image \"%s\" has the unsupported format %d", asset->name, asset->format);
			return;
		}

		if (image.imageFormat == DXGI_FORMAT_BC1_UNORM_SRGB)
		{
			image.imageFormat = DXGI_FORMAT_BC3_UNORM_SRGB; // IW7 has no BC1_UNORM_SRGB mip size (0x140E03040)
			dump_pixels(asset, image, [&](const std::string_view pixels, const std::uint32_t first_level, const std::uint32_t level_count)
			{
				return transcode_bc1_srgb(asset, pixels, first_level, level_count);
			});
			return;
		}

		dump_pixels(asset, image, [](const std::string_view pixels, std::uint32_t, std::uint32_t)
		{
			return std::optional<std::string>(pixels);
		});
	}

	std::optional<std::string> dump_grading_clut(const GfxGradingClut* clut)
	{
		const auto* source = clut->hdrClutImage ? static_cast<GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, clut->hdrClutImage->name)) : nullptr;
		if (!source || clut->inputColorimetry != HDR_GRADING_CLUT_INPUT_COLORIMETRY_BT2020_PQ
			|| clut->outputColorimetry != HDR_GRADING_CLUT_OUTPUT_COLORIMETRY_BT2020_PQ || get_dxgi_format(source->format) != DXGI_FORMAT_R16G16B16A16_UNORM
			|| source->width < 2 || source->width != source->height || source->width != source->depth)
		{
			ZONETOOL_WARNING("Grading CLUT \"%s\" has an unsupported format", clut->name);
			return {};
		}

		if (!mark_dumped(ASSET_TYPE_IMAGE, clut->name))
		{
			return clut->name;
		}

		const std::size_t size = source->width;
		const auto pixels = get_image_pixels(source);
		const auto tight_pixels = pixels ? remove_level_padding(source, *pixels, 0, 1) : std::nullopt;
		if (!tight_pixels || tight_pixels->size() < size * size * size * 8)
		{
			ZONETOOL_WARNING("Cannot read the pixels of grading CLUT \"%s\"", clut->name);
			return {};
		}

		const auto* texels = reinterpret_cast<const std::uint16_t*>(tight_pixels->data());
		std::string converted(size * size * size * 4, '\0');
		auto* texel = reinterpret_cast<std::uint8_t*>(converted.data());
		for (auto blue = 0u; blue < size; blue++)
		{
			for (auto green = 0u; green < size; green++)
			{
				for (auto red = 0u; red < size; red++, texel += 4)
				{
					const color display{red / (size - 1.0f), green / (size - 1.0f), blue / (size - 1.0f)};
					const auto graded = grade_display_color(texels, size, display);
					for (auto channel = 0u; channel < graded.size(); channel++)
					{
						texel[channel] = to_byte(graded[channel]);
					}
					texel[3] = 0xFF;
				}
			}
		}

		zonetool::iw7::GfxImage image{};
		image.imageFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		image.flags = zonetool::iw7::IMG_DISK_FLAG_NOPICMIP | zonetool::iw7::IMG_DISK_FLAG_NOMIPMAPS | zonetool::iw7::IMG_DISK_FLAG_CLAMP_U
			| zonetool::iw7::IMG_DISK_FLAG_CLAMP_V | zonetool::iw7::IMG_DISK_FLAG_MAPTYPE_3D;
		image.mapType = zonetool::iw7::MAPTYPE_3D;
		image.semantic = zonetool::iw7::TS_COLOR_MAP;
		image.category = zonetool::iw7::IMG_CATEGORY_LOAD_FROM_FILE;
		image.width = static_cast<unsigned short>(size);
		image.height = static_cast<unsigned short>(size);
		image.depth = static_cast<unsigned short>(size);
		image.numElements = 1;
		image.levelCount = 1;
		image.name = clut->name;
		image.pixelData = reinterpret_cast<unsigned char*>(converted.data());
		image.dataLen1 = static_cast<unsigned int>(converted.size());
		image.dataLen2 = image.dataLen1;
		dump_header("images\\" + clean_name(clut->name) + ".iw7Image", &image);
		return clut->name;
	}

	std::optional<std::string> dump_color(const GfxImage* asset)
	{
		const auto srgb = DirectX::IsSRGB(get_dxgi_format(asset->format));

		channel_tables tables{};
		for (auto i = 0; i < 256; i++)
		{
			tables[0][i] = tables[1][i] = tables[2][i] = srgb ? to_squared_gamma(i) : static_cast<std::uint8_t>(i);
			tables[3][i] = static_cast<std::uint8_t>(i);
		}

		return dump_repacked(asset, std::format("{}~c", asset->name), zonetool::iw7::TS_COLOR_MAP, tables);
	}

	std::optional<std::string> dump_color_specular(const GfxImage* asset, const unsigned char scale, const unsigned char offset)
	{
		const auto srgb = DirectX::IsSRGB(get_dxgi_format(asset->format));

		channel_tables tables{};
		for (auto i = 0; i < 256; i++)
		{
			tables[0][i] = tables[1][i] = tables[2][i] = srgb ? to_squared_gamma(i) : static_cast<std::uint8_t>(i);
			tables[3][i] = to_range(i, scale, offset);
		}

		return dump_repacked(asset, std::format("{}~cs{:02x}{:02x}", asset->name, scale, offset), zonetool::iw7::TS_COLOR_SPECULAR_MAP, tables);
	}

	std::optional<std::string> dump_emissive(const GfxImage* asset, const std::array<float, 3>& tint)
	{
		const auto srgb = DirectX::IsSRGB(get_dxgi_format(asset->format));

		channel_tables tables{};
		for (auto i = 0; i < 256; i++)
		{
			const auto linear = srgb ? srgb_to_linear(i) : i / 255.0f;
			for (auto channel = 0u; channel < tint.size(); channel++)
			{
				tables[channel][i] = to_byte(std::sqrt(linear * tint[channel]));
			}
			tables[3][i] = 0xFF;
		}

		const auto name = std::format("{}~e{:02x}{:02x}{:02x}", asset->name, to_byte(tint[0]), to_byte(tint[1]), to_byte(tint[2]));
		return dump_repacked(asset, name, zonetool::iw7::TS_COLOR_MAP, tables);
	}

	std::optional<std::string> dump_normal_gloss(const GfxImage* asset, const unsigned char gloss_scale, const unsigned char gloss_offset,
		const unsigned char normal_scale)
	{
		channel_tables tables{};
		for (auto i = 0; i < 256; i++)
		{
			tables[0][i] = to_range(i, gloss_scale, gloss_offset);
			tables[1][i] = tables[3][i] = to_normal_scale(i, normal_scale);
			tables[2][i] = to_byte(std::sqrt(i / 255.0f));
		}

		const auto name = std::format("{}~ng{:02x}{:02x}{:02x}", asset->name, gloss_scale, gloss_offset, normal_scale);
		return dump_repacked(asset, name, zonetool::iw7::TS_NORMAL_OCCLUSSION_GLOSS_MAP, tables);
	}

	std::string dump_layer_reveal(GfxImage* mask, const bool use_coverage, const bool use_height)
	{
		if (!mask || (!use_coverage && !use_height))
		{
			return dump_default_reveal();
		}

		if (use_coverage && use_height)
		{
			dump(mask);
			return mask->name;
		}

		channel_tables tables{};
		for (auto i = 0; i < 256; i++)
		{
			tables[0][i] = use_coverage ? static_cast<std::uint8_t>(i) : 0xFF;
			tables[1][i] = use_height ? static_cast<std::uint8_t>(i) : 0x80;
			tables[2][i] = tables[3][i] = static_cast<std::uint8_t>(i);
		}

		if (const auto name = dump_repacked(mask, std::format("{}~rv{:d}{:d}", mask->name, use_coverage, use_height), zonetool::iw7::TS_ALPHA_REVEAL_THICKNESS_MAP, tables))
		{
			return *name;
		}

		return dump_default_reveal();
	}

	std::optional<std::array<std::string, 3>> dump_lightmap(const GfxLightmapRaw& lightmap, const std::string& name, const float scale)
	{
		const auto fraction = decode_raw(lightmap.textures[0], DXGI_FORMAT_R32G32B32A32_FLOAT);
		const auto irradiance = decode_raw(lightmap.textures[1], DXGI_FORMAT_R32G32B32A32_FLOAT);
		const auto direction = decode_raw(lightmap.textures[2], DXGI_FORMAT_R8G8_UNORM);
		const auto& desc = lightmap.textures[1].desc;
		if (!fraction || !irradiance || !direction || !has_size(*fraction, desc) || !has_size(*direction, desc))
		{
			ZONETOOL_WARNING("Cannot decode lightmap \"%s\"", name.data());
			return {};
		}

		const std::size_t width = desc.width;
		const std::size_t height = desc.height;
		DirectX::ScratchImage lighting;
		if (FAILED(lighting.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, width, height * 2, 1, 1)))
		{
			return {};
		}

		const auto& target = *lighting.GetImage(0, 0, 0);
		for (auto row = 0u; row < height; row++)
		{
			const auto* fraction_row = get_row<DirectX::XMFLOAT4>(*fraction->GetImage(0, 0, 0), row);
			const auto* irradiance_row = get_row<DirectX::XMFLOAT4>(*irradiance->GetImage(0, 0, 0), row);
			const auto* direction_row = get_row<std::array<std::uint8_t, 2>>(*direction->GetImage(0, 0, 0), row);
			auto* ambient_row = get_row<DirectX::XMFLOAT4>(target, row);
			auto* directional_row = get_row<DirectX::XMFLOAT4>(target, height + row);
			for (auto column = 0u; column < width; column++)
			{
				const auto ambient = fraction_row[column].x * scale;
				const auto directional = (1.0f - fraction_row[column].x) * scale / std::max(get_octahedron_z(direction_row[column]), 0.001f);
				const auto& light = irradiance_row[column];
				ambient_row[column] = {light.x * ambient, light.y * ambient, light.z * ambient, 1.0f};
				directional_row[column] = {light.x * directional, light.y * directional, light.z * directional, 1.0f};
			}
		}

		const auto secondary = compress(lighting, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT);
		if (!secondary)
		{
			ZONETOOL_WARNING("Cannot encode lightmap \"%s\"", name.data());
			return {};
		}

		std::string primary(width / 4 * height / 4 * 8, '\0');
		for (auto block = 0u; block < primary.size(); block += 8)
		{
			primary[block] = primary[block + 1] = '\xFF';
		}

		const std::array<std::string, 3> names{name + "_primary", name + "_secondary", name + "_secondunorm"};
		const auto& direction_image = *direction->GetImage(0, 0, 0);
		dump_lightmap_image(names[0], DXGI_FORMAT_BC4_UNORM, width, height, primary);
		dump_lightmap_image(names[1], DXGI_FORMAT_BC6H_UF16, width, height * 2,
			{reinterpret_cast<const char*>(secondary->GetPixels()), secondary->GetPixelsSize()});
		dump_lightmap_image(names[2], DXGI_FORMAT_R8G8_UNORM, width, height,
			{reinterpret_cast<const char*>(direction_image.pixels), direction_image.slicePitch});
		return names;
	}

	void dump_reflection_probe_array(const GfxImage* octahedron, const char* name)
	{
		const auto format = get_dxgi_format(octahedron->format);
		const auto pixels = get_image_pixels(octahedron);
		const auto tight_pixels = pixels ? remove_level_padding(octahedron, *pixels, 0, octahedron->levelCount) : std::nullopt;
		const auto elements = std::max<std::size_t>(1, octahedron->numElements);

		DirectX::ScratchImage source;
		if (!tight_pixels || format == DXGI_FORMAT_UNKNOWN
			|| FAILED(source.Initialize2D(format, octahedron->width, octahedron->height, elements, octahedron->levelCount))
			|| source.GetPixelsSize() != tight_pixels->size())
		{
			ZONETOOL_WARNING("Cannot read the reflection probes of image \"%s\"", octahedron->name);
			return;
		}

		std::memcpy(source.GetPixels(), tight_pixels->data(), tight_pixels->size());

		DirectX::ScratchImage decoded;
		if (FAILED(DirectX::Decompress(source.GetImages(), source.GetImageCount(), source.GetMetadata(), DXGI_FORMAT_R32G32B32A32_FLOAT, decoded)))
		{
			ZONETOOL_WARNING("Cannot decode the reflection probes of image \"%s\"", octahedron->name);
			return;
		}

		const std::size_t face_size = std::max(1, octahedron->width / 2);
		std::size_t levels = 1;
		while ((face_size >> levels) > 0)
		{
			levels++;
		}

		DirectX::ScratchImage cubes;
		if (FAILED(cubes.InitializeCube(DXGI_FORMAT_R32G32B32A32_FLOAT, face_size, face_size, elements, levels)))
		{
			return;
		}

		for (auto element = 0u; element < elements; element++)
		{
			for (auto level = 0u; level < levels; level++)
			{
				const auto* source_level = decoded.GetImage(std::min<std::size_t>(level, octahedron->levelCount - 1), element, 0);
				for (auto face = 0u; face < 6; face++)
				{
					write_cube_face(*source_level, *cubes.GetImage(level, element * 6 + face, 0), face);
				}
			}
		}

		const auto compressed = compress(cubes, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT);
		if (!compressed)
		{
			ZONETOOL_WARNING("Cannot encode the reflection probes of image \"%s\"", octahedron->name);
			return;
		}

		zonetool::iw7::GfxImage image{};
		image.imageFormat = DXGI_FORMAT_BC6H_UF16;
		image.flags = zonetool::iw7::IMG_DISK_FLAG_MAPTYPE_CUBE_ARRAY | zonetool::iw7::IMG_DISK_FLAG_PACKED_ATLAS
			| zonetool::iw7::IMG_DISK_FLAG_GAMMA_SRGB | zonetool::iw7::IMG_DISK_FLAG_NOPICMIP;
		image.mapType = zonetool::iw7::MAPTYPE_CUBE_ARRAY;
		image.semantic = zonetool::iw7::TS_FUNCTION;
		image.category = zonetool::iw7::IMG_CATEGORY_AUTO_GENERATED;
		image.width = static_cast<unsigned short>(face_size);
		image.height = static_cast<unsigned short>(face_size);
		image.depth = 1;
		image.numElements = static_cast<unsigned short>(elements);
		image.levelCount = static_cast<unsigned char>(levels);
		image.name = name;
		image.pixelData = compressed->GetPixels();
		image.dataLen1 = static_cast<unsigned int>(compressed->GetPixelsSize());
		image.dataLen2 = image.dataLen1;
		dump_header("images\\" + clean_name(name) + ".iw7Image", &image);
	}
}
