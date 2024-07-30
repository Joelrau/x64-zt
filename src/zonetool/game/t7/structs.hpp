#pragma once
#include <d3d11.h>

namespace t7
{
	namespace game
	{
		typedef float vec_t;
		typedef vec_t vec2_t[2];
		typedef vec_t vec3_t[3];
		typedef vec_t vec4_t[4];

		typedef uint32_t ScrString_t;
		typedef ScrString_t ScrString;

		enum ControllerIndex_t
		{
			INVALID_CONTROLLER_PORT = -1,
			CONTROLLER_INDEX_FIRST = 0x0,
			CONTROLLER_INDEX_0 = 0x0,
			CONTROLLER_INDEX_1 = 0x1,
			CONTROLLER_INDEX_2 = 0x2,
			CONTROLLER_INDEX_3 = 0x3,
			CONTROLLER_INDEX_COUNT = 0x4,
		};

		enum LocalClientNum_t
		{
			INVALID_LOCAL_CLIENT = -1,
			LOCAL_CLIENT_0 = 0x0,
			LOCAL_CLIENT_FIRST = 0x0,
			LOCAL_CLIENT_KEYBOARD_AND_MOUSE = 0x0,
			LOCAL_CLIENT_1 = 0x1,
			LOCAL_CLIENT_2 = 0x2,
			LOCAL_CLIENT_3 = 0x3,
			LOCAL_CLIENT_COUNT = 0x4,
		};

		enum eGameModes
		{
			MODE_GAME_MATCHMAKING_PLAYLIST = 0x0,
			MODE_GAME_MATCHMAKING_MANUAL = 0x1,
			MODE_GAME_DEFAULT = 0x2,
			MODE_GAME_LEAGUE = 0x3,
			MODE_GAME_THEATER = 0x4,
			MODE_GAME_COUNT = 0x5,
			MODE_GAME_INVALID = 0x5,
		};

		enum eModes
		{
			MODE_ZOMBIES = 0x0,
			MODE_MULTIPLAYER = 0x1,
			MODE_CAMPAIGN = 0x2,
			MODE_COUNT = 0x3,
			MODE_INVALID = 0x3,
			MODE_FIRST = 0x0,
		};

		typedef uint32_t dvarStrHash_t;

		enum dvarType_t
		{
			DVAR_TYPE_INVALID = 0x0,
			DVAR_TYPE_BOOL = 0x1,
			DVAR_TYPE_FLOAT = 0x2,
			DVAR_TYPE_FLOAT_2 = 0x3,
			DVAR_TYPE_FLOAT_3 = 0x4,
			DVAR_TYPE_FLOAT_4 = 0x5,
			DVAR_TYPE_INT = 0x6,
			DVAR_TYPE_ENUM = 0x7,
			DVAR_TYPE_STRING = 0x8,
			DVAR_TYPE_COLOR = 0x9,
			DVAR_TYPE_INT64 = 0xA,
			DVAR_TYPE_UINT64 = 0xB,
			DVAR_TYPE_LINEAR_COLOR_RGB = 0xC,
			DVAR_TYPE_COLOR_XYZ = 0xD,
			DVAR_TYPE_COLOR_LAB = 0xE,
			DVAR_TYPE_SESSIONMODE_BASE_DVAR = 0xF,
			DVAR_TYPE_COUNT = 0x10,
		};

		enum dvarFlags_e
		{
			DVAR_NONE = 0,
			DVAR_ARCHIVE = 1 << 0,
			DVAR_USERINFO = 1 << 1,
			DVAR_SERVERINFO = 1 << 2,
			DVAR_SYSTEMINFO = 1 << 3,
			DVAR_LATCH = 1 << 4,
			DVAR_ROM = 1 << 5,
			DVAR_SAVED = 1 << 6,
			DVAR_INIT = 1 << 7,
			DVAR_CHEAT = 1 << 8,
			//DVAR_UNKNOWN = 1 << 9,
			DVAR_EXTERNAL = 1 << 10,
			//DVAR_UNKNOWN3x = 1 << 11-13,
			DVAR_SESSIONMODE = 1 << 15
		};

		struct dvar_t;

		struct DvarValue
		{
			union
			{
				bool enabled;
				int integer;
				uint32_t unsignedInt;
				int64_t integer64;
				uint64_t unsignedInt64;
				float value;
				vec4_t vector;
				const char* string;
				byte color[4];
				const dvar_t* indirect[3];
			} value;

			uint64_t encryptedValue;
		};

		union DvarLimits
		{
			struct
			{
				int stringCount;
				const char** strings;
			} enumeration;

			struct
			{
				int min;
				int max;
			} integer;

			struct
			{
				int64_t min;
				int64_t max;
			} integer64;

			struct
			{
				uint64_t min;
				uint64_t max;
			} unsignedInt64;

			struct
			{
				float min;
				float max;
			} value;

			struct
			{
				vec_t min;
				vec_t max;
			} vector;
		};

		struct dvar_t
		{
			dvarStrHash_t name;
			const char* debugName;
			const char* description;
			unsigned int flags;
			dvarType_t type;
			bool modified;
			DvarValue current;
			DvarValue latched;
			DvarValue reset;
			DvarLimits domain;
			dvar_t* hashNext;
		};

		typedef void (*xcommand_t)();

		struct cmd_function_s
		{
			cmd_function_s* next;
			const char* name;
			const char* autoCompleteDir;
			const char* autoCompleteExt;
			xcommand_t function;
			int autoComplete;
		};

		struct CmdArgs
		{
			int nesting;
			int localClientNum[8];
			int controllerIndex[8];
			int argshift[8];
			int argc[8];
			const char** argv[8];
			char textPool[8192];
			const char* argvPool[512];
			int usedTextPool[8];
			int totalUsedArgvPool;
			int totalUsedTextPool;
		};

		struct va_info_t
		{
			char va_string[4][1024];
			int index;
		};

		struct TLSData
		{
			va_info_t* vaInfo;
			jmp_buf* errorJmpBuf;
			void* traceInfo;
			CmdArgs* cmdArgs;
			void* errorData;
		};
	}
}