#include <std_include.hpp>
#include "oodle.hpp"

namespace zonetool::xfile::oodle
{
	namespace
	{
		using decompress_function = std::intptr_t(__stdcall*)(const void* source, std::intptr_t source_size, void* destination,
			std::intptr_t destination_size, int fuzz_safe, int check_crc, int verbosity, void* dictionary_base,
			std::intptr_t dictionary_size, void* callback, void* callback_data, void* decoder_memory,
			std::intptr_t decoder_memory_size, int thread_phase);

		using compress_function = std::intptr_t(__stdcall*)(int compressor, const void* source, std::intptr_t source_size,
			void* destination, int level, const void* options, const void* dictionary_base, const void* long_range_matcher,
			void* scratch_memory, std::intptr_t scratch_size);

		enum compressor
		{
			compressor_kraken = 8,
		};

		enum compression_level
		{
			compression_level_normal = 4,
		};

		decompress_function decompress_ = nullptr;
		compress_function compress_ = nullptr;
	}

	void load(const std::filesystem::path& game_folder)
	{
		if (decompress_)
		{
			return;
		}

		for (const auto& entry : std::filesystem::directory_iterator(game_folder))
		{
			const auto name = entry.path().filename().string();
			if (!name.starts_with("oo2core_") || !name.ends_with("_win64.dll"))
			{
				continue;
			}

			const auto library = LoadLibraryW(entry.path().wstring().data());
			if (!library)
			{
				continue;
			}

			decompress_ = reinterpret_cast<decompress_function>(GetProcAddress(library, "OodleLZ_Decompress"));
			compress_ = reinterpret_cast<compress_function>(GetProcAddress(library, "OodleLZ_Compress"));
			if (decompress_ && compress_)
			{
				return;
			}
		}

		throw std::runtime_error(std::format("cannot find the Oodle library (oo2core_*_win64.dll) in {}", game_folder.string()));
	}

	void decompress(const std::string_view source, char* destination, const std::size_t size)
	{
		if (!decompress_)
		{
			throw std::runtime_error("the Oodle library is not loaded");
		}

		const auto result = decompress_(source.data(), static_cast<std::intptr_t>(source.size()), destination,
			static_cast<std::intptr_t>(size), 1, 0, 0, nullptr, 0, nullptr, nullptr, nullptr, 0, 3);
		if (result != static_cast<std::intptr_t>(size))
		{
			throw std::runtime_error("oodle block failed");
		}
	}

	std::string compress(const std::string_view source)
	{
		if (!compress_)
		{
			throw std::runtime_error("the Oodle library is not loaded");
		}

		std::string destination(source.size() + 0x1000, '\0');
		const auto size = compress_(compressor_kraken, source.data(), static_cast<std::intptr_t>(source.size()), destination.data(),
			compression_level_normal, nullptr, nullptr, nullptr, nullptr, 0);
		if (size <= 0)
		{
			throw std::runtime_error("oodle compression failed");
		}

		destination.resize(size);
		return destination;
	}
}
