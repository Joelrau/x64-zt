#include <std_include.hpp>
#include "xfile.hpp"

#include <io/game_files.hpp>

#include <zlib.h>
#include <lz4.h>

namespace zonetool::xfile
{
	namespace
	{
		constexpr std::size_t auth_block_size = 0x4000;
		constexpr std::size_t auth_blocks_per_group = 512;

		enum class s1_compression : std::uint8_t
		{
			zlib = 1,
			none = 3,
			block_container = 4,
		};

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

		enum block_codec : std::uint8_t
		{
			codec_zlib_size = 1,
			codec_zlib_speed = 2,
			codec_lz4hc = 3,
			codec_lz4 = 4,
			codec_none = 5,
		};

		void decompress_block(const std::uint8_t codec, const std::string_view block, char* destination, const std::size_t size)
		{
			switch (codec)
			{
			case codec_zlib_size:
			case codec_zlib_speed:
			{
				const auto inflated = inflate_zlib(block, MAX_WBITS, size);
				if (inflated.size() != size)
				{
					throw std::runtime_error("zlib block has the wrong size");
				}

				std::memcpy(destination, inflated.data(), size);
				return;
			}
			case codec_lz4hc:
			case codec_lz4:
				if (LZ4_decompress_safe(block.data(), destination, static_cast<int>(block.size()), static_cast<int>(size)) != static_cast<int>(size))
				{
					throw std::runtime_error("lz4 block failed");
				}
				return;
			case codec_none:
				if (block.size() != size)
				{
					throw std::runtime_error("uncompressed block has the wrong size");
				}

				std::memcpy(destination, block.data(), size);
				return;
			}

			throw std::runtime_error(std::format("unknown block codec {}", codec));
		}

		template <typename DecompressedSize>
		std::string decompress_blocks(byte_reader& reader, const std::uint64_t total_size, const std::uint8_t codec)
		{
			std::string result;
			result.resize(total_size);

			std::size_t written = 0;
			while (written < total_size)
			{
				const auto compressed_size = reader.read<std::uint32_t>();
				const auto decompressed_size = static_cast<std::size_t>(reader.read<DecompressedSize>());
				const auto stored_size = codec == codec_none ? decompressed_size : compressed_size;
				const auto block = reader.read_view(stored_size);
				reader.skip(((stored_size + 3) & ~std::size_t(3)) - stored_size);

				if (written + decompressed_size > total_size)
				{
					throw std::runtime_error("block container exceeds its declared size");
				}

				try
				{
					decompress_block(codec, block, result.data() + written, decompressed_size);
				}
				catch (const std::exception& e)
				{
					throw std::runtime_error(std::format("{} at output offset 0x{:X}", e.what(), written));
				}

				written += decompressed_size;
			}

			return result;
		}

		std::string decompress_container(const std::string_view data)
		{
			byte_reader reader(data);

			const auto total_size = reader.read<std::uint64_t>();
			const auto flags = reader.read<std::array<std::uint8_t, 4>>();
			return decompress_blocks<std::uint32_t>(reader, total_size, flags[3]);
		}

		std::string decompress(const XFileHeader& header, const std::string_view data)
		{
			if (!header.compress)
			{
				return std::string(data);
			}

			switch (static_cast<s1_compression>(header.compressType))
			{
			case s1_compression::zlib:
				return inflate_zlib(data, MAX_WBITS, 0);
			case s1_compression::none:
				return std::string(data);
			case s1_compression::block_container:
				return decompress_container(data);
			}

			throw std::runtime_error(std::format("unknown compression type {}", header.compressType));
		}

		std::string read_signed_data(byte_reader& reader, const bool is_signed)
		{
			return is_signed ? strip_auth_blocks(reader.rest()) : std::string(reader.rest());
		}

		void read_s1(byte_reader& reader, const format& format, fastfile& file)
		{
			XFileHeader header{};
			reader.read(&header, header_size_before_stream_files);

			file.stream_files.resize(header.imageCount);
			reader.read(file.stream_files.data(), file.stream_files.size() * sizeof(XStreamFile));
			reader.read(&header.baseFileLen, file_length_fields_size);

			const auto zone = decompress(header, read_signed_data(reader, file.is_signed));

			byte_reader zone_reader(zone);
			const auto size = zone_reader.read<std::uint64_t>();
			zone_reader.skip(sizeof(std::uint64_t));

			file.block_sizes.resize(format.block_count);
			zone_reader.read(file.block_sizes.data(), file.block_sizes.size() * sizeof(std::uint64_t));

			if (size != zone_reader.remaining())
			{
				throw std::runtime_error(std::format("zone size 0x{:X} does not match data size 0x{:X}", size, zone_reader.remaining()));
			}

			file.payload = zone_reader.rest();
		}

		constexpr std::size_t iw7_block_count = 10;

#pragma pack(push, 1)
		struct iw7_file_header
		{
			char magic[8];
			std::uint32_t version;
			std::uint8_t unused;
			std::uint8_t has_no_image_fastfile;
			std::uint8_t has_no_shared_fastfile;
			std::uint8_t unknown;
			std::uint32_t file_time_high;
			std::uint32_t file_time_low;
			std::uint64_t size;
			std::uint64_t unknown_sizes[2];
			std::uint64_t block_sizes[iw7_block_count];
			std::uint64_t unknown_array[8];
			std::uint32_t shared_stream_hash;
			std::uint32_t shared_stream_count;
			std::uint32_t image_stream_hash;
			std::uint32_t image_stream_count;
		};

		struct iw7_stream_file
		{
			std::uint64_t offset;
			std::uint64_t offset_end;
			std::uint16_t file_index;
			std::uint8_t is_localized;
			std::uint8_t pad[5];
		};
#pragma pack(pop)

		static_assert(sizeof(iw7_file_header) == 0xD0);
		static_assert(sizeof(iw7_stream_file) == 0x18);

		std::vector<XStreamFile> read_iw7_stream_files(byte_reader& reader, const std::size_t count)
		{
			std::vector<XStreamFile> files(count);
			for (auto& file : files)
			{
				const auto stream_file = reader.read<iw7_stream_file>();
				file.isLocalized = stream_file.is_localized;
				file.fileIndex = stream_file.file_index;
				file.offset = stream_file.offset;
				file.offsetEnd = stream_file.offset_end;
			}

			return files;
		}

		std::string decompress_iwc(const std::string_view data)
		{
			enum compressor : std::uint8_t
			{
				compressor_passthrough = 1,
				compressor_block = 2,
			};

			byte_reader reader(data);

			const auto type = reader.read<std::uint8_t>();
			if (reader.read_view(3) != "IWC")
			{
				throw std::runtime_error("compressor header has no IWC magic");
			}

			if (type == compressor_passthrough)
			{
				return std::string(reader.rest());
			}

			if (type != compressor_block)
			{
				throw std::runtime_error(std::format("unknown compressor {}", type));
			}

			const auto total_size = reader.read<std::uint64_t>();
			const auto block_size_and_codec = reader.read<std::uint32_t>();
			return decompress_blocks<std::uint64_t>(reader, total_size, static_cast<std::uint8_t>(block_size_and_codec >> 24));
		}

		void read_iw7(byte_reader& reader, const format& format, fastfile& file)
		{
			if (format.block_count != iw7_block_count)
			{
				throw std::runtime_error(std::format("IW7 zones have {} blocks", iw7_block_count));
			}

			const auto header = reader.read<iw7_file_header>();
			file.block_sizes.assign(std::begin(header.block_sizes), std::end(header.block_sizes));
			file.shared_stream_files = read_iw7_stream_files(reader, header.shared_stream_count);
			file.stream_files = read_iw7_stream_files(reader, header.image_stream_count);
			reader.skip(3 * sizeof(std::uint64_t));

			file.payload = decompress_iwc(read_signed_data(reader, file.is_signed));
		}
	}

	fastfile read(const std::filesystem::path& path, const format& format)
	{
		std::string data;
		if (!game_files::read(path, data))
		{
			throw std::runtime_error("cannot read file");
		}

		struct
		{
			char magic[8];
			std::uint32_t version;
		} identity{};
		byte_reader(data).read(&identity, sizeof(identity));

		fastfile file{};
		file.magic = std::string(identity.magic, sizeof(identity.magic));
		file.version = identity.version;

		if (file.magic == format.signed_magic)
		{
			file.is_signed = true;
		}
		else if (file.magic != format.unsigned_magic)
		{
			throw std::runtime_error(std::format("unknown magic \"{}\"", file.magic));
		}

		if (file.version != format.version)
		{
			throw std::runtime_error(std::format("version {} is not {}", file.version, format.version));
		}

		byte_reader reader(data);
		switch (format.container)
		{
		case container::s1:
			read_s1(reader, format, file);
			break;
		case container::iw7:
			read_iw7(reader, format, file);
			break;
		}

		return file;
	}
}
