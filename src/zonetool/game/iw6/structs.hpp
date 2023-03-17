#pragma once

namespace iw6
{
	namespace game
	{
		enum DvarSetSource : std::uint32_t
		{
			DVAR_SOURCE_INTERNAL = 0x0,
			DVAR_SOURCE_EXTERNAL = 0x1,
			DVAR_SOURCE_SCRIPT = 0x2,
			DVAR_SOURCE_UISCRIPT = 0x3,
			DVAR_SOURCE_SERVERCMD = 0x4,
			DVAR_SOURCE_NUM = 0x5,
		};

		enum DvarFlags : std::uint32_t
		{
			DVAR_FLAG_NONE = 0,
			DVAR_FLAG_SAVED = 0x1,
			DVAR_FLAG_LATCHED = 0x2,
			DVAR_FLAG_CHEAT = 0x4,
			DVAR_FLAG_REPLICATED = 0x8,
			DVAR_FLAG_WRITE = 0x800,
			DVAR_FLAG_READ = 0x2000,
		};

		enum dvar_type : std::int8_t
		{
			boolean = 0,
			value = 1,
			vec2 = 2,
			vec3 = 3,
			vec4 = 4,
			integer = 5,
			enumeration = 6,
			string = 7,
			color = 8,
			rgb = 9 // Color without alpha
		};

		union dvar_value
		{
			bool enabled;
			int integer;
			unsigned int unsignedInt;
			float value;
			float vector[4];
			const char* string;
			char color[4];
		};

		struct $A37BA207B3DDD6345C554D4661813EDD
		{
			int stringCount;
			const char* const* strings;
		};

		struct $9CA192F9DB66A3CB7E01DE78A0DEA53D
		{
			int min;
			int max;
		};

		struct $251C2428A496074035CACA7AAF3D55BD
		{
			float min;
			float max;
		};

		union dvar_limits
		{
			$A37BA207B3DDD6345C554D4661813EDD enumeration;
			$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
			$251C2428A496074035CACA7AAF3D55BD value;
			$251C2428A496074035CACA7AAF3D55BD vector;
		};

		struct dvar_t
		{
			const char* name;
			unsigned int flags;
			dvar_type type;
			bool modified;
			dvar_value current;
			dvar_value latched;
			dvar_value reset;
			dvar_limits domain;
		};

		struct CmdArgs
		{
			int nesting;
			int localClientNum[8];
			int controllerIndex[8];
			int argc[8];
			const char** argv[8];
		};

		struct cmd_function_s
		{
			cmd_function_s* next;
			const char* name;
			void(__cdecl* function)();
		};
	}
}
