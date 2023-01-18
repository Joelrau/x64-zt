#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x1404033B0 };

	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer,
		void(int, int, const char*))> Cbuf_ExecuteBufferInternal{ 0x1404034C0 };
	WEAK symbol<void(const char* message)> Conbuf_AppendText{ 0x140513FF0 };
	WEAK symbol<char* (int start)> ConcatArgs{ 0x140335D70 };
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{ 0x140403F60 };
	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{ 0x140403950 };
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{ 0x1404045D0 };
	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{ 0x1404046F0 };
	WEAK symbol<void()> Cmd_EndTokenizeString{ 0x140403C20 };

	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{ 0x1400D78A0 };
	WEAK symbol<void()> Com_Quit_f{ 0x1400DA830 };

	WEAK symbol<void()> Quit{ 0x1400DA830 };

	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{ 0x1404FD0A0 };
	WEAK symbol<dvar_t* (const char* name)> Dvar_FindVar{ 0x1404FBB00 };
	WEAK symbol<void(const dvar_t* dvar)> Dvar_ClearModified{ 0x1404FB930 };
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{ 0x14041D830 };
	WEAK symbol<const char* (dvar_t* dvar, dvar_value value)> Dvar_ValueToString{ 0x1404FE660 };
	WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{ 0x1404FCC40 };
	WEAK symbol<void(const char*, const char*,
		DvarSetSource)> Dvar_SetFromStringByNameFromSource{ 0x1404FD490 };

	WEAK symbol<dvar_t* (int hash, const char* name, bool value,
		unsigned int flags)> Dvar_RegisterBool{ 0x1404FA540 };
	WEAK symbol<dvar_t* (int hash, const char* name, int value, int min, int max,
		unsigned int flags)> Dvar_RegisterInt{ 0x1404FAA20 };
	WEAK symbol<dvar_t* (int hash, const char* dvarName, float value, float min,
		float max, unsigned int flags)> Dvar_RegisterFloat{ 0x1404FA910 };
	WEAK symbol<dvar_t* (int hash, const char* dvarName, const char* value,
		unsigned int flags)> Dvar_RegisterString{ 0x1404FAB00 };
	WEAK symbol<dvar_t* (int dvarName, const char* a2, float x, float y, float z,
		float w, float min, float max, unsigned int flags)> Dvar_RegisterVec4{ 0x1404FAF40 };

	WEAK symbol<int(const char* fname)> generateHashValue{ 0x1401B1010 };

	WEAK symbol<char* (char* string)> I_CleanStr{ 0x140503D00 };

	WEAK symbol<const char* (int, int, int)> Key_KeynumToString{ 0x14024FE10 };

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{ 0x14059A700 };

	WEAK symbol<void(int arg, char* buffer, int bufferLength)> SV_Cmd_ArgvBuffer{ 0x140404CA0 };
	WEAK symbol<void(const char* text_in)> SV_Cmd_TokenizeString{ 0x140404D20 };
	WEAK symbol<void()> SV_Cmd_EndTokenizedString{ 0x140404CE0 };

	WEAK symbol<void(const char* error, ...)> Sys_Error{ 0x140511520 };
	WEAK symbol<int()> Sys_Milliseconds{ 0x140513710 };
	WEAK symbol<bool()> Sys_IsDatabaseReady2{ 0x14042B090 };

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> sv_cmd_args{ 0x14946BA20 };

	WEAK symbol<const char*> command_whitelist{ 0x14120C360 };
	WEAK symbol<cmd_function_s*> cmd_functions{ 0x14946BAC8 };
	WEAK symbol<CmdArgs> cmd_args{ 0x14946B970 };

	WEAK symbol<int> dvarCount{ 0x14D064CF4 };
	WEAK symbol<dvar_t> dvarPool{ 0x14D064D00 };

	WEAK symbol<DWORD> threadIds{ 0x149810E00 };
}
