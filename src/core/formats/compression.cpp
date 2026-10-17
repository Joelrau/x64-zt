#include <std_include.hpp>
#include "compression.hpp"

#include <atomic>
#include <stdexcept>
#include <thread>

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

	namespace
	{
		constexpr std::size_t zlib_chunk_size = 0x400000;
		constexpr std::size_t zlib_window_size = 0x8000;
		constexpr std::uint8_t zlib_header[] = {0x78, 0xDA};

		std::vector<std::uint8_t> deflate_raw_chunk(const std::uint8_t* data, const std::size_t size, const std::size_t dictionary_size, const bool last)
		{
			z_stream stream{};
			if (deflateInit2(&stream, ZLIB_COMPRESSION, Z_DEFLATED, -MAX_WBITS, 8, Z_DEFAULT_STRATEGY) != Z_OK)
			{
				return {};
			}

			if (dictionary_size != 0)
			{
				deflateSetDictionary(&stream, data - dictionary_size, static_cast<uInt>(dictionary_size));
			}

			std::vector<std::uint8_t> result(deflateBound(&stream, static_cast<uLong>(size)) + 16);
			stream.next_in = const_cast<Bytef*>(data);
			stream.avail_in = static_cast<uInt>(size);
			stream.next_out = result.data();
			stream.avail_out = static_cast<uInt>(result.size());

			const auto status = deflate(&stream, last ? Z_FINISH : Z_SYNC_FLUSH);
			const auto complete = last
				? status == Z_STREAM_END
				: status == Z_OK && stream.avail_in == 0 && stream.avail_out != 0;

			result.resize(stream.total_out);
			deflateEnd(&stream);

			if (!complete)
			{
				return {};
			}

			return result;
		}

		std::vector<std::uint8_t> compress_zlib_parallel(const std::uint8_t* data, const std::size_t size)
		{
			const auto chunk_count = (size + zlib_chunk_size - 1) / zlib_chunk_size;
			std::vector<std::vector<std::uint8_t>> chunks(chunk_count);
			std::atomic_size_t next_chunk{};

			const auto compress_chunks = [&]
			{
				for (auto index = next_chunk++; index < chunk_count; index = next_chunk++)
				{
					const auto offset = index * zlib_chunk_size;
					const auto chunk_size = std::min(zlib_chunk_size, size - offset);
					const auto dictionary_size = std::min(offset, zlib_window_size);
					chunks[index] = deflate_raw_chunk(data + offset, chunk_size, dictionary_size, index + 1 == chunk_count);
				}
			};

			const auto thread_count = std::min<std::size_t>(std::max(1u, std::thread::hardware_concurrency()), chunk_count);
			std::vector<std::thread> threads;
			for (auto i = 0ull; i < thread_count; i++)
			{
				threads.emplace_back(compress_chunks);
			}

			for (auto& thread : threads)
			{
				thread.join();
			}

			std::vector<std::uint8_t> result(std::begin(zlib_header), std::end(zlib_header));
			for (const auto& chunk : chunks)
			{
				if (chunk.empty())
				{
					throw std::runtime_error("An error occured while compressing the fastfile with zlib");
				}

				result.insert(result.end(), chunk.begin(), chunk.end());
			}

			const auto checksum = adler32_z(1, data, size);
			for (auto shift = 24; shift >= 0; shift -= 8)
			{
				result.push_back(static_cast<std::uint8_t>(checksum >> shift));
			}

			return result;
		}
	}

	std::vector<std::uint8_t> compress_zlib(const std::uint8_t* data, const std::size_t size, bool compress_blocks)
	{
		auto compressBound = [](unsigned long sourceLen)
		{
			return static_cast<unsigned long>((ceil(sourceLen * 1.001)) + 12);
		};

		if (compress_blocks == false && size >= zlib_chunk_size * 2)
		{
			return compress_zlib_parallel(data, size);
		}

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
