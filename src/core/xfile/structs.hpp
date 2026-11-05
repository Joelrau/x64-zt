#pragma once

#include <cstddef>
#include <cstdint>

namespace zonetool
{
#pragma pack(push, 1)
	struct XFileHeader
	{
		char header[8];
		std::uint32_t version;
		std::uint8_t compress;
		std::uint8_t compressType;
		std::uint8_t sizeOfPointer;
		std::uint8_t sizeOfLong;
		std::uint32_t fileTimeHigh;
		std::uint32_t fileTimeLow;
		std::uint32_t imageCount;
		std::uint64_t baseFileLen;
		std::uint64_t totalFileLen;
	};

	template <std::size_t num_streams>
	struct XZoneMemory
	{
		std::uint64_t size;
		std::uint64_t externalsize;
		std::uint64_t streams[num_streams];
	};

	struct XStreamFile
	{
		static constexpr std::uint16_t zone_pak_file_index = 96;

		std::uint16_t isLocalized;
		std::uint16_t fileIndex;
		char pad[4];
		std::uint64_t offset;
		std::uint64_t offsetEnd;
	};

	struct XStreamFileKey
	{
		static constexpr std::uint16_t unresolved_slot = 0xFFFF;

		std::uint64_t hash;
		std::uint16_t index;
		std::uint16_t unk_0xA;
		std::uint8_t isLocalized;
		std::uint8_t unk_0xD;
		std::uint16_t slot;
	};

	struct XStreamFileLocation
	{
		static constexpr std::uint16_t fastfile_index = 0;
		static constexpr std::uint16_t imagefile_pak_flag = 0x8000;
		static constexpr std::uint16_t pak_index_mask = 0x1FFF;
		static constexpr std::uint16_t zone_pak_index = imagefile_pak_flag | XStreamFile::zone_pak_file_index;

		std::uint64_t offset;
		std::uint32_t size;
		std::uint8_t isLocalized;
		std::uint8_t unk_0xD;
		std::uint16_t fileIndex;
	};

	static_assert(sizeof(XStreamFileLocation) == sizeof(XStreamFileKey));

	struct DB_AuthSignature
	{
		unsigned char bytes[256];
	};

	struct DB_AuthHash
	{
		unsigned char bytes[32];
	};

	struct XPakHeader
	{
		char header[8];
		std::int32_t version;
		unsigned char unknown[16];
		DB_AuthHash hash;
		DB_AuthSignature signature;
	};
#pragma pack(pop)

	struct XAssetList
	{
		std::int32_t stringCount;
		std::uint64_t strings;
		std::int32_t assetCount;
		std::uint64_t assets;
		std::uint64_t globals;
	};

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

#pragma pack(push, 1)
	struct s2_file_header
	{
		char magic[8];
		std::uint32_t header_version;
		std::uint32_t version;
		std::uint8_t compress;
		std::uint8_t compress_type;
		std::uint8_t stream_compress_type;
		std::uint8_t unknown;
		std::uint32_t unknown2;
		std::uint32_t file_time_high;
		std::uint32_t file_time_low;
		std::uint32_t stream_key_count;
	};
#pragma pack(pop)

	static_assert(sizeof(s2_file_header) == 0x24);

	enum XFileBlock
	{
		XFILE_BLOCK_TEMP = 0x0,
		XFILE_BLOCK_PHYSICAL = 0x1,
		XFILE_BLOCK_RUNTIME = 0x2,
		XFILE_BLOCK_VIRTUAL = 0x3,
		XFILE_BLOCK_LARGE = 0x4,
		XFILE_BLOCK_CALLBACK = 0x5,
		XFILE_BLOCK_SCRIPT = 0x6,
		MAX_XFILE_COUNT = 0x7,
	};
}
