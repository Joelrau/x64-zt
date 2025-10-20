#pragma once

#include "memory.hpp"

#include "io/filesystem.hpp"
#include "io/assetmanager.hpp"

#include "csv.hpp"

#include "game/mode.hpp"
#include "game/shared.hpp"

#include <utils/memory.hpp>

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

#define ZONETOOL_INFO(__FMT__, ...) \
	printf("[ INFO ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)

#define ZONETOOL_ERROR(__FMT__, ...) \
	printf("[ ERROR ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)

#define ZONETOOL_FATAL(__FMT__, ...) \
	printf("[ FATAL ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__); \
	MessageBoxA(nullptr, &utils::string::va("Oops! An unexpected error occured. Error was: \n" __FMT__ "\n\nZoneTool must be restarted to resolve the error. Last error code reported by windows: 0x%08X (%u)", __VA_ARGS__, GetLastError(), GetLastError())[0], nullptr, MB_ICONERROR); \
	std::quick_exit(EXIT_FAILURE)

#define ZONETOOL_WARNING(__FMT__, ...) \
	printf("[ WARNING ][ %s ]: " __FMT__ "\n", zonetool::strip_template(__FUNCTION__), __VA_ARGS__)

namespace zonetool
{
	struct zonetool_globals_t
	{
		bool verify;
		bool dump;
		bool dump_referenced;
		bool dump_csv;
		game::game_mode target_game;
		filesystem::file csv_file;
	};
}
