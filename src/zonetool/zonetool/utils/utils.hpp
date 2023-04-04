#pragma once

#include "memory.hpp"

#include "io/filesystem.hpp"
#include "io/assetmanager.hpp"

#include "csv.hpp"

#include "shader.hpp"
#include "game/mode.hpp"
#include "game/shared.hpp"

#include <utils/memory.hpp>

#define JSON_DIAGNOSTICS 1
#include "json.hpp"
using json = nlohmann::json;
using ordered_json = nlohmann::ordered_json;

namespace nlohmann
{
	static inline std::vector<std::uint8_t> get_object_bytes(json object)
	{
		if (!object.is_object())
		{
			throw (detail::concat("type must be object, but is ", object.type_name()));
		}
		auto bytes = object["bytes"];
		if (bytes.is_null())
		{
			throw ("object doesn't have \"bytes\" field");
		}
		std::vector<std::uint8_t> m_bytes(bytes.size());
		for (auto i = 0; i < bytes.size(); i++)
		{
			auto byte = bytes[i].get<std::uint8_t>();
			m_bytes[i] = byte;
		}
		return m_bytes;
	}
}

#define MAX_ZONE_SIZE (1024ull * 1024ull * 1024ull) * 2ull
#define MAX_MEM_SIZE (1024ull * 1024ull * 1024ull) * 2ull

#define ZONETOOL_INFO(__FMT__,...) \
	printf("[ INFO ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__,...) \
	printf("[ ERROR ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__,...) \
	printf("[ FATAL ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__); \
	MessageBoxA(nullptr, &utils::string::va("Oops! An unexpected error occured. Error was: \n" __FMT__ "\n\nZoneTool must be restarted to resolve the error. Last error code reported by windows: 0x%08X (%u)", __VA_ARGS__, GetLastError(), GetLastError())[0], nullptr, MB_ICONERROR); \
	std::quick_exit(EXIT_FAILURE)

#define ZONETOOL_WARNING(__FMT__,...) \
	printf("[ WARNING ][ " __FUNCTION__ " ]: " __FMT__ "\n", __VA_ARGS__)

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
	struct Xzone_memory
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

	struct zonetool_globals_t
	{
		bool verify;
		bool dump;
		game::game_mode target_game;
		filesystem::file csv_file;
	};
}