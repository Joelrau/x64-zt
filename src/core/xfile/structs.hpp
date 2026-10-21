#pragma once

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
		std::uint16_t unknown[2];
		std::uint16_t slot;
	};

	struct XStreamFileLocation
	{
		static constexpr std::uint16_t fastfile_index = 0;
		// s2 marks imagefile paks with 0x8000 (0x4000 is imagestream) and keeps the pak index in the low 13 bits (sub_14007FCC0);
		// a client opens the zone's own <zone>.pak for this index like h1-mod does for XStreamFile::zone_pak_file_index
		static constexpr std::uint16_t imagefile_pak_flag = 0x8000;
		static constexpr std::uint16_t pak_index_mask = 0x1FFF;
		static constexpr std::uint16_t zone_pak_index = imagefile_pak_flag | XStreamFile::zone_pak_file_index;

		std::uint64_t offset; // the game packs offset into 36 bits and size into 28
		std::uint32_t size;
		std::uint8_t isLocalized;
		char pad;
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
