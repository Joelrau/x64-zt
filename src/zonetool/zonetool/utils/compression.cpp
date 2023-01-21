#include <std_include.hpp>
#include "compression.hpp"

#include <stdexcept>

#include <zstd.h>
#include <zlib.h>

#include <lz4.h>
#include <lz4hc.h>
#include <lz4frame.h>

#include <tomcrypt.h>

#include <utils/string.hpp>

#define LZ4_COMPRESSION 4
#define LZ4_CLEVEL 8 // compression level
#define MAX_BLOCK_SIZE 0x10000ull

#define ZSTD_COMPRESSION 11
#define ZLIB_COMPRESSION Z_BEST_COMPRESSION

namespace compression
{
	namespace lz4
	{
		namespace
		{
			size_t align_value(size_t value, unsigned int alignment)
			{
				const auto diff = value % alignment;
				return diff != 0
					? value + (alignment - diff)
					: value;
			}

			const char* align_value(const char* value, unsigned int alignment)
			{
				return reinterpret_cast<const char*>(
					align_value(reinterpret_cast<size_t>(value), alignment));
			}
		}

		std::vector<std::uint8_t> compress_lz4_block(const void* data, const size_t size)
		{
			std::vector<std::uint8_t> out_buffer;

			auto bytes_to_compress = size;
			if (bytes_to_compress > std::numeric_limits<unsigned int>::max())
			{
				throw std::runtime_error("cannot compress more than `std::numeric_limits<unsigned int>::max()` bytes");
			}

			auto data_ptr = reinterpret_cast<const char*>(data);

			const auto write = [&](void* data, const size_t len)
			{
				for (auto i = 0ull; i < len; i++)
				{
					out_buffer.push_back(reinterpret_cast<char*>(data)[i]);
				}
			};

			auto first_block = true;

			while (bytes_to_compress > 0)
			{
				const auto block_size = static_cast<unsigned int>(std::min(bytes_to_compress, MAX_BLOCK_SIZE));
				const auto bound = LZ4_compressBound(block_size);

				std::string buffer;
				buffer.resize(bound);

				const auto compressed_size = LZ4_compress_HC(data_ptr,
					buffer.data(), block_size, bound, LZ4_CLEVEL);
				buffer.resize(align_value(compressed_size, 4));

				if (first_block)
				{
					compressed_block_header header{};
					header.unknown2 = 1;
					header.compression_type = LZ4_COMPRESSION;
					header.uncompressed_size = static_cast<unsigned int>(bytes_to_compress);
					header.compressed_size = compressed_size;
					header.uncompressed_block_size = block_size;

					write(&header, sizeof(header));
				}
				else
				{
					intermediate_header header{};
					header.compressed_size = compressed_size;
					header.uncompressed_block_size = block_size;

					write(&header, sizeof(header));
				}

				write(buffer.data(), buffer.size());

				first_block = false;

				bytes_to_compress -= block_size;
				data_ptr += block_size;
			}

			return out_buffer;
		}

		std::vector<std::uint8_t> compress_lz4_block(const std::vector<std::uint8_t>& data, const size_t size)
		{
			return compress_lz4_block(data.data(), size);
		}

		std::vector<std::uint8_t> compress_lz4_block(const std::vector<std::uint8_t>& data)
		{
			return compress_lz4_block(data, data.size());
		}

		std::string compress_lz4_block(const std::string& data)
		{
			const auto compressed = compress_lz4_block(data.data(), data.size());
			return { compressed.begin(), compressed.end() };
		}

		std::vector<std::uint8_t> decompress_lz4_block(const void* data, const size_t size)
		{
			std::vector<std::uint8_t> out_buffer;

			auto data_ptr = reinterpret_cast<const char*>(data);
			auto end_ptr = data_ptr + size;

			const auto write = [&](void* data, const size_t len)
			{
				for (auto i = 0ull; i < len; i++)
				{
					out_buffer.push_back(reinterpret_cast<char*>(data)[i]);
				}
			};

			auto first_block = true;

			compressed_block_header header{};

			while (data_ptr < end_ptr)
			{
				if (first_block)
				{
					header = *reinterpret_cast<compressed_block_header*>(
						reinterpret_cast<size_t>(data_ptr));

					data_ptr += sizeof(compressed_block_header);
				}
				else
				{
					const auto int_header = reinterpret_cast<intermediate_header*>(
						reinterpret_cast<size_t>(data_ptr));

					header.compressed_size = int_header->compressed_size;
					header.uncompressed_size = int_header->uncompressed_block_size;
					header.uncompressed_block_size = int_header->uncompressed_block_size;

					data_ptr += sizeof(intermediate_header);
				}

				if (header.compression_type != 4)
				{
					throw std::runtime_error("invalid compression type");
				}

				std::string buffer;
				buffer.resize(header.uncompressed_block_size);

				const auto read_count = static_cast<unsigned int>(LZ4_decompress_safe(data_ptr, buffer.data(),
					header.compressed_size, header.uncompressed_block_size));

				if (read_count != header.uncompressed_block_size)
				{
					throw std::runtime_error("bad read");
				}

				first_block = false;

				data_ptr += header.compressed_size;
				data_ptr = align_value(data_ptr, 4);

				write(buffer.data(), buffer.size());
			}

			return out_buffer;
		}

		std::vector<std::uint8_t> decompress_lz4_block(const std::vector<std::uint8_t>& data, const size_t size)
		{
			return decompress_lz4_block(data.data(), size);
		}

		std::vector<std::uint8_t> decompress_lz4_block(const std::vector<std::uint8_t>& data)
		{
			return decompress_lz4_block(data, data.size());
		}

		std::string decompress_lz4_block(const std::string& data)
		{
			const auto decompressed = decompress_lz4_block(data.data(), data.size());
			return { decompressed.begin(), decompressed.end() };
		}
	}

	std::vector<std::uint8_t> compress_lz4(const std::uint8_t* data, const std::size_t size)
	{
		return compression::lz4::compress_lz4_block(data, size);
	}

	std::vector<std::uint8_t> compress_zlib(const std::uint8_t* data, const std::size_t size, bool compress_blocks)
	{
		auto compressBound = [](unsigned long sourceLen)
		{
			return static_cast<unsigned long>((ceil(sourceLen * 1.001)) + 12);
		};

		if (compress_blocks == false)
		{
			// calculate buffer size needed for current zone
			auto compressed_size = compressBound(static_cast<unsigned long>(size));

			// alloc array for compressed data
			std::vector<std::uint8_t> compressed;
			compressed.resize(compressed_size);

			// compress buffer
			compress2(compressed.data(), &compressed_size, data, static_cast<uLong>(size), ZLIB_COMPRESSION);
			compressed.resize(compressed_size);

			// return compressed buffer
			return compressed;
		}
		else
		{
			// data should be 0x10000 byte aligned
			const auto block_size = 0x10000;
			auto bound_size = compressBound(block_size);
			auto num_blocks = size / block_size;

			std::vector<std::vector<std::uint8_t>> blocks;
			blocks.resize(num_blocks);

			auto data_ptr = data;
			for (auto& block : blocks)
			{
				// allocate for compressed data
				block.resize(bound_size);

				// compress block buffer
				unsigned long compressed_size;
				compress2(block.data(), &compressed_size, data_ptr, block_size, ZLIB_COMPRESSION);
				if (compressed_size >= block_size)
				{
					// discard compressed data and just store uncompressed data
					block.resize(block_size + 2);

					// 0 block size is uncompressed
					block[0] = 0;
					block[1] = 0;
					memcpy(block.data() + 2, data_ptr, block_size);
				}
				else
				{
					block.resize(compressed_size);

					// overwrite zlib header with block size
					compressed_size -= 2;
					block[0] = (compressed_size & 0xff00) >> 8;
					block[1] = compressed_size & 0xff;
				}

				// go to next block
				data_ptr += block_size;
			}

			std::vector<uint8_t> compressed;
			for (auto& block : blocks)
			{
				compressed.insert(compressed.end(), block.begin(), block.end());
			}

			return compressed;
		}
	}

	std::vector<std::uint8_t> compress_zstd(const std::uint8_t* data, const std::size_t size)
	{
		// calculate buffer size needed for current zone
		auto compressed_size = ZSTD_compressBound(size);

		// alloc array for compressed data
		std::vector<std::uint8_t> compressed;
		compressed.resize(compressed_size);

		// compress buffer
		auto destsize = ZSTD_compress(compressed.data(), compressed_size, data, size, ZSTD_COMPRESSION);
		compressed.resize(destsize);

		if (ZSTD_isError(destsize))
		{
			throw std::runtime_error(utils::string::va("An error occured while compressing the fastfile: %s", ZSTD_getErrorName(destsize)));
		}

		// return compressed buffer
		return compressed;
	}
}
