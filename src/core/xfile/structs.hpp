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
		std::uint16_t isLocalized;
		std::uint16_t fileIndex;
		char pad[4];
		std::uint64_t offset;
		std::uint64_t offsetEnd;
	};

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
