#include <std_include.hpp>

#include <d3d11.h>
#include <wrl/client.h>

#include "image_compress.hpp"

#include "../zonetool.hpp"

#include <utils/flags.hpp>

#pragma comment(lib, "d3d11.lib")

namespace image_compress
{
	namespace
	{
		ID3D11Device* get_gpu()
		{
			static const auto device = []
			{
				Microsoft::WRL::ComPtr<ID3D11Device> result;
				if (utils::flags::has_flag("no_gpu")
					|| FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &result, nullptr, nullptr)))
				{
					return Microsoft::WRL::ComPtr<ID3D11Device>{};
				}

				ZONETOOL_INFO("Compressing BC6H and BC7 images on the GPU");
				return result;
			}();
			return device.Get();
		}

		std::optional<DirectX::ScratchImage> compress_on_gpu(const DirectX::ScratchImage& source, const DXGI_FORMAT format, const DirectX::TEX_COMPRESS_FLAGS flags)
		{
			static std::mutex mutex;

			auto* const device = (format == DXGI_FORMAT_BC7_UNORM || format == DXGI_FORMAT_BC6H_UF16) ? get_gpu() : nullptr;
			if (!device)
			{
				return {};
			}

			std::lock_guard lock(mutex);
			DirectX::ScratchImage compressed;
			if (FAILED(DirectX::Compress(device, source.GetImages(), source.GetImageCount(), source.GetMetadata(), format, flags,
				DirectX::TEX_ALPHA_WEIGHT_DEFAULT, compressed)))
			{
				return {};
			}

			return compressed;
		}
	}

	std::optional<DirectX::ScratchImage> compress(const DirectX::ScratchImage& source, const DXGI_FORMAT format, const DirectX::TEX_COMPRESS_FLAGS flags)
	{
		if (auto compressed = compress_on_gpu(source, format, flags))
		{
			return compressed;
		}

		auto metadata = source.GetMetadata();
		metadata.format = format;

		DirectX::ScratchImage compressed;
		if (FAILED(compressed.Initialize(metadata)))
		{
			return {};
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
		const auto compress_strips = [&]
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

				const auto& target_image = compressed.GetImages()[index];
				std::memcpy(target_image.pixels + target_image.rowPitch * (row / 4), result.GetPixels(), result.GetPixelsSize());
			}
		};

		std::vector<std::jthread> threads(utils::flags::get_thread_count());
		for (auto& thread : threads)
		{
			thread = std::jthread(compress_strips);
		}

		threads.clear();
		if (failed)
		{
			return {};
		}

		return compressed;
	}
}
