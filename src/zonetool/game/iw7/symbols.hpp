#pragma once

#define WEAK __declspec(selectany)

namespace iw7
{
	namespace game
	{
		/***************************************************************
		 * Functions
		 **************************************************************/

		WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x140B8D830 };

		WEAK symbol<void()> Com_Quit_f{ 0x140BADC90 };

		WEAK symbol<bool()> Com_FrontEnd_IsInFrontEnd{ 0x1405AE6C0 };
		WEAK symbol<void()> Com_FrontEnd_ExitFrontEnd{ 0x1405AE4F0 };
		WEAK symbol<bool()> Com_FrontEndScene_IsActive{ 0x1405AEBA0 };
		WEAK symbol<void()> Com_FrontEndScene_ShutdownAndDisable{ 0x1405AEFB0 };
		WEAK symbol<void()> Com_FrontEndScene_Shutdown{ 0x1405AED00 };

		WEAK symbol<void(GameModeType)> Com_GameMode_SetDesiredGameMode{ 0x1405AFDA0 };
		WEAK symbol<GameModeType()> Com_GameMode_GetActiveGameMode{ 0x1405AFD50 };

		WEAK symbol<bool()> Com_IsAnyLocalServerStarting{ 0x140BAD9C0 };
		WEAK symbol<bool()> Com_IsAnyLocalServerRunning{ 0x140BAD9A0 };

		WEAK symbol<void(const char* localizedMessage, const char* localizedTitle)> Com_SetLocalizedErrorMessage{ 0x140BAF300 };

		WEAK symbol<void()> Com_SyncThreads{ 0x140BB02D0 };

		WEAK symbol<void(const char* finalmsg)> Com_Shutdown{ 0x140BAFEA0 };

		WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x140B7C290 };
		WEAK symbol<void(int localClientNum, void(*))> Cbuf_AddCall{ 0x140B7C220 };
		WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer)> Cbuf_ExecuteBuffer{ 0x140B7C3B0 };
		WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer, void(int, int, const char*))> Cbuf_ExecuteBufferInternal{ 0x140B7C3C0 };

		WEAK symbol<void(int localClientNum, int andMask)> CL_Keys_RemoveCatcher{ 0x1409A9B00 };

		WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{ 0x140B7D040 };
		WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{ 0x140B7C8F0 };
		WEAK symbol<void(const char* cmdName)> Cmd_RemoveCommand{ 0x140B7D630 };
		WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{ 0x140B7D850 };
		WEAK symbol<void()> Cmd_EndTokenizeString{ 0x140B7CC90 };

		WEAK symbol<const char* (const DDLState* state, int enumValue)> DDL_Lookup_GetEnumString{ 0x140030430 };
		WEAK symbol<bool(const DDLState* state)> DDL_StateIsLeaf{ 0x14002E3C0 };
		WEAK symbol<DDLType(const DDLState* state)> DDL_GetType{ 0x14002E5A0 };
		WEAK symbol<DDLValue(const DDLState* state, const DDLContext* ddlContext)> DDL_GetValue{ 0x14002F5E0 };

		WEAK symbol<dvar_t* (const char* name, bool value,
			unsigned int flags, const char* description)> Dvar_RegisterBool{ 0x140CEB380 };
		WEAK symbol<dvar_t* (const char* name, int value, int min, int max,
			unsigned int flags, const char* description)> Dvar_RegisterInt{ 0x140CEB920 };
		WEAK symbol<dvar_t* (const char* name, float value, float min,
			float max, unsigned int flags, const char* description)> Dvar_RegisterFloat{ 0x140CEB890 };
		WEAK symbol<dvar_t* (const char* name, const char* value,
			unsigned int flags, const char* description)> Dvar_RegisterString{ 0x140CEBD50 };
		WEAK symbol<dvar_t* (const char* name, float x, float y,
			float min, float max, unsigned int flags, const char* description)> Dvar_RegisterVec2{ 0x140CEBF50 };
		WEAK symbol<dvar_t* (const char* name, float x, float y, float z,
			float min, float max, unsigned int flags, const char* description)> Dvar_RegisterVec3{ 0x140CEBFE0 };
		WEAK symbol<dvar_t* (const char* name, float x, float y, float z,
			float w, float min, float max, unsigned int flags, const char* description)> Dvar_RegisterVec4{ 0x140CEC110 };

		WEAK symbol<dvar_t* (unsigned int checksum, const char* string, DvarSetSource source)> Dvar_SetFromStringByChecksum{ 0x140CECDB0 };
		WEAK symbol<dvar_t* (const char* name, const char* string, DvarSetSource source)> Dvar_SetFromStringByName{ 0x140CECF30 };
		WEAK symbol<void(dvar_t* dvar, const char* string, DvarSetSource source)> Dvar_SetFromStringFromSource{ 0x140CECFF0 };

		WEAK symbol<void(const char* name, const char* string)> Dvar_SetCommand{ 0x140CECB30 };
		WEAK symbol<dvar_t* (const char* name)> Dvar_FindVar{ 0x140CEA460 };
		WEAK symbol<void(void* callback, void* userData)> Dvar_ForEach{ 0x140CEA480 };
		WEAK symbol<dvar_t* (unsigned int checksum)> Dvar_FindMalleableVar{ 0x140CEA3C0 };
		WEAK symbol<void(const dvar_t* dvar)> Dvar_ClearModified{ 0x140CE9E90 };
		WEAK symbol<const char* (const dvar_t* dvar)> Dvar_DisplayableLatchedValue{ 0x140CEA1D0 };
		WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{ 0x140BB1F30 };
		WEAK symbol<const char* (dvar_t* dvar, DvarValue value)> Dvar_ValueToString{ 0x140CEED00 };
		WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{ 0x140CEC490 };
		WEAK symbol<unsigned int(const char* name)> Dvar_GenerateChecksum{ 0x140CEA520 };
		WEAK symbol<void(dvar_t* dvar, int value)> Dvar_SetInt{ 0x140CED3D0 };
		WEAK symbol<void(bool cheatOverride)> Dvar_OverrideCheatProtection{ 0x140CEB250 };

		WEAK symbol<__int64(const char* qpath, char** buffer)> FS_ReadFile{ 0x140CDE200 };
		WEAK symbol<void(char* buffer)> FS_FreeFile{ 0x140CDE1F0 };
		WEAK symbol<void(int h, const char* fmt, ...)> FS_Printf{ 0x140CDD1C0 };

		WEAK symbol<char* (char* string)> I_CleanStr{ 0x140CFACC0 };

		WEAK symbol<const char* (int, int, int)> Key_KeynumToString{ 0x1409A95E0 };

		WEAK symbol<unsigned int(int controllerIndex)> Live_SyncOnlineDataFlags{ 0x140DC5CE0 };

		WEAK symbol<void()> R_SyncRenderThread{ 0x140E27EE0 };
		
		WEAK symbol<const char* ()> SEH_GetCurrentLanguageCode{ 0x140CBAF50 };

		WEAK symbol<int()> Sys_Milliseconds{ 0x140D58110 };

		WEAK symbol<HANDLE(Sys_Folder folder, const char* baseFilename)> Sys_CreateFile{ 0x140CFDF50 };

		WEAK symbol<bool()> Sys_IsDatabaseThread{ 0x140BB7B30 };

		WEAK symbol<void(int)> Scr_AddInt{ 0x140C0A580 };
		WEAK symbol<int()> Scr_GetInt{ 0x140C0B950 };

		WEAK symbol<void(const char* string)> SV_Cmd_TokenizeString{ 0x140B7DD00 };
		WEAK symbol<void()> SV_Cmd_EndTokenizedString{ 0x140B7DCC0 };
		WEAK symbol<bool()> SV_Loaded{ 0x140C114C0 };
		WEAK symbol<bool(const char* name)> SV_MapExists{ 0x140CDB620 };

		WEAK symbol<__int64(void* stream, int flush)> db_inflate{ 0x140E77380 };
		WEAK symbol<__int64(void* stream, const char* version, int stream_size)> db_inflateInit_{ 0x140E77980 };
		WEAK symbol<__int64(void* stream)> db_inflateEnd{ 0x140E777F0 };

		WEAK symbol<void(void* stream, void* memory, int size)> DB_Zip_InitThreadMemory{ 0x140E78290 };
		WEAK symbol<void(void* stream)> DB_Zip_ShutdownThreadMemory{ 0x140E782D0 };

		/***************************************************************
		 * Variables
		 **************************************************************/

		WEAK symbol<CmdArgs> sv_cmd_args{ 0x145D65C20 };
		WEAK symbol<CmdArgs> cmd_args{ 0x145D65B70 };
		WEAK symbol<cmd_function_s*> cmd_functions{ 0x145D65CC8 };
		WEAK symbol<const char*> command_whitelist{ 0x1414D1B70 };

		WEAK symbol<int> dvarCount{ 0x147595E54 };
		WEAK symbol<dvar_t*> dvarPool{ 0x147595E60 };

		WEAK symbol<DWORD> threadIds{ 0x14602BAB0 };

		WEAK symbol<DB_LoadData> g_load{ 0x1452A8010 };
		WEAK symbol<int> g_authLoad_isSecure{ 0x14529DD90 };
		WEAK symbol<DB_ReadStream> db_stream{ 0x1452A8050 };
		WEAK symbol<db_z_stream_s> db_zip_stream{ 0x14529DD30 };
		WEAK symbol<char*> db_zip_memory{ 0x14525B500 };

		WEAK symbol<unsigned __int64> g_streamPos{ 0x145687E30 };
	}
}
