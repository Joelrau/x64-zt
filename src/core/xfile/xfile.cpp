#include <std_include.hpp>
#include "xfile.hpp"

#include <io/game_files.hpp>
#include <utils/cryptography.hpp>

#include <zlib.h>
#include <lz4.h>
#include <lz4hc.h>

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

		constexpr std::size_t s2_header_version = 5;
		constexpr std::size_t s2_rsa_block_size = 256;
		constexpr std::size_t s2_aes_key_size = 24;
		constexpr std::size_t s2_aes_iv_size = 16;
		constexpr std::size_t s2_master_hash_offset = 0x150;
		constexpr std::string_view s2_master_magic = "S1ffs100";

#pragma pack(push, 1)
		struct s2_file_header
		{
			char magic[8];
			std::uint32_t header_version;
			std::uint32_t version;
			std::uint8_t compress;
			std::uint8_t compress_type;
			std::uint8_t unknown[2];
			std::uint32_t unknown2;
			std::uint32_t file_time_high;
			std::uint32_t file_time_low;
			std::uint32_t stream_key_count;
		};
#pragma pack(pop)

		static_assert(sizeof(s2_file_header) == 0x24);

		std::string decrypt_s2_block(const std::string_view block, const std::string_view key, const std::string_view hash)
		{
			return utils::cryptography::aes::crypt_ctr_little_endian(block, hash.substr(0, s2_aes_iv_size), key);
		}

		std::string decrypt_s2_auth_blocks(byte_reader& reader, const format& format)
		{
			if (!format.zone_key)
			{
				throw std::runtime_error("signed zones need a zone key");
			}

			const auto rsa_key = format.zone_key();
			if (rsa_key.empty())
			{
				throw std::runtime_error("cannot find the zone key");
			}

			const auto key_data = utils::cryptography::rsa::decrypt(std::string(reader.read_view(s2_rsa_block_size)), "sha256", rsa_key);
			if (key_data.size() != s2_aes_key_size + s2_aes_iv_size)
			{
				throw std::runtime_error("cannot decrypt the zone key");
			}

			const std::string_view key(key_data.data(), s2_aes_key_size);
			const auto master = decrypt_s2_block(reader.read_view(std::min(auth_block_size, reader.remaining())), key, std::string_view(key_data).substr(s2_aes_key_size));
			if (!master.starts_with(s2_master_magic))
			{
				throw std::runtime_error("auth header has the wrong magic");
			}

			std::string result;
			result.reserve(reader.remaining());

			for (std::size_t group = 0; reader.remaining(); group++)
			{
				const auto group_hash_offset = s2_master_hash_offset + group * sizeof(DB_AuthHash);
				if (group_hash_offset + sizeof(DB_AuthHash) > master.size())
				{
					throw std::runtime_error("zone has too many auth groups");
				}

				const auto hashes = decrypt_s2_block(reader.read_view(std::min(auth_block_size, reader.remaining())), key, std::string_view(master).substr(group_hash_offset));
				for (std::size_t block = 0; block < auth_blocks_per_group && reader.remaining(); block++)
				{
					const auto data = reader.read_view(std::min(auth_block_size, reader.remaining()));
					result.append(decrypt_s2_block(data, key, std::string_view(hashes).substr(block * sizeof(DB_AuthHash))));
				}
			}

			return result;
		}

		std::uint8_t s2_block_codec(const std::uint8_t codec)
		{
			switch (codec)
			{
			case 1:
				return codec_zlib_size;
			case 2:
			case 3:
				return codec_lz4;
			}

			throw std::runtime_error(std::format("block codec {} is Oodle, which is not supported", codec));
		}

		void read_s2(byte_reader& reader, const format& format, fastfile& file)
		{
			const auto header = reader.read<s2_file_header>();
			if (header.header_version != s2_header_version)
			{
				throw std::runtime_error(std::format("header version {} is not {}", header.header_version, s2_header_version));
			}

			file.stream_keys.resize(header.stream_key_count);
			reader.read(file.stream_keys.data(), file.stream_keys.size() * sizeof(XStreamFileKey));
			file.shared_stream_keys.resize(reader.read<std::uint32_t>());
			reader.read(file.shared_stream_keys.data(), file.shared_stream_keys.size() * sizeof(XStreamFileKey));
			reader.skip(2 * sizeof(std::uint64_t));

			const auto size = reader.read<std::uint64_t>();
			reader.skip(sizeof(std::uint64_t));
			const auto block_count = reader.read<std::uint64_t>();
			if (block_count != format.block_count)
			{
				throw std::runtime_error(std::format("zone has {} blocks, not {}", block_count, format.block_count));
			}

			file.block_sizes.resize(block_count);
			reader.read(file.block_sizes.data(), file.block_sizes.size() * sizeof(std::uint64_t));

			const auto data = file.is_signed ? decrypt_s2_auth_blocks(reader, format) : std::string(reader.rest());
			byte_reader container(data);
			const auto total_size = container.read<std::uint64_t>();
			const auto flags = container.read<std::array<std::uint8_t, 4>>();
			file.payload = decompress_blocks<std::uint32_t>(container, total_size, s2_block_codec(flags[3]));

			if (size != file.payload.size())
			{
				throw std::runtime_error(std::format("zone size 0x{:X} does not match data size 0x{:X}", size, file.payload.size()));
			}
		}
	}

	std::string decompress_s2_stream(const std::string_view data)
	{
		byte_reader reader(data);
		const auto total_size = reader.read<std::uint64_t>();
		const auto flags = reader.read<std::array<std::uint8_t, 4>>();
		return decompress_blocks<std::uint32_t>(reader, total_size, s2_block_codec(flags[3]));
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
			std::uint32_t s2_version;
		} identity{};
		byte_reader(data).read(&identity, sizeof(identity));

		fastfile file{};
		file.magic = std::string(identity.magic, sizeof(identity.magic));
		file.version = format.container == container::s2 ? identity.s2_version : identity.version;

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
		case container::s2:
			read_s2(reader, format, file);
			break;
		}

		return file;
	}
	namespace
	{
		constexpr std::size_t s2_block_size = 0x10000;
		constexpr std::uint8_t s2_write_codec = 3;
		constexpr int s2_lz4_level = 9;

		std::vector<std::string> compress_s2_blocks(const std::string_view payload)
		{
			const auto block_count = (payload.size() + s2_block_size - 1) / s2_block_size;
			std::vector<std::string> blocks(block_count);

			const auto worker_count = std::max(1u, std::min<unsigned>(std::thread::hardware_concurrency(), static_cast<unsigned>(block_count)));
			std::atomic_size_t next{};

			const auto work = [&]
			{
				for (auto index = next++; index < block_count; index = next++)
				{
					const auto source = payload.substr(index * s2_block_size, s2_block_size);
					auto& block = blocks[index];
					block.resize(LZ4_compressBound(static_cast<int>(source.size())));

					const auto size = LZ4_compress_HC(source.data(), block.data(), static_cast<int>(source.size()),
						static_cast<int>(block.size()), s2_lz4_level);
					if (size <= 0)
					{
						throw std::runtime_error("lz4 compression failed");
					}

					block.resize(size);
				}
			};

			std::vector<std::thread> workers;
			for (auto i = 1u; i < worker_count; i++)
			{
				workers.emplace_back(work);
			}

			work();
			for (auto& worker : workers)
			{
				worker.join();
			}

			return blocks;
		}

		template <typename T>
		void append(std::string& output, const T& value)
		{
			output.append(reinterpret_cast<const char*>(&value), sizeof(T));
		}

		void append_stream_keys(std::string& output, const std::vector<XStreamFileKey>& keys)
		{
			output.append(reinterpret_cast<const char*>(keys.data()), keys.size() * sizeof(XStreamFileKey));
		}
	}

	std::string write_s2(const format& format, const fastfile& file, const std::string_view payload)
	{
		if (format.container != container::s2)
		{
			throw std::runtime_error("write_s2 needs the s2 container");
		}

		if (file.is_signed)
		{
			throw std::runtime_error("signed s2 zones are not supported");
		}

		const auto blocks = compress_s2_blocks(payload);

		std::string container(sizeof(std::uint64_t), '\0');
		*reinterpret_cast<std::uint64_t*>(container.data()) = payload.size();
		container.append({0, 0, 1, static_cast<char>(s2_write_codec)});
		for (std::size_t i = 0; i < blocks.size(); i++)
		{
			const auto decompressed_size = std::min(s2_block_size, payload.size() - i * s2_block_size);
			append(container, static_cast<std::uint32_t>(blocks[i].size()));
			append(container, static_cast<std::uint32_t>(decompressed_size));
			container.append(blocks[i]);
			container.append(((blocks[i].size() + 3) & ~std::size_t(3)) - blocks[i].size(), '\0');
		}

		s2_file_header header{};
		std::memcpy(header.magic, format.unsigned_magic.data(), sizeof(header.magic));
		header.header_version = s2_header_version;
		header.version = format.version;
		header.compress = 1;
		header.compress_type = 2;
		header.unknown[0] = 1;
		header.unknown[1] = 0;
		header.unknown2 = static_cast<std::uint32_t>(s2_block_size);
		header.stream_key_count = static_cast<std::uint32_t>(file.stream_keys.size());

		std::string output;
		append(output, header);
		append_stream_keys(output, file.stream_keys);
		append(output, static_cast<std::uint32_t>(file.shared_stream_keys.size()));
		append_stream_keys(output, file.shared_stream_keys);

		const auto file_size = output.size() + 2 * sizeof(std::uint64_t) + 3 * sizeof(std::uint64_t)
			+ format.block_count * sizeof(std::uint64_t) + container.size();
		append(output, static_cast<std::uint64_t>(file_size));
		append(output, static_cast<std::uint64_t>(file_size));
		append(output, static_cast<std::uint64_t>(payload.size()));
		append(output, std::uint64_t{});
		append(output, static_cast<std::uint64_t>(format.block_count));
		for (std::size_t i = 0; i < format.block_count; i++)
		{
			append(output, i < file.block_sizes.size() ? file.block_sizes[i] : std::uint64_t{});
		}

		output.append(container);
		return output;
	}

}
