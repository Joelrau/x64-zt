#include <std_include.hpp>
#include "xfile.hpp"

#include <utils/io.hpp>

#include <zlib.h>
#include <lz4.h>

namespace zonetool::xfile
{
	namespace
	{
		constexpr std::size_t auth_block_size = 0x4000;
		constexpr std::size_t auth_blocks_per_group = 512;

		constexpr std::size_t header_size_before_stream_files = offsetof(XFileHeader, baseFileLen);
		constexpr std::size_t file_length_fields_size = sizeof(XFileHeader) - header_size_before_stream_files;

		class byte_reader
		{
		public:
			byte_reader(const std::string_view data)
				: data_(data)
			{
			}

			template <typename T>
			T read()
			{
				T value{};
				this->read(&value, sizeof(T));
				return value;
			}

			void read(void* destination, const std::size_t size)
			{
				if (this->remaining() < size)
				{
					throw std::runtime_error("unexpected end of file");
				}

				std::memcpy(destination, this->data_.data() + this->offset_, size);
				this->offset_ += size;
			}

			std::string_view read_view(const std::size_t size)
			{
				if (this->remaining() < size)
				{
					throw std::runtime_error("unexpected end of file");
				}

				const auto view = this->data_.substr(this->offset_, size);
				this->offset_ += size;
				return view;
			}

			void skip(const std::size_t size)
			{
				this->read_view(size);
			}

			std::string_view rest()
			{
				return this->read_view(this->remaining());
			}

			std::size_t remaining() const
			{
				return this->data_.size() - this->offset_;
			}

		private:
			std::string_view data_;
			std::size_t offset_{};
		};

		std::string strip_auth_blocks(const std::string_view data)
		{
			std::string result;
			result.reserve(data.size());

			byte_reader reader(data);
			reader.skip(std::min(auth_block_size, reader.remaining()));

			while (reader.remaining())
			{
				reader.skip(std::min(auth_block_size, reader.remaining()));

				for (std::size_t block = 0; block < auth_blocks_per_group && reader.remaining(); block++)
				{
					result.append(reader.read_view(std::min(auth_block_size, reader.remaining())));
				}
			}

			return result;
		}

		std::string inflate_zlib(const std::string_view data, const int window_bits, const std::size_t size_hint)
		{
			z_stream stream{};
			if (inflateInit2(&stream, window_bits) != Z_OK)
			{
				throw std::runtime_error("inflateInit2 failed");
			}

			const auto _ = gsl::finally([&]
			{
				inflateEnd(&stream);
			});

			std::string result;
			result.resize(std::max(size_hint, data.size() * 4));

			stream.next_in = reinterpret_cast<const Bytef*>(data.data());
			stream.avail_in = static_cast<uInt>(data.size());

			while (true)
			{
				if (stream.total_out == result.size())
				{
					result.resize(result.size() * 2);
				}

				stream.next_out = reinterpret_cast<Bytef*>(result.data() + stream.total_out);
				stream.avail_out = static_cast<uInt>(std::min<std::size_t>(result.size() - stream.total_out, UINT32_MAX));

				const auto status = inflate(&stream, Z_NO_FLUSH);
				if (status == Z_STREAM_END)
				{
					break;
				}

				if (status != Z_OK && status != Z_BUF_ERROR)
				{
					throw std::runtime_error("zlib inflate failed: "s + (stream.msg ? stream.msg : "unknown error"));
				}

				if (status == Z_BUF_ERROR && stream.avail_in == 0)
				{
					throw std::runtime_error("zlib stream is truncated");
				}
			}

			result.resize(stream.total_out);
			return result;
		}

		std::string decompress_container(const std::string_view data)
		{
			enum codec : std::uint8_t
			{
				codec_zlib_first = 1,
				codec_zlib_last = 2,
				codec_lz4_first = 3,
				codec_lz4_last = 4,
			};

			byte_reader reader(data);

			const auto total_size = reader.read<std::uint64_t>();
			const auto flags = reader.read<std::array<std::uint8_t, 4>>();
			const auto block_codec = flags[3];

			const auto is_zlib = block_codec >= codec_zlib_first && block_codec <= codec_zlib_last;
			const auto is_lz4 = block_codec >= codec_lz4_first && block_codec <= codec_lz4_last;
			if (!is_zlib && !is_lz4)
			{
				throw std::runtime_error(std::format("unknown block codec {}", block_codec));
			}

			std::string result;
			result.resize(total_size);

			std::size_t written = 0;
			while (written < total_size)
			{
				const auto compressed_size = reader.read<std::uint32_t>();
				const auto decompressed_size = reader.read<std::uint32_t>();
				const auto block = reader.read_view(compressed_size);
				reader.skip(((compressed_size + 3) & ~3u) - compressed_size);

				if (written + decompressed_size > total_size)
				{
					throw std::runtime_error("block container exceeds its declared size");
				}

				if (is_lz4)
				{
					const auto size = LZ4_decompress_safe(block.data(), result.data() + written,
						static_cast<int>(compressed_size), static_cast<int>(decompressed_size));
					if (size != static_cast<int>(decompressed_size))
					{
						throw std::runtime_error(std::format("lz4 block at output offset 0x{:X} failed", written));
					}
				}
				else
				{
					const auto inflated = inflate_zlib(block, MAX_WBITS, decompressed_size);
					if (inflated.size() != decompressed_size)
					{
						throw std::runtime_error(std::format("zlib block at output offset 0x{:X} failed", written));
					}
					std::memcpy(result.data() + written, inflated.data(), inflated.size());
				}

				written += decompressed_size;
			}

			return result;
		}

		std::string decompress(const XFileHeader& header, const std::string_view data)
		{
			if (!header.compress)
			{
				return std::string(data);
			}

			switch (static_cast<compression>(header.compressType))
			{
			case compression::zlib:
				return inflate_zlib(data, MAX_WBITS, 0);
			case compression::none:
				return std::string(data);
			case compression::block_container:
				return decompress_container(data);
			}

			throw std::runtime_error(std::format("unknown compression type {}", header.compressType));
		}
	}

	fastfile read(const std::filesystem::path& path, const format& format)
	{
		std::string data;
		if (!utils::io::read_file(path.string(), &data))
		{
			throw std::runtime_error("cannot read file");
		}

		byte_reader reader(data);

		fastfile file{};
		reader.read(&file.header, header_size_before_stream_files);

		const std::string_view magic(file.header.header, sizeof(file.header.header));
		if (magic == format.signed_magic)
		{
			file.is_signed = true;
		}
		else if (magic != format.unsigned_magic)
		{
			throw std::runtime_error(std::format("unknown magic \"{}\"", magic));
		}

		if (file.header.version != format.version)
		{
			throw std::runtime_error(std::format("version {} is not {}", file.header.version, format.version));
		}

		file.stream_files.resize(file.header.imageCount);
		reader.read(file.stream_files.data(), file.stream_files.size() * sizeof(XStreamFile));
		reader.read(&file.header.baseFileLen, file_length_fields_size);

		const auto compressed = file.is_signed ? strip_auth_blocks(reader.rest()) : std::string(reader.rest());
		file.payload = decompress(file.header, compressed);

		return file;
	}
}
