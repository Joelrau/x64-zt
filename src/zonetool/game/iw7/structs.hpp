#pragma once
#include <d3d11.h>

namespace iw7
{
	namespace game
	{
		typedef float vec_t;
		typedef vec_t vec2_t[2];
		typedef vec_t vec3_t[3];
		typedef vec_t vec4_t[4];

		enum scr_string_t : std::int32_t
		{
		};

		enum GameModeType : std::uint32_t
		{
			GAME_MODE_NONE = 0x0,
			GAME_MODE_SP = 0x1,
			GAME_MODE_MP = 0x2,
			GAME_MODE_CP = 0x3,
		};

		enum Com_GameMode_Feature : std::int32_t
		{
			FEATURE_GRAVITY = 33,
			FEATURE_TIMESCALE = 69,
		};

		enum errorParm
		{
			ERR_FATAL = 0,
			ERR_DROP = 1,
		};

		enum Sys_Folder
		{
			SF_ZONE = 0x0,
			SF_ZONE_LOC = 0x1,
			SF_VIDEO = 0x2,
			SF_VIDEO_LOC = 0x3,
			SF_PAKFILE = 0x4,
			SF_PAKFILE_LOC = 0x5,
			SF_COUNT = 0x6,
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
			void(__fastcall* function)();
		};

		struct SvCommandInfo
		{
			const char* name;
			void(__fastcall* function)();
			cmd_function_s svvar;
		};

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
			DVAR_FLAG_NETWORK = 0x10,
			DVAR_FLAG_EXTERNAL = 0x100,
			DVAR_FLAG_WRITE = 0x800,
			DVAR_FLAG_READ = 0x2000,
		};

		enum DvarType : std::uint8_t
		{
			DVAR_TYPE_BOOL = 0x0,
			DVAR_TYPE_FLOAT = 0x1,
			DVAR_TYPE_FLOAT_2 = 0x2,
			DVAR_TYPE_FLOAT_3 = 0x3,
			DVAR_TYPE_FLOAT_4 = 0x4,
			DVAR_TYPE_INT = 0x5,
			DVAR_TYPE_ENUM = 0x6,
			DVAR_TYPE_STRING = 0x7,
			DVAR_TYPE_COLOR = 0x8,
			DVAR_TYPE_FLOAT_3_COLOR = 0x9,
			DVAR_TYPE_COUNT = 0xA,

			boolean = 0,
			value = 1,
			vec2 = 2,
			vec3 = 3,
			vec4 = 4,
			integer = 5,
			enumeration = 6,
			string = 7,
			color = 8,
			rgb = 9, // Color without alpha
		};

		union DvarValue
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

		union DvarLimits
		{
			$A37BA207B3DDD6345C554D4661813EDD enumeration;
			$9CA192F9DB66A3CB7E01DE78A0DEA53D integer;
			$251C2428A496074035CACA7AAF3D55BD value;
			$251C2428A496074035CACA7AAF3D55BD vector;
		};

		struct dvar_t
		{
			unsigned int checksum;
			unsigned int flags;
			char level;
			DvarType type;
			bool modified;
			DvarValue current;
			DvarValue latched;
			DvarValue reset;
			DvarLimits domain;
			bool(__fastcall* domainFunc)(dvar_t*, DvarValue);
			dvar_t* hashNext;
		}; static_assert(sizeof(dvar_t) == 96);

		enum svscmd_type
		{
			SV_CMD_CAN_IGNORE = 0x0,
			SV_CMD_RELIABLE = 0x1,
		};

		enum keyNum_t
		{
			K_NONE = 0x0,
			K_FIRSTGAMEPADBUTTON_RANGE_1 = 0x1,
			K_BUTTON_A = 0x1,
			K_BUTTON_B = 0x2,
			K_BUTTON_X = 0x3,
			K_BUTTON_Y = 0x4,
			K_BUTTON_LSHLDR = 0x5,
			K_BUTTON_RSHLDR = 0x6,
			K_LASTGAMEPADBUTTON_RANGE_1 = 0x6,
			K_BS = 0x8,
			K_TAB = 0x9,
			K_ENTER = 0xD,
			K_FIRSTGAMEPADBUTTON_RANGE_2 = 0xE,
			K_BUTTON_START = 0xE,
			K_BUTTON_BACK = 0xF,
			K_BUTTON_LSTICK = 0x10,
			K_BUTTON_RSTICK = 0x11,
			K_BUTTON_LTRIG = 0x12,
			K_BUTTON_RTRIG = 0x13,
			K_DPAD_UP = 0x14,
			K_FIRSTDPAD = 0x14,
			K_DPAD_DOWN = 0x15,
			K_DPAD_LEFT = 0x16,
			K_DPAD_RIGHT = 0x17,
			K_BUTTON_LSTICK_ALTIMAGE2 = 0x10,
			K_BUTTON_RSTICK_ALTIMAGE2 = 0x11,
			K_BUTTON_LSTICK_ALTIMAGE = 0xBC,
			K_BUTTON_RSTICK_ALTIMAGE = 0xBD,
			K_LASTDPAD = 0x17,
			K_LASTGAMEPADBUTTON_RANGE_2 = 0x17,
			K_ESCAPE = 0x1B,
			K_FIRSTGAMEPADBUTTON_RANGE_3 = 0x1C,
			K_APAD_UP = 0x1C,
			K_FIRSTAPAD = 0x1C,
			K_APAD_DOWN = 0x1D,
			K_APAD_LEFT = 0x1E,
			K_APAD_RIGHT = 0x1F,
			K_LASTAPAD = 0x1F,
			K_LASTGAMEPADBUTTON_RANGE_3 = 0x1F,
			K_SPACE = 0x20,
			K_GRAVE = 0x60,
			K_TILDE = 0x7E,
			K_BACKSPACE = 0x7F,
			K_ASCII_FIRST = 0x80,
			K_ASCII_181 = 0x80,
			K_ASCII_191 = 0x81,
			K_ASCII_223 = 0x82,
			K_ASCII_224 = 0x83,
			K_ASCII_225 = 0x84,
			K_ASCII_228 = 0x85,
			K_ASCII_229 = 0x86,
			K_ASCII_230 = 0x87,
			K_ASCII_231 = 0x88,
			K_ASCII_232 = 0x89,
			K_ASCII_233 = 0x8A,
			K_ASCII_236 = 0x8B,
			K_ASCII_241 = 0x8C,
			K_ASCII_242 = 0x8D,
			K_ASCII_243 = 0x8E,
			K_ASCII_246 = 0x8F,
			K_ASCII_248 = 0x90,
			K_ASCII_249 = 0x91,
			K_ASCII_250 = 0x92,
			K_ASCII_252 = 0x93,
			K_END_ASCII_CHARS = 0x94,
			K_COMMAND = 0x96,
			K_CAPSLOCK = 0x97,
			K_POWER = 0x98,
			K_PAUSE = 0x99,
			K_UPARROW = 0x9A,
			K_DOWNARROW = 0x9B,
			K_LEFTARROW = 0x9C,
			K_RIGHTARROW = 0x9D,
			K_ALT = 0x9E,
			K_CTRL = 0x9F,
			K_SHIFT = 0xA0,
			K_INS = 0xA1,
			K_DEL = 0xA2,
			K_PGDN = 0xA3,
			K_PGUP = 0xA4,
			K_HOME = 0xA5,
			K_END = 0xA6,
			K_F1 = 0xA7,
			K_F2 = 0xA8,
			K_F3 = 0xA9,
			K_F4 = 0xAA,
			K_F5 = 0xAB,
			K_F6 = 0xAC,
			K_F7 = 0xAD,
			K_F8 = 0xAE,
			K_F9 = 0xAF,
			K_F10 = 0xB0,
			K_F11 = 0xB1,
			K_F12 = 0xB2,
			K_F13 = 0xB3,
			K_F14 = 0xB4,
			K_F15 = 0xB5,
			K_KP_HOME = 0xB6,
			K_KP_UPARROW = 0xB7,
			K_KP_PGUP = 0xB8,
			K_KP_LEFTARROW = 0xB9,
			K_KP_5 = 0xBA,
			K_KP_RIGHTARROW = 0xBB,
			K_KP_END = 0xBC,
			K_KP_DOWNARROW = 0xBD,
			K_KP_PGDN = 0xBE,
			K_KP_ENTER = 0xBF,
			K_KP_INS = 0xC0,
			K_KP_DEL = 0xC1,
			K_KP_SLASH = 0xC2,
			K_KP_MINUS = 0xC3,
			K_KP_PLUS = 0xC4,
			K_KP_NUMLOCK = 0xC5,
			K_KP_STAR = 0xC6,
			K_KP_EQUALS = 0xC7,
			K_MOUSE1 = 0xC8,
			K_MOUSE2 = 0xC9,
			K_MOUSE3 = 0xCA,
			K_MOUSE4 = 0xCB,
			K_MOUSE5 = 0xCC,
			K_MWHEELDOWN = 0xCD,
			K_MWHEELUP = 0xCE,
			K_AUX1 = 0xCF,
			K_AUX2 = 0xD0,
			K_AUX3 = 0xD1,
			K_AUX4 = 0xD2,
			K_AUX5 = 0xD3,
			K_AUX6 = 0xD4,
			K_AUX7 = 0xD5,
			K_AUX8 = 0xD6,
			K_AUX9 = 0xD7,
			K_AUX10 = 0xD8,
			K_AUX11 = 0xD9,
			K_AUX12 = 0xDA,
			K_AUX13 = 0xDB,
			K_AUX14 = 0xDC,
			K_AUX15 = 0xDD,
			K_AUX16 = 0xDE,
			K_LAST_KEY = 0xDF
		};

		struct KeyState
		{
			int down;
			int unk;
			int repeats;
			int binding;
		};

		struct field_t
		{
			int cursor;
			int scroll;
			int drawWidth;
			int widthInPixels;
			float charHeight;
			int fixedSize;
			char buffer[256];
		};

		struct PlayerKeyState
		{
			int overstrikeMode;
			int anyKeyDown;
			KeyState keys[256];
			field_t test;
			char __pad0[8];
		}; static_assert(sizeof(PlayerKeyState) == 0x1128);

		struct ScreenPlacement
		{
			vec2_t scaleVirtualToReal;
			vec2_t scaleVirtualToFull;
			vec2_t scaleRealToVirtual;
			vec2_t realViewportPosition;
			vec2_t realViewportSize;
			vec2_t virtualViewableMin;
			vec2_t virtualViewableMax;
			vec2_t realViewableMin;
			vec2_t realViewableMax;
			vec2_t virtualAdjustableMin;
			vec2_t virtualAdjustableMax;
			vec2_t realAdjustableMin;
			vec2_t realAdjustableMax;
			vec2_t subScreenLeft;
		};

		enum netadrtype_t
		{
			NA_BOT = 0x0,
			NA_BAD = 0x1,
			NA_LOOPBACK = 0x2,
			NA_BROADCAST = 0x3,
			NA_IP = 0x4,
		};

		enum netsrc_t
		{
			NS_CLIENT1 = 0x0,
			NS_MAXCLIENTS = 0x1,
			NS_SERVER = 0x2,
			NS_PACKET = 0x3,
			NS_INVALID_NETSRC = 0x4,
		};

		struct netadr_s
		{
			netadrtype_t type;
			unsigned char ip[4];
			unsigned __int16 port;
			netsrc_t localNetID;
			unsigned int addrHandleIndex;
		};

		struct msg_t
		{
			int overflowed;
			int readOnly;
			char* data;
			char* splitData;
			int maxsize;
			int cursize;
			int splitSize;
			int readcount;
			int bit;
			int lastEntityRef;
			netsrc_t targetLocalNetID;
			int useZlib;
		};

		enum CriticalSection : std::int32_t
		{
			CRITSECT_DEMONWARE = 39,
		};

		enum ThreadContext
		{
			THREAD_CONTEXT_MAIN = 0,
			THREAD_CONTEXT_BACKEND = 1,
			THREAD_CONTEXT_WORKER0 = 2,
			THREAD_CONTEXT_WORKER1 = 3,
			THREAD_CONTEXT_WORKER2 = 4,
			THREAD_CONTEXT_WORKER3 = 5,
			THREAD_CONTEXT_WORKER4 = 6,
			THREAD_CONTEXT_WORKER5 = 7,
			THREAD_CONTEXT_WORKER6 = 8,
			THREAD_CONTEXT_WORKER7 = 9,
			THREAD_CONTEXT_SERVER = 10,
			THREAD_CONTEXT_CINEMATIC = 11,
			THREAD_CONTEXT_WINDOW = 12,
			THREAD_CONTEXT_INPUT = 13,
			THREAD_CONTEXT_DATABASE = 14,
			THREAD_CONTEXT_SOUND_STREAM = 15,
			THREAD_CONTEXT_SOUND = 16,
			THREAD_CONTEXT_SOUND_DECODE = 17,
			THREAD_CONTEXT_UNKNOWN18 = 18,
			THREAD_CONTEXT_UNKNOWN19 = 19,
			THREAD_CONTEXT_RECIPE = 20,
		};

		enum StatsGroup
		{
			STATSGROUP_RANKED = 0x0,
			STATSGROUP_COOP = 0x1,
			STATSGROUP_COMMON = 0x2,
			STATSGROUP_RANKEDLOADOUTS = 0x3,
			STATSGROUP_PRIVATELOADOUTS = 0x4,
			//STATSGROUP_NONGAME = 0x5,
			STATSGROUP_COUNT = 0x5,
			STATSGROUP_IGNORE = 0x6,
			STATSGROUP_FIRST = 0x0,
		};

		namespace ddl
		{
			enum DDLType
			{
				DDL_INVALID_TYPE = 0xFFFFFFFF,
				DDL_BYTE_TYPE = 0x0,
				DDL_SHORT_TYPE = 0x1,
				DDL_UINT_TYPE = 0x2,
				DDL_INT_TYPE = 0x3,
				DDL_UINT64_TYPE = 0x4,
				DDL_FLOAT_TYPE = 0x5,
				DDL_FIXEDPOINT_TYPE = 0x6,
				DDL_STRING_TYPE = 0x7,
				DDL_STRUCT_TYPE = 0x8,
				DDL_ENUM_TYPE = 0x9,
			};

			union DDLValue
			{
				int intValue;
				unsigned int uintValue;
				unsigned __int64 uint64Value;
				float floatValue;
				float fixedPointValue;
				const char* stringPtr;
			};

			struct DDLMember
			{
				const char* name;
				int index;
				int bitSize;
				int limitSize;
				int offset;
				int type;
				int externalIndex;
				unsigned int rangeLimit;
				bool isArray;
				int arraySize;
				int enumIndex;
			};

			struct DDLState
			{
				bool isValid;
				int offset;
				int arrayIndex;
				DDLMember* member;
				//const DDLDef* ddlDef;
			};

			struct DDLContext
			{

			};
		}
		using namespace ddl;

		struct DBFile
		{
			char name[64];
			void* handle;
			char __pad0[46];
			bool isSecure;
			char __pad1[49];
		}; assert_sizeof(DBFile, 0xA8);
		assert_offsetof(DBFile, isSecure, 118);

		struct db_internal_state
		{
			int dummy;
		};

		struct db_z_stream_s
		{
			unsigned char* next_in;
			unsigned int avail_in;
			unsigned int total_in;
			unsigned char* next_out;
			unsigned int avail_out;
			unsigned int total_out;
			char* msg;
			db_internal_state* state;
			unsigned __int8* (__fastcall* zalloc)(unsigned __int8*, unsigned int, unsigned int);
			void(__fastcall* zfree)(unsigned __int8*, unsigned __int8*);
			unsigned __int8* opaque;
			int data_type;
			unsigned long adler;
			unsigned long reserved;
		};

		enum DB_CompressorType
		{
			DB_COMPRESSOR_INVALID = 0x0,
			DB_COMPRESSOR_PASSTHROUGH = 0x1,
			DB_COMPRESSOR_BLOCK = 0x2,
		};

		struct DB_ReadStream
		{
			unsigned char* next_in;
			unsigned __int64 avail_in;
			unsigned __int64 total_in;
			unsigned char* next_out;
			unsigned __int64 avail_out;
			unsigned __int64 total_out;
			DB_CompressorType compressor;
		};

		struct DB_LoadData
		{
			DBFile* file;
			unsigned __int8* fileBuffer;
			unsigned __int64 readSize;
			unsigned __int64 completedReadSize;
			unsigned __int64 offset;
			unsigned __int8* start_in;
			DB_ReadStream stream;
		};
	}
}