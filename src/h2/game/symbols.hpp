#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	// Functions

	WEAK symbol<void(unsigned int weapon, bool isAlternate, 
		char* output, unsigned int maxStringLen)> BG_GetWeaponNameComplete{0x1406A0800};

	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x14059A050};

	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessage{0x14037F450};
	WEAK symbol<void(int localClientNum, const char* message)> CG_GameMessageBold{0x14037F1B0};
	WEAK symbol<char*(const unsigned int weapon, 
		bool isAlternate, char* outputBuffer, int bufferLen)> CG_GetWeaponDisplayName{0x1403B9210};

	WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x14059A5F0};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x14059ABA0};

	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x1405A2D80};
	WEAK symbol<void(char const* finalMessage)> Com_Shutdown{0x1405A62C0};
	WEAK symbol<void()> Com_Quit_f{0x1405A50D0};
	WEAK symbol<bool()> Com_InFrontend{0x140328BD0};
	WEAK symbol<void()> Quit{0x1405A52A0};

	WEAK symbol<DB_FileSysInterface*()> DB_FSInitialize{0x140272EC0};
	
	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x140618F90};
	WEAK symbol<dvar_t*(int hash)> Dvar_FindMalleableVar{0x140618F00};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x1405A75D0};
	WEAK symbol<dvar_t*(int hash, const char* name, bool value, unsigned int flags)> Dvar_RegisterBool{0x140617BB0};
	WEAK symbol<dvar_t*(int hash, const char* name, const char** valueList, int defaultIndex, unsigned int flags)> Dvar_RegisterEnum{0x140617E90};
	WEAK symbol<dvar_t*(int hash, const char* name, int value, int min, int max, unsigned int flags)> Dvar_RegisterInt{0x140618090};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, float value, float min, float max, unsigned int flags)>
		Dvar_RegisterFloat{0x140617F80};
	WEAK symbol<dvar_t*(int hash, const char* dvarName, const char* value, unsigned int flags)>
		Dvar_RegisterString{0x140618170};
	WEAK symbol<dvar_t*(int dvarName, const char* a2, float x, float y, float z, float w, float min, float max,
		unsigned int flags)> Dvar_RegisterVec4{0x1406185F0};
	WEAK symbol<const char*(const dvar_t* dvar)> Dvar_DisplayableValue{0x140618EA0};
	WEAK symbol<const char*(dvar_t* dvar, void* a2, void* value)> Dvar_ValueToString{0x14061B8F0};
	WEAK symbol<void(int hash, const char* name, const char* buffer)> Dvar_SetCommand{0x14061A5C0};
	WEAK symbol<void(const char* dvarName, const char* string, DvarSetSource source)> Dvar_SetFromStringFromSource{0x14061A910};
	WEAK symbol<void(const dvar_t* dvar, const char* value)> Dvar_SetString{0x14061ABF0};
	WEAK symbol<void(const dvar_t* dvar, DvarSetSource source)> Dvar_Reset{0x140619FE0};

	WEAK symbol<int(const char* fname)> generateHashValue{0x140343D20};

	WEAK symbol<unsigned int(const char* name)> G_GetWeaponForName{0x14051B260};
	WEAK symbol<int(void* ps, unsigned int weapon, int a3, int a4, __int64 a5, int a6)> 
		G_GivePlayerWeapon{0x14051B660};
	WEAK symbol<void(void* ps, const unsigned int weapon, int hadWeapon)> G_InitializeAmmo{0x1404C4110};
	WEAK symbol<void(int clientNum, const unsigned int weapon)> G_SelectWeapon{0x14051C0D0};

	WEAK symbol<char*(char* string)> I_CleanStr{0x140620660};

	WEAK symbol<const char*(int, int, int)> Key_KeynumToString{0x1403D32D0};

	WEAK symbol<const char*(unsigned int stringValue)> SL_ConvertToString{0x1405BFBB0};
	WEAK symbol<unsigned int(const char* str, unsigned int user)> SL_GetString{0x1405C0170};
	WEAK symbol<unsigned int(const char* str)> SL_GetCanonicalString{0x1405BC970};

	WEAK symbol<bool()> SV_Loaded{0x1406B3860};

	WEAK symbol<void()> Sys_ShowConsole{0x140633080};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x1405A9FE0};
	WEAK symbol<int()> Sys_Milliseconds{0x140650720};
	WEAK symbol<bool()> Sys_IsMainThread{0x1405AA020};
	WEAK symbol<void(int critSec)> Sys_EnterCriticalSection{0x140624240};
	WEAK symbol<void(int critSec)> Sys_LeaveCriticalSection{0x1406242C0};

	WEAK symbol<const char*()> SEH_GetCurrentLanguageCode{0x1405E5180};
	WEAK symbol<const char*()> SEH_GetCurrentLanguageName{0x1405E5200};

	WEAK symbol<const char*(const char* string)> UI_SafeTranslateString{0x1405A2930};
	WEAK symbol<int(int localClientNum, const char* sound)> UI_PlayLocalSoundAlias{0x140606080};

	WEAK symbol<void*(jmp_buf* Buf, int Value)> longjmp{0x14089EED0};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x1408EC2E0};

	// Variables

	WEAK symbol<cmd_function_s*> cmd_functions{0x14AD17BB8};
	WEAK symbol<CmdArgs> cmd_args{0x14AD17A60};
	WEAK symbol<const char*> command_whitelist{0x140BF84E0};

	WEAK symbol<HWND> hWnd{0x14CCF81C0};

	WEAK symbol<const char*> g_assetNames{0x140BEF280};

	WEAK symbol<int> g_compressor{0x142065E80};
	WEAK symbol<int> g_poolSize{0x140BF2E40};

	WEAK symbol<DWORD> threadIds{0x14B11DC80};

	WEAK symbol<int> keyCatchers{0x14203F3C0};

	WEAK symbol<PlayerKeyState> playerKeys{0x141E8767C};

	WEAK symbol<int> dvarCount{0x14BFBB310};
	WEAK symbol<dvar_t> dvarPool{0x14BFBB320};

	WEAK symbol<unsigned int> levelEntityId{0x14B5E0B30};
	WEAK symbol<int> g_script_error_level{0x14BA9CC24};
	WEAK symbol<jmp_buf> g_script_error{0x14BA9CD40};
}
