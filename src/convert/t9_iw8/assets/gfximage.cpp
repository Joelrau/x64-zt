#include <std_include.hpp>

#include "../include.hpp"
#include "gfximage.hpp"

#include <io/output_cache.hpp>

#include <formats/image_compress.hpp>

#include <utils/flags.hpp>

namespace zonetool::t9::converter::iw8::gfximage
{
	namespace
	{
		namespace target = zonetool::iw8;

		using channel_tables = std::array<std::array<std::uint8_t, 256>, 4>;

		const GfxImage* resolve(const GfxImage* asset)
		{
			if (!asset || asset->format)
			{
				return asset;
			}

			const auto loaded = static_cast<const GfxImage*>(db_find_asset(ASSET_TYPE_IMAGE, xhash_name(asset->name)));
			return loaded ? loaded : asset;
		}

		bool has_pixels(const GfxImage* asset)
		{
			return asset && asset->format && asset->pixelSize;
		}

		std::optional<std::array<std::uint8_t, 4>> get_code_image_texel(const GfxImage* asset) // core_bootstrap texels after the reader stop
		{
			if (!asset || asset->format)
			{
				return {};
			}

			const auto name = xhash_name(asset->name);
			if (name == "$black_color" || name == "$black")
			{
				return {{0, 0, 0, 255}};
			}

			if (name == "$blacktransparent_color")
			{
				return {{0, 0, 0, 0}};
			}

			if (name == "$white")
			{
				return {{255, 255, 255, 255}};
			}

			return {};
		}

		bool has_source(const GfxImage* asset)
		{
			return has_pixels(asset) || get_code_image_texel(asset);
		}

		std::string get_output_name(const GfxImage* asset)
		{
			auto name = xhash_name(asset->name);
			std::ranges::replace(name, ':', '_');
			std::ranges::replace(name, '$', '_');
			return name;
		}

		bool is_single_channel(const DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_BC4_UNORM || format == DXGI_FORMAT_BC4_SNORM || format == DXGI_FORMAT_R8_UNORM;
		}

		bool is_hdr(const DXGI_FORMAT format)
		{
			return format == DXGI_FORMAT_BC6H_UF16 || format == DXGI_FORMAT_BC6H_SF16;
		}

		std::uint8_t to_byte(const float value)
		{
			return static_cast<std::uint8_t>(std::lround(std::clamp(value, 0.0f, 1.0f) * 255.0f));
		}

		std::uint8_t linear_to_srgb(const float linear)
		{
			return to_byte(linear <= 0.0031308f ? linear * 12.92f : 1.055f * std::pow(linear, 1.0f / 2.4f) - 0.055f);
		}

		float srgb_to_linear(const float srgb)
		{
			return srgb <= 0.04045f ? srgb / 12.92f : std::pow((srgb + 0.055f) / 1.055f, 2.4f);
		}

		std::optional<DirectX::ScratchImage> load_source(const GfxImage* asset)
		{
			if (!has_pixels(asset))
			{
				return {};
			}

			if (asset->mapType != MAPTYPE_2D)
			{
				ZONETOOL_WARNING("Image \"%s\" map type %u is not 2D", xhash_name(asset->name).data(), asset->mapType);
				return {};
			}

			auto image = load_image(asset);
			if (image && !image->OverrideFormat(DirectX::MakeLinear(image->GetMetadata().format)))
			{
				return {};
			}

			return image;
		}

		HRESULT decompress_in_strips(const DirectX::ScratchImage& source, DirectX::ScratchImage& decoded)
		{
			auto metadata = source.GetMetadata();
			metadata.format = DXGI_FORMAT_R8G8B8A8_UNORM;
			if (const auto result = decoded.Initialize(metadata); FAILED(result))
			{
				return result;
			}

			std::vector<std::pair<std::size_t, std::size_t>> strips;
			for (auto index = 0u; index < source.GetImageCount(); index++)
			{
				for (auto row = 0u; row < source.GetImages()[index].height; row += 64)
				{
					strips.emplace_back(index, row);
				}
			}

			std::atomic_size_t next_strip{};
			std::atomic_bool failed{};
			const auto decompress_strips = [&]
			{
				for (auto strip = next_strip++; strip < strips.size(); strip = next_strip++)
				{
					const auto [index, row] = strips[strip];
					auto part = source.GetImages()[index];
					part.height = std::min<std::size_t>(64, part.height - row);
					part.slicePitch = part.rowPitch * ((part.height + 3) / 4);
					part.pixels += part.rowPitch * (row / 4);

					DirectX::ScratchImage result;
					if (FAILED(DirectX::Decompress(part, DXGI_FORMAT_R8G8B8A8_UNORM, result)))
					{
						failed = true;
						continue;
					}

					const auto& target_image = decoded.GetImages()[index];
					const auto& strip_image = *result.GetImage(0, 0, 0);
					for (auto line = 0u; line < strip_image.height; line++)
					{
						std::memcpy(target_image.pixels + target_image.rowPitch * (row + line), strip_image.pixels + strip_image.rowPitch * line,
							std::min(target_image.rowPitch, strip_image.rowPitch));
					}
				}
			};

			std::vector<std::jthread> threads(utils::flags::get_thread_count());
			for (auto& thread : threads)
			{
				thread = std::jthread(decompress_strips);
			}

			threads.clear();
			return failed ? E_FAIL : S_OK;
		}

		std::optional<DirectX::ScratchImage> decode(const GfxImage* asset, DirectX::ScratchImage source)
		{
			const auto& metadata = source.GetMetadata();
			if (metadata.format == DXGI_FORMAT_R8G8B8A8_UNORM)
			{
				return source;
			}

			DirectX::ScratchImage decoded;
			const auto result = DirectX::IsCompressed(metadata.format)
				? decompress_in_strips(source, decoded)
				: DirectX::Convert(source.GetImages(), source.GetImageCount(), metadata, DXGI_FORMAT_R8G8B8A8_UNORM, DirectX::TEX_FILTER_FORCE_NON_WIC,
					DirectX::TEX_THRESHOLD_DEFAULT, decoded);
			if (FAILED(result))
			{
				ZONETOOL_WARNING("Cannot decode image \"%s\" format %u", xhash_name(asset->name).data(), asset->format);
				return {};
			}

			for (auto i = 0u; i < decoded.GetImageCount(); i++)
			{
				const auto& image = decoded.GetImages()[i];
				for (auto row = 0u; row < image.height; row++)
				{
					auto* texel = image.pixels + row * image.rowPitch;
					for (auto column = 0u; column < image.width; column++, texel += 4)
					{
						if (is_single_channel(metadata.format))
						{
							texel[1] = texel[0];
							texel[2] = texel[0];
						}
						else if (is_hdr(metadata.format))
						{
							for (auto channel = 0u; channel < 3; channel++)
							{
								texel[channel] = linear_to_srgb(texel[channel] / 255.0f);
							}
						}
					}
				}
			}

			if (is_hdr(metadata.format))
			{
				ZONETOOL_WARNING("Image \"%s\" is BC6H, the converted image is LDR", xhash_name(asset->name).data());
			}

			return decoded;
		}

		std::optional<DirectX::ScratchImage> make_solid_image(const std::array<std::uint8_t, 4>& texel)
		{
			DirectX::ScratchImage image;
			if (FAILED(image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 4, 4, 1, 1)))
			{
				return {};
			}

			auto* pixels = image.GetPixels();
			for (auto i = 0u; i < image.GetPixelsSize(); i += 4)
			{
				std::memcpy(pixels + i, texel.data(), texel.size());
			}

			return image;
		}

		std::optional<DirectX::ScratchImage> read_pixels(const GfxImage* asset)
		{
			if (const auto texel = get_code_image_texel(asset))
			{
				return make_solid_image(*texel);
			}

			const auto source = load_source(asset);
			DirectX::ScratchImage top_level;
			if (!source || FAILED(top_level.InitializeFromImage(*source->GetImage(0, 0, 0))))
			{
				return {};
			}

			return decode(asset, std::move(top_level));
		}

		bool is_identity(const channel_tables& tables, const std::array<std::uint8_t, 4>& sources)
		{
			for (auto channel = 0u; channel < tables.size(); channel++)
			{
				if (sources[channel] != channel)
				{
					return false;
				}

				for (auto i = 0u; i < tables[channel].size(); i++)
				{
					if (tables[channel][i] != i)
					{
						return false;
					}
				}
			}

			return true;
		}

		channel_tables make_identity_tables()
		{
			channel_tables tables{};
			for (auto& table : tables)
			{
				for (auto i = 0u; i < table.size(); i++)
				{
					table[i] = static_cast<std::uint8_t>(i);
				}
			}

			return tables;
		}

		channel_tables make_inverted_tables()
		{
			channel_tables tables{};
			for (auto& table : tables)
			{
				for (auto i = 0u; i < table.size(); i++)
				{
					table[i] = static_cast<std::uint8_t>(255 - i);
				}
			}

			return tables;
		}

		bool is_bc7_2d(const DirectX::ScratchImage& image)
		{
			const auto& metadata = image.GetMetadata();
			return metadata.format == DXGI_FORMAT_BC7_UNORM && metadata.dimension == DirectX::TEX_DIMENSION_TEXTURE2D && metadata.arraySize == 1
				&& metadata.width % 4 == 0 && metadata.height % 4 == 0;
		}

		target::GfxImage* make_image(const std::string& name, const DirectX::ScratchImage& compressed, const target::GfxPixelFormat format,
			const std::uint32_t flags, const target::TextureSemantic semantic)
		{
			static utils::memory::allocator allocator;

			const auto& metadata = compressed.GetMetadata();
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(name);
			result->format = format;
			result->flags = static_cast<target::GfxImageFlags>(flags);
			result->totalSize = static_cast<unsigned int>(compressed.GetPixelsSize());
			result->width = static_cast<unsigned short>(metadata.width);
			result->height = static_cast<unsigned short>(metadata.height);
			result->depth = 1;
			result->numElements = static_cast<unsigned short>(metadata.arraySize);
			result->atlasInfo.atlasSize = {.rowCount = 1, .colCount = 1};
			result->semantic = semantic;
			result->category = target::IMG_CATEGORY_LOAD_FROM_FILE;
			result->levelCount = static_cast<unsigned char>(metadata.mipLevels);
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);

			auto* pixels = result->pixels.residentData;
			for (auto level = 0u; level < metadata.mipLevels; level++)
			{
				for (auto item = 0u; item < metadata.arraySize; item++)
				{
					const auto* image = compressed.GetImage(level, item, 0);
					std::memcpy(pixels, image->pixels, image->slicePitch);
					pixels += image->slicePitch;
				}
			}

			return result;
		}

		target::GfxImage* convert(const GfxImage* asset, const std::string& name, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic, const channel_tables& tables, const std::array<std::uint8_t, 4>& sources = {0, 1, 2, 3})
		{
			auto loaded = load_source(asset);
			if (!loaded)
			{
				return nullptr;
			}

			if (is_identity(tables, sources) && is_bc7_2d(*loaded))
			{
				return make_image(name, *loaded, format, flags, semantic);
			}

			auto decoded = decode(asset, std::move(*loaded));
			if (!decoded)
			{
				return nullptr;
			}

			const auto& metadata = decoded->GetMetadata();
			if (metadata.width % 4 || metadata.height % 4)
			{
				ZONETOOL_WARNING("Image \"%s\" is %zux%zu, BC7 needs a multiple of 4", name.data(), metadata.width, metadata.height);
				return nullptr;
			}

			for (auto i = 0u; i < decoded->GetImageCount(); i++)
			{
				const auto& image = decoded->GetImages()[i];
				for (auto row = 0u; row < image.height; row++)
				{
					auto* texel = image.pixels + row * image.rowPitch;
					for (auto column = 0u; column < image.width; column++, texel += 4)
					{
						const std::array<std::uint8_t, 4> source{texel[0], texel[1], texel[2], texel[3]};
						for (auto channel = 0u; channel < tables.size(); channel++)
						{
							texel[channel] = tables[channel][source[sources[channel]]];
						}
					}
				}
			}

			const auto compressed = image_compress::compress(*decoded, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", name.data());
				return nullptr;
			}

			return make_image(name, *compressed, format, flags, semantic);
		}

		bool resize(std::optional<DirectX::ScratchImage>& source, const std::size_t width, const std::size_t height)
		{
			if (!source || (source->GetMetadata().width == width && source->GetMetadata().height == height))
			{
				return true;
			}

			DirectX::ScratchImage resized;
			if (FAILED(DirectX::Resize(*source->GetImage(0, 0, 0), width, height, DirectX::TEX_FILTER_LINEAR | DirectX::TEX_FILTER_FORCE_NON_WIC, resized)))
			{
				return false;
			}

			source = std::move(resized);
			return true;
		}

		template <std::size_t count>
		bool read_composed_sources(const std::array<const GfxImage*, count>& sources, std::array<std::optional<DirectX::ScratchImage>, count>& pixels,
			std::size_t& width, std::size_t& height)
		{
			width = 4;
			height = 4;
			for (auto i = 0u; i < count; i++)
			{
				if ((pixels[i] = read_pixels(sources[i])))
				{
					width = std::max(width, pixels[i]->GetMetadata().width);
					height = std::max(height, pixels[i]->GetMetadata().height);
				}
			}

			width = (width + 3) & ~3ull;
			height = (height + 3) & ~3ull;
			for (auto& source : pixels)
			{
				if (!resize(source, width, height))
				{
					return false;
				}
			}

			return true;
		}

		const std::uint8_t* read_texel(const std::optional<DirectX::ScratchImage>& source, const std::size_t row, const std::size_t column)
		{
			return source ? source->GetImage(0, 0, 0)->pixels + row * source->GetImage(0, 0, 0)->rowPitch + column * 4 : nullptr;
		}

		target::GfxImage* compress_composed(const std::string& name, const DirectX::Image& image, const target::GfxPixelFormat format, const std::uint32_t flags,
			const target::TextureSemantic semantic)
		{
			DirectX::ScratchImage mips;
			if (FAILED(DirectX::GenerateMipMaps(image, DirectX::TEX_FILTER_BOX | DirectX::TEX_FILTER_FORCE_NON_WIC, 0, mips)))
			{
				return nullptr;
			}

			const auto compressed = image_compress::compress(mips, DXGI_FORMAT_BC7_UNORM, DirectX::TEX_COMPRESS_BC7_QUICK);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress image \"%s\"", name.data());
				return nullptr;
			}

			return make_image(name, *compressed, format, flags, semantic);
		}

		std::array<float, 256> make_linear_table(const GfxImage* asset)
		{
			const auto srgb = DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->format));

			std::array<float, 256> table{};
			for (auto i = 0; i < 256; i++)
			{
				table[i] = srgb ? srgb_to_linear(i / 255.0f) : i / 255.0f * i / 255.0f;
			}

			return table;
		}

		std::array<float, 256> make_unorm_table(const GfxImage* asset)
		{
			const auto srgb = asset && DirectX::IsSRGB(static_cast<DXGI_FORMAT>(asset->format));

			std::array<float, 256> table{};
			for (auto i = 0; i < 256; i++)
			{
				table[i] = srgb ? srgb_to_linear(i / 255.0f) : i / 255.0f;
			}

			return table;
		}

		std::array<float, 3> read_linear_color(const std::uint8_t* color_texel, const std::array<float, 256>& color_linear, const color_tint* tint,
			const std::uint8_t* mask_texel, const std::array<float, 256>& mask_unorm)
		{
			auto amount = 0.0f;
			if (tint && mask_texel)
			{
				amount = mask_unorm[mask_texel[0]];
			}
			else if (tint && !tint->mask)
			{
				amount = color_texel[3] / 255.0f;
			}

			std::array<float, 3> result{};
			for (auto channel = 0u; channel < 3; channel++)
			{
				const auto scale = tint ? 1.0f + amount * (tint->value[channel] - 1.0f) : 1.0f;
				result[channel] = std::clamp(color_linear[color_texel[channel]] * scale, 0.0f, 1.0f);
			}

			return result;
		}

		std::array<float, 4> sample_wrap(const DirectX::ScratchImage& source, const std::array<float, 256>& table, const float u, const float v)
		{
			const auto& image = *source.GetImage(0, 0, 0);
			const auto x = u * image.width - 0.5f;
			const auto y = v * image.height - 0.5f;
			const auto x0 = std::floor(x);
			const auto y0 = std::floor(y);
			const auto wrap = [](const float value, const std::size_t size)
			{
				const auto index = static_cast<std::int64_t>(value) % static_cast<std::int64_t>(size);
				return static_cast<std::size_t>(index < 0 ? index + static_cast<std::int64_t>(size) : index);
			};

			const std::array columns{wrap(x0, image.width), wrap(x0 + 1.0f, image.width)};
			const std::array rows{wrap(y0, image.height), wrap(y0 + 1.0f, image.height)};
			const std::array column_weights{1.0f - (x - x0), x - x0};
			const std::array row_weights{1.0f - (y - y0), y - y0};

			std::array<float, 4> result{};
			for (auto row = 0u; row < 2; row++)
			{
				for (auto column = 0u; column < 2; column++)
				{
					const auto* texel = image.pixels + rows[row] * image.rowPitch + columns[column] * 4;
					const auto weight = column_weights[column] * row_weights[row];
					for (auto channel = 0u; channel < 3; channel++)
					{
						result[channel] += table[texel[channel]] * weight;
					}

					result[3] += texel[3] / 255.0f * weight;
				}
			}

			return result;
		}

		float apply_mix_levels(const float value, const std::array<float, 4>& levels)
		{
			const auto scaled = std::clamp(std::clamp(value - levels[0], 0.0f, 1.0f) / std::max(levels[1] - levels[0], 0.0001f), 0.0f, 1.0f);
			return levels[2] + scaled * (levels[3] - levels[2]);
		}

		std::optional<DirectX::ScratchImage> read_tint_mask(const GfxImage* mask, const std::string& name, const std::size_t width, const std::size_t height)
		{
			if (!mask)
			{
				return {};
			}

			auto pixels = read_pixels(mask);
			if (!pixels)
			{
				ZONETOOL_WARNING("Cannot read tint mask \"%s\" of image \"%s\", the tint is not baked", xhash_name(mask->name).data(), name.data());
				return {};
			}

			if (!resize(pixels, width, height))
			{
				return {};
			}

			return pixels;
		}

		const color_tint* get_bakeable_tint(const color_tint* tint, const GfxImage* asset)
		{
			if (!tint || !tint->mask || has_source(resolve(tint->mask)))
			{
				return tint;
			}

			ZONETOOL_WARNING("Tint mask \"%s\" of image \"%s\" has no pixels, the tint is not baked", xhash_name(tint->mask->name).data(),
				xhash_name(asset->name).data());
			return nullptr;
		}

		std::uint64_t get_tint_fingerprint(const color_tint& tint, const GfxImage* mask)
		{
			return fingerprint{}.add_value(tint.value).add_value(mask ? mask->name.value : 0ull).value();
		}

		std::string get_tinted_name(const std::string& name, const std::uint64_t tint_fingerprint)
		{
			return std::format("{}_t{:08x}", name, static_cast<std::uint32_t>(tint_fingerprint));
		}

		std::uint64_t get_fingerprint(const std::string_view kind, const std::initializer_list<const GfxImage*> sources)
		{
			fingerprint result;
			result.add_value(cache_revision::t9_iw8_image).add(kind);
			for (const auto* source : sources)
			{
				if (!has_pixels(source))
				{
					result.add_value(has_source(source) ? source->name.value : 0);
					continue;
				}

				result.add_value(source->name.value).add_value(source->format).add_value(source->flags).add_value(source->mapType)
					.add_value(source->width).add_value(source->height).add_value(source->depth).add_value(source->levelCount)
					.add_value(source->pixelSize).add_value(source->streamPartCount);
			}

			return result.value();
		}

		target::GfxImage* dump_once(const std::string& name, const std::uint64_t source_fingerprint, const target::GfxImageSemanticSpecific semantic_specific,
			const std::function<target::GfxImage*()>& convert_image)
		{
			static utils::memory::allocator allocator;
			static std::unordered_map<std::string, target::GfxImage*> converted;

			const auto [entry, inserted] = converted.try_emplace(name);
			if (!inserted)
			{
				return entry->second;
			}

			auto file_name = name;
			std::ranges::replace(file_name, '*', '_');
			const auto output = std::format("{}images\\{}.iw8Image", filesystem::get_dump_path(), file_name);
			const auto output_fingerprint = fingerprint{}.add_value(source_fingerprint).add_value(semantic_specific.albedoMapScaleBias).value();
			if (is_dump_current(output, output_fingerprint))
			{
				entry->second = allocator.allocate<target::GfxImage>();
				entry->second->name = allocator.duplicate_string(name);
				entry->second->semanticSpecific = semantic_specific;
			}
			else if ((entry->second = convert_image()))
			{
				entry->second->semanticSpecific = semantic_specific;
				target::gfx_image::dump(entry->second);
				update_dump_cache(output, output_fingerprint);
			}

			if (entry->second)
			{
				add_csv_row("image", name);
			}

			return entry->second;
		}
	}

	target::GfxImage* dump_color(const GfxImage* asset, const color_tint* tint)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		tint = get_bakeable_tint(tint, asset);
		if (!tint)
		{
			const auto name = get_output_name(asset);
			return dump_once(name, get_fingerprint("color", {asset}), {.albedoMapScaleBias = 9}, [&]
			{
				return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP, make_identity_tables());
			});
		}

		const auto* mask = resolve(tint->mask);
		const auto tint_fingerprint = get_tint_fingerprint(*tint, mask);
		const auto name = get_tinted_name(get_output_name(asset), tint_fingerprint);
		const auto source_fingerprint = fingerprint{}.add_value(get_fingerprint("color_tint", {asset, mask})).add_value(tint_fingerprint).value();
		return dump_once(name, source_fingerprint, {.albedoMapScaleBias = 9}, [&]() -> target::GfxImage*
		{
			std::array<std::optional<DirectX::ScratchImage>, 1> pixels;
			std::size_t width, height;
			if (!read_composed_sources(std::array{asset}, pixels, width, height) || !pixels[0])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			const auto mask_pixels = read_tint_mask(mask, name, width, height);

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto color_linear = make_linear_table(asset);
			const auto mask_unorm = make_unorm_table(mask);
			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* color_texel = read_texel(pixels[0], row, column);
					const auto color = read_linear_color(color_texel, color_linear, tint, read_texel(mask_pixels, row, column), mask_unorm);

					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					for (auto channel = 0u; channel < 3; channel++)
					{
						texel[channel] = linear_to_srgb(color[channel]);
					}

					texel[3] = color_texel[3];
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP);
		});
	}

	target::GfxImage* dump_layered_color(const layered_color& asset)
	{
		if (asset.layers.empty() || asset.mixes.size() + 1 != asset.layers.size())
		{
			return nullptr;
		}

		std::vector<const GfxImage*> colors;
		std::vector<const GfxImage*> masks;
		std::vector<const GfxImage*> mixes;
		fingerprint parameters;
		for (const auto& layer : asset.layers)
		{
			colors.push_back(resolve(layer.color));
			masks.push_back(resolve(layer.tint.mask));
			if (!has_source(colors.back()))
			{
				return nullptr;
			}

			parameters.add_value(get_fingerprint("layer", {colors.back(), masks.back()})).add_value(layer.tint.value).add_value(layer.scale);
		}

		for (const auto& mix : asset.mixes)
		{
			mixes.push_back(resolve(mix.map));
			if (!has_source(mixes.back()))
			{
				return nullptr;
			}

			parameters.add_value(get_fingerprint("mix", {mixes.back()})).add_value(mix.scale).add_value(mix.levels);
		}

		const auto* softlight = resolve(asset.softlight.map);
		if (softlight && !has_source(softlight))
		{
			return nullptr;
		}

		parameters.add_value(get_fingerprint("softlight", {softlight})).add_value(asset.softlight.scale);
		const auto source_fingerprint = fingerprint{}.add_value(get_fingerprint("layered_color", {})).add_value(parameters.value()).value();
		const auto name = std::format("{}_l{:08x}", get_output_name(colors[0]), static_cast<std::uint32_t>(source_fingerprint));
		return dump_once(name, source_fingerprint, {.albedoMapScaleBias = 9}, [&]() -> target::GfxImage*
		{
			struct sampled_map
			{
				DirectX::ScratchImage pixels;
				std::array<float, 256> table;
			};

			const auto read_map = [&](const GfxImage* map, const std::array<float, 256>& table) -> std::optional<sampled_map>
			{
				auto pixels = read_pixels(map);
				if (!pixels)
				{
					return {};
				}

				return sampled_map{std::move(*pixels), table};
			};

			std::size_t width = 4;
			std::size_t height = 4;
			std::vector<sampled_map> color_maps;
			std::vector<std::optional<sampled_map>> mask_maps;
			for (auto i = 0u; i < colors.size(); i++)
			{
				auto color = read_map(colors[i], make_linear_table(colors[i]));
				if (!color)
				{
					ZONETOOL_WARNING("Cannot read the layer %u color of image \"%s\"", i, name.data());
					return nullptr;
				}

				width = std::max(width, color->pixels.GetMetadata().width);
				height = std::max(height, color->pixels.GetMetadata().height);
				color_maps.push_back(std::move(*color));
				mask_maps.push_back(masks[i] ? read_map(masks[i], make_unorm_table(masks[i])) : std::nullopt);
			}

			std::vector<sampled_map> mix_maps;
			for (const auto* mix : mixes)
			{
				auto map = read_map(mix, make_unorm_table(mix));
				if (!map)
				{
					ZONETOOL_WARNING("Cannot read a mix map of image \"%s\"", name.data());
					return nullptr;
				}

				mix_maps.push_back(std::move(*map));
			}

			const auto softlight_map = softlight ? read_map(softlight, make_linear_table(softlight)) : std::nullopt;
			if (softlight && !softlight_map)
			{
				ZONETOOL_WARNING("Cannot read the soft light detail map of image \"%s\"", name.data());
				return nullptr;
			}

			width = std::min(std::bit_ceil(width), 2048ull);
			height = std::min(std::bit_ceil(height), 2048ull);

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto& image = *composed.GetImage(0, 0, 0);
			std::atomic_size_t next_row{};
			const auto bake_rows = [&]
			{
				for (auto row = next_row++; row < height; row = next_row++)
				{
					for (auto column = 0u; column < width; column++)
					{
						const auto u = (column + 0.5f) / width;
						const auto v = (row + 0.5f) / height;

						std::array<float, 4> result{};
						for (auto i = 0u; i < asset.layers.size(); i++)
						{
							const auto& layer = asset.layers[i];
							const auto layer_u = u * layer.scale[0];
							const auto layer_v = v * layer.scale[1];
							const auto texel = sample_wrap(color_maps[i].pixels, color_maps[i].table, layer_u, layer_v);
							auto amount = texel[3];
							if (masks[i])
							{
								amount = mask_maps[i] ? sample_wrap(mask_maps[i]->pixels, mask_maps[i]->table, layer_u, layer_v)[0] : 0.0f;
							}

							std::array<float, 4> layer_color{};
							for (auto channel = 0u; channel < 3; channel++)
							{
								layer_color[channel] = texel[channel] * (1.0f + amount * (layer.tint.value[channel] - 1.0f));
							}

							layer_color[3] = texel[3];
							if (i == 0)
							{
								result = layer_color;
								continue;
							}

							const auto& mix = asset.mixes[i - 1];
							const auto blend = apply_mix_levels(sample_wrap(mix_maps[i - 1].pixels, mix_maps[i - 1].table, u * mix.scale[0], v * mix.scale[1])[0],
								mix.levels);
							for (auto channel = 0u; channel < 4; channel++)
							{
								result[channel] = std::lerp(result[channel], layer_color[channel], blend);
							}
						}

						if (softlight_map)
						{
							const auto detail = sample_wrap(softlight_map->pixels, softlight_map->table, u * asset.softlight.scale[0], v * asset.softlight.scale[1]);
							const auto bright = (detail[0] + detail[1] + detail[2]) / 3.0f > 0.5f;
							for (auto channel = 0u; channel < 3; channel++)
							{
								const auto base = std::clamp(result[channel], 0.0f, 1.0f);
								const auto twice = 2.0f * detail[channel];
								result[channel] = bright ? std::sqrt(base) * std::max(twice - 1.0f, 0.0f) + 2.0f * base * (1.0f - detail[channel])
									: (base * (1.0f - twice) + twice) * base;
							}
						}

						auto* texel = image.pixels + row * image.rowPitch + column * 4;
						for (auto channel = 0u; channel < 3; channel++)
						{
							texel[channel] = linear_to_srgb(std::clamp(result[channel], 0.0f, 1.0f));
						}

						texel[3] = to_byte(result[3]);
					}
				}
			};

			std::vector<std::jthread> threads(utils::flags::get_thread_count());
			for (auto& thread : threads)
			{
				thread = std::jthread(bake_rows);
			}

			threads.clear();
			return compress_composed(name, image, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_COLOR_MAP);
		});
	}

	target::GfxImage* dump_color_specular(const GfxImage* color, const GfxImage* specular, const color_tint* tint)
	{
		color = resolve(color);
		specular = resolve(specular);
		if (!has_source(color) || !has_pixels(specular))
		{
			return dump_color(color, tint);
		}

		tint = get_bakeable_tint(tint, color);
		const auto* mask = tint ? resolve(tint->mask) : nullptr;
		const auto tint_fingerprint = tint ? get_tint_fingerprint(*tint, mask) : 0;
		const auto base_name = std::format("{}_{}_cs", get_output_name(color), get_output_name(specular));
		const auto name = tint ? get_tinted_name(base_name, tint_fingerprint) : base_name;
		const auto source_fingerprint = tint
			? fingerprint{}.add_value(get_fingerprint("color_specular_tint", {color, specular, mask})).add_value(tint_fingerprint).value()
			: get_fingerprint("color_specular", {color, specular});
		return dump_once(name, source_fingerprint, {.albedoMapScaleBias = 0xFF}, [&]() -> target::GfxImage*
		{
			const std::array sources{color, specular};
			std::array<std::optional<DirectX::ScratchImage>, 2> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height) || !pixels[0] || !pixels[1])
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			const auto mask_pixels = tint ? read_tint_mask(mask, name, width, height) : std::nullopt;

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto color_linear = make_linear_table(color);
			const auto mask_unorm = make_unorm_table(mask);
			const auto specular_linear = make_linear_table(specular);
			const auto get_luminance = [](const std::array<float, 3>& value)
			{
				return value[0] * 0.2126f + value[1] * 0.7152f + value[2] * 0.0722f;
			};

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* color_texel = read_texel(pixels[0], row, column);
					const auto* specular_texel = read_texel(pixels[1], row, column);

					const auto diffuse = read_linear_color(color_texel, color_linear, tint, read_texel(mask_pixels, row, column), mask_unorm);
					std::array<float, 3> specular_color{}, albedo{};
					for (auto channel = 0u; channel < 3; channel++)
					{
						specular_color[channel] = specular_linear[specular_texel[channel]];
						albedo[channel] = std::max(diffuse[channel] + specular_color[channel] - 0.1f, 0.0f);
					}

					const auto specular_luminance = get_luminance(specular_color);
					const auto albedo_luminance = get_luminance(albedo);
					const auto metalness = specular_luminance > 0.1f && albedo_luminance > 0.0f
						? std::min((specular_luminance - 0.1f) / albedo_luminance, 1.0f) : 0.0f;

					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					for (auto channel = 0u; channel < 3; channel++)
					{
						texel[channel] = linear_to_srgb(metalness > 0.0f ? albedo[channel] : diffuse[channel]);
					}

					texel[3] = to_byte(metalness > 0.0f ? 0.1f + 0.9f * metalness : specular_luminance);
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_GAMMA_SRGB | target::IMG_DISK_FLAG_AUTOMETALNESS,
				target::TS_METALNESS_MAP);
		});
	}

	target::GfxImage* dump_normal_gloss(const GfxImage* normal, const GfxImage* roughness, const GfxImage* occlusion)
	{
		normal = resolve(normal);
		roughness = resolve(roughness);
		occlusion = resolve(occlusion);
		const std::array sources{has_pixels(normal) ? normal : nullptr, has_pixels(roughness) ? roughness : nullptr,
			has_pixels(occlusion) ? occlusion : nullptr};
		if (!sources[0] && !sources[1])
		{
			return nullptr;
		}

		const auto name = std::format("{}_{}_{}_ng", sources[0] ? get_output_name(normal) : "flat", sources[1] ? get_output_name(roughness) : "rough",
			sources[2] ? get_output_name(occlusion) : "open");
		return dump_once(name, get_fingerprint("normal_gloss", {sources[0], sources[1], sources[2]}), {.normalMapScaleBias = 0xFF00FF},
			[&]() -> target::GfxImage*
		{
			std::array<std::optional<DirectX::ScratchImage>, 3> pixels;
			std::size_t width, height;
			if (!read_composed_sources(sources, pixels, width, height))
			{
				ZONETOOL_WARNING("Cannot read the sources of image \"%s\"", name.data());
				return nullptr;
			}

			DirectX::ScratchImage composed;
			if (FAILED(composed.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, width, height, 1, 1)))
			{
				return nullptr;
			}

			const auto& image = *composed.GetImage(0, 0, 0);
			for (auto row = 0u; row < height; row++)
			{
				for (auto column = 0u; column < width; column++)
				{
					const auto* normal_texel = read_texel(pixels[0], row, column);
					const auto* roughness_texel = read_texel(pixels[1], row, column);
					const auto* occlusion_texel = read_texel(pixels[2], row, column);

					auto* texel = image.pixels + row * image.rowPitch + column * 4;
					texel[0] = roughness_texel ? static_cast<std::uint8_t>(255 - roughness_texel[0]) : 0;
					texel[1] = normal_texel ? normal_texel[1] : 128;
					texel[2] = occlusion_texel ? to_byte(occlusion_texel[0] / 255.0f * occlusion_texel[0] / 255.0f) : 255;
					texel[3] = normal_texel ? normal_texel[0] : 128;
				}
			}

			return compress_composed(name, image, target::GFX_PF_BC7, target::IMG_DISK_FLAG_NORMAL_OCCLUSON_GLOSS, target::TS_NORMAL_OCCLUSION_GLOSS_MAP);
		});
	}

	target::GfxImage* dump_opacity(const GfxImage* asset)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		const auto name = get_output_name(asset) + "_opacity";
		return dump_once(name, get_fingerprint("opacity", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, make_identity_tables(), {3, 3, 3, 3});
		});
	}

	target::GfxImage* dump_2d(const GfxImage* asset)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		const auto name = get_output_name(asset);
		return dump_once(name, get_fingerprint("2d", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7_SRGB, target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_CLAMP_U
				| target::IMG_DISK_FLAG_CLAMP_V | target::IMG_DISK_FLAG_GAMMA_SRGB, target::TS_2D, make_identity_tables());
		});
	}

	target::GfxImage* dump_reveal_opacity(const GfxImage* asset)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		const auto name = get_output_name(asset) + "_reveal_opacity";
		return dump_once(name, get_fingerprint("reveal_opacity", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, make_identity_tables(), {0, 0, 0, 0});
		});
	}

	target::GfxImage* dump_alpha_transmission(const GfxImage* asset)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		const auto name = get_output_name(asset) + "_alpha_transmission";
		return dump_once(name, get_fingerprint("alpha_transmission", {asset}), {}, [&]
		{
			return convert(asset, name, target::GFX_PF_BC7, 0, target::TS_2D, make_inverted_tables(), {3, 3, 3, 3});
		});
	}

	bool is_code_image(const GfxImage* asset)
	{
		return get_code_image_texel(resolve(asset)).has_value();
	}

	const char* get_color_reference(const GfxImage* asset)
	{
		const auto texel = get_code_image_texel(resolve(asset));
		return texel && !(*texel)[0] ? "$black" : "$white";
	}

	target::GfxImage* dump_identity_grading_clut(const std::string& name, const std::size_t size)
	{
		const auto cache_key = fingerprint().add_value(get_fingerprint("identity_grading_clut", {})).add_value(size).value();
		return dump_once(name, cache_key, {}, [&]
		{
			std::vector<std::uint16_t> texels(size * size * size * 4);
			for (auto b = 0u; b < size; b++)
			{
				for (auto g = 0u; g < size; g++)
				{
					for (auto r = 0u; r < size; r++)
					{
						auto* texel = &texels[((b * size + g) * size + r) * 4];
						texel[0] = static_cast<std::uint16_t>(r * 65535 / (size - 1));
						texel[1] = static_cast<std::uint16_t>(g * 65535 / (size - 1));
						texel[2] = static_cast<std::uint16_t>(b * 65535 / (size - 1));
					}
				}
			}

			static utils::memory::allocator allocator;
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = allocator.duplicate_string(name);
			result->format = target::GFX_PF_R16G16B16A16;
			result->flags = static_cast<target::GfxImageFlags>(target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS
				| target::IMG_DISK_FLAG_MAPTYPE_3D);
			result->totalSize = static_cast<unsigned int>(texels.size() * sizeof(std::uint16_t));
			result->width = static_cast<unsigned short>(size);
			result->height = static_cast<unsigned short>(size);
			result->depth = static_cast<unsigned short>(size);
			result->numElements = 1;
			result->semantic = target::TS_FUNCTION;
			result->category = target::IMG_CATEGORY_AUTO_GENERATED;
			result->levelCount = 1;
			result->streamedPartCount = 8; // TODO: stock gradingclutmp_rust_clut only
			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			std::memcpy(result->pixels.residentData, texels.data(), result->totalSize);
			return result;
		});
	}

	target::GfxImage* dump_ies_lookup()
	{
		return dump_once("*ieslookup", get_fingerprint("ies_lookup", {}), {}, []
		{
			static utils::memory::allocator allocator;
			const auto result = allocator.allocate<target::GfxImage>();
			result->name = "*ieslookup";
			result->format = target::GFX_PF_R8;
			result->flags = static_cast<target::GfxImageFlags>(target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_MAPTYPE_1D);
			result->width = 512;
			result->height = 1;
			result->depth = 1;
			result->numElements = 1;
			result->semantic = target::TS_FUNCTION;
			result->category = target::IMG_CATEGORY_AUTO_GENERATED;
			result->levelCount = 8;
			result->streamedPartCount = 4;
			for (auto level = 0; level < result->levelCount; level++)
			{
				result->totalSize += std::max(result->width >> level, 16);
			}

			result->pixels.residentData = allocator.allocate_array<char>(result->totalSize);
			std::memset(result->pixels.residentData, 0xFF, result->totalSize);
			return result;
		});
	}

	target::GfxImage* dump_uniform_reflection_probe(const std::string& name)
	{
		return dump_once(name, get_fingerprint("uniform_reflection_probe", {}), {}, [&]() -> target::GfxImage*
		{
			DirectX::ScratchImage image;
			if (FAILED(image.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 256, 256, 1, 6)))
			{
				return nullptr;
			}

			std::fill_n(reinterpret_cast<float*>(image.GetPixels()), image.GetPixelsSize() / sizeof(float), 1.0f);
			const auto compressed = image_compress::compress(image, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT);
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot compress the reflection probe image \"%s\"", name.data());
				return nullptr;
			}

			const auto result = make_image(name, *compressed, target::GFX_PF_BC6H, target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_MAPTYPE_ARRAY,
				target::TS_FUNCTION);
			result->atlasInfo = {};
			result->category = target::IMG_CATEGORY_LIGHTMAP;
			return result;
		});
	}

	namespace
	{
		std::array<float, 3> get_hemisphere_octahedron_direction(const float u, const float v)
		{
			const auto a = u * 2.0f - 1.0f;
			const auto b = v * 2.0f - 1.0f;
			const auto x = (b - a) * 0.5f;
			const auto y = (a + b) * 0.5f;
			const auto z = std::max(1.0f - std::abs(x) - std::abs(y), 0.0f);
			const auto length = std::sqrt(x * x + y * y + z * z);
			return {x / length, y / length, z / length};
		}

		std::array<float, 3> sample_equirectangular(const DirectX::Image& image, const std::array<float, 3>& direction)
		{
			const auto [x, y, z] = direction;
			const auto u = 0.5f - std::atan2(y, x) / (2.0f * std::numbers::pi_v<float>);
			const auto v = std::acos(std::clamp(z, -1.0f, 1.0f)) / std::numbers::pi_v<float>;

			const auto width = static_cast<std::ptrdiff_t>(image.width);
			const auto column = u * image.width - 0.5f;
			const auto column_floor = std::floor(column);
			const auto column_0 = static_cast<std::size_t>((static_cast<std::ptrdiff_t>(column_floor) % width + width) % width);
			const auto column_1 = (column_0 + 1) % image.width;
			const auto column_weight = column - column_floor;

			const auto row = std::clamp(v * image.height - 0.5f, 0.0f, static_cast<float>(image.height - 1));
			const auto row_0 = static_cast<std::size_t>(row);
			const auto row_1 = std::min(row_0 + 1, image.height - 1);
			const auto row_weight = row - row_0;

			const auto texel = [&](const std::size_t texel_column, const std::size_t texel_row)
			{
				return reinterpret_cast<const float*>(image.pixels + texel_row * image.rowPitch) + texel_column * 4;
			};

			std::array<float, 3> result{};
			for (auto channel = 0u; channel < result.size(); channel++)
			{
				const auto top = std::lerp(texel(column_0, row_0)[channel], texel(column_1, row_0)[channel], column_weight);
				const auto bottom = std::lerp(texel(column_0, row_1)[channel], texel(column_1, row_1)[channel], column_weight);
				result[channel] = std::lerp(top, bottom, row_weight);
			}

			return result;
		}

		std::optional<DirectX::ScratchImage> read_equirectangular_sky(const GfxImage* asset)
		{
			auto source = load_source(asset);
			if (!source)
			{
				ZONETOOL_WARNING("Cannot load sky image \"%s\"", xhash_name(asset->name).data());
				return {};
			}

			const auto metadata = source->GetMetadata();
			DirectX::ScratchImage decoded;
			const auto decode_result = DirectX::IsCompressed(metadata.format)
				? DirectX::Decompress(*source->GetImage(0, 0, 0), DXGI_FORMAT_R16G16B16A16_FLOAT, decoded)
				: DirectX::Convert(*source->GetImage(0, 0, 0), DXGI_FORMAT_R16G16B16A16_FLOAT, DirectX::TEX_FILTER_FORCE_NON_WIC, DirectX::TEX_THRESHOLD_DEFAULT,
					decoded);
			source.reset();
			if (FAILED(decode_result))
			{
				ZONETOOL_WARNING("Cannot decode sky image \"%s\" format %u %zux%zu: 0x%X", xhash_name(asset->name).data(), metadata.format, metadata.width,
					metadata.height, decode_result);
				return {};
			}

			DirectX::ScratchImage resized;
			const auto resize_result = DirectX::Resize(*decoded.GetImage(0, 0, 0), std::max<std::size_t>(metadata.width / 2, 4),
				std::max<std::size_t>(metadata.height / 2, 4), DirectX::TEX_FILTER_BOX | DirectX::TEX_FILTER_FORCE_NON_WIC, resized);
			decoded.Release();

			DirectX::ScratchImage result;
			const auto convert_result = FAILED(resize_result) ? resize_result : DirectX::Convert(*resized.GetImage(0, 0, 0), DXGI_FORMAT_R32G32B32A32_FLOAT,
				DirectX::TEX_FILTER_FORCE_NON_WIC, DirectX::TEX_THRESHOLD_DEFAULT, result);
			if (FAILED(convert_result))
			{
				ZONETOOL_WARNING("Cannot resize sky image \"%s\" %zux%zu: 0x%X", xhash_name(asset->name).data(), metadata.width, metadata.height, convert_result);
				return {};
			}

			return result;
		}

		std::optional<DirectX::ScratchImage> make_hemisphere_octahedron(const DirectX::Image& sky)
		{
			DirectX::ScratchImage result;
			if (FAILED(result.Initialize2D(DXGI_FORMAT_R32G32B32A32_FLOAT, 2048, 2048, 1, 1)))
			{
				return {};
			}

			const auto& image = *result.GetImage(0, 0, 0);
			for (auto row = 0u; row < image.height; row++)
			{
				auto* texel = reinterpret_cast<float*>(image.pixels + row * image.rowPitch);
				for (auto column = 0u; column < image.width; column++, texel += 4)
				{
					const auto direction = get_hemisphere_octahedron_direction((column + 0.5f) / image.width, (row + 0.5f) / image.height);
					const auto color = sample_equirectangular(sky, direction);
					std::copy(color.begin(), color.end(), texel);
					texel[3] = 1.0f;
				}
			}

			return result;
		}
	}

	target::GfxImage* dump_sky(const GfxImage* asset)
	{
		asset = resolve(asset);
		if (!has_pixels(asset))
		{
			return nullptr;
		}

		const auto name = get_output_name(asset) + "_sky";
		return dump_once(name, get_fingerprint("sky", {asset}), {}, [&]() -> target::GfxImage*
		{
			const auto sky = read_equirectangular_sky(asset);
			const auto octahedron = sky ? make_hemisphere_octahedron(*sky->GetImage(0, 0, 0)) : std::nullopt;
			const auto compressed = octahedron ? image_compress::compress(*octahedron, DXGI_FORMAT_BC6H_UF16, DirectX::TEX_COMPRESS_DEFAULT) : std::nullopt;
			if (!compressed)
			{
				ZONETOOL_WARNING("Cannot convert sky image \"%s\": %s failed", name.data(), !sky ? "read" : !octahedron ? "projection" : "compression");
				return nullptr;
			}

			return make_image(name, *compressed, target::GFX_PF_BC6H, target::IMG_DISK_FLAG_NOPICMIP | target::IMG_DISK_FLAG_NOMIPMAPS
				| target::IMG_DISK_FLAG_CLAMP_U | target::IMG_DISK_FLAG_CLAMP_V | target::IMG_DISK_FLAG_OCTAHEDRON, target::TS_FUNCTION);
		});
	}
}
