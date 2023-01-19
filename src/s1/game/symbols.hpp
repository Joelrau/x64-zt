#pragma once

#define WEAK __declspec(selectany)

namespace game
{
	/***************************************************************
	 * Functions
	 **************************************************************/

	WEAK symbol<void(errorParm code, const char* message, ...)> Com_Error{0x1403CE480};
	WEAK symbol<void()> Com_Frame_Try_Block_Function{0x1403CEF30};
	WEAK symbol<CodPlayMode()> Com_GetCurrentCoDPlayMode{0x1404C9690};
	WEAK symbol<void()> Com_Quit_f{0x1403D08C0};

	WEAK symbol<void(const char* cmdName, void (), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{0x1403AF2C0};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{0x1403AF900};
	WEAK symbol<void(const char*)> Cmd_RemoveCommand{0x1403AFEF0};
	WEAK symbol<void(const char* text_in)> Cmd_TokenizeString{0x1403B0020};
	WEAK symbol<void()> Cmd_EndTokenizeString{0x1403AF5B0};

	WEAK symbol<void(const char* message)> Conbuf_AppendText{0x1404D9040};

	WEAK symbol<char*(int start)> ConcatArgs{0x1402E9670};

	WEAK symbol<void(int localClientNum, void (*)(int localClientNum))> Cbuf_AddCall{0x1403AECF0};
	WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{0x1403AED70};
	WEAK symbol<void(int localClientNum, int controllerIndex, const char* buffer, void (int, int, const char*))> Cbuf_ExecuteBufferInternal{0x1403AEE80};

	WEAK symbol<dvar_t*(const char* name)> Dvar_FindVar{0x1404BF8B0};
	WEAK symbol<void(const dvar_t* dvar)> Dvar_ClearModified{0x1404BF690};
	WEAK symbol<void(char* buffer, int index)> Dvar_GetCombinedString{0x1403D3290};
	WEAK symbol<bool(const char* name)> Dvar_IsValidName{0x1404BFF70};
	WEAK symbol<void(dvar_t* dvar, DvarSetSource source)> Dvar_Reset{0x1404C1DB0};
	WEAK symbol<void(const char* dvar, const char* buffer)> Dvar_SetCommand{0x1404C2520};
	WEAK symbol<void(const dvar_t* dvar, const char* string)> Dvar_SetString{0x1404C3610};
	WEAK symbol<void(const dvar_t* dvar, bool value)> Dvar_SetBool{0x1404C1F30};
	WEAK symbol<void(const char*, const char*, DvarSetSource)> Dvar_SetFromStringByNameFromSource{0x1404C2E40};
	WEAK symbol<const char*(dvar_t* dvar, dvar_value value)> Dvar_ValueToString{0x1404C47B0};

	WEAK symbol<dvar_t*(const char* dvarName, bool value, unsigned int flags, const char* description)>
	Dvar_RegisterBool{0x1404C0BE0};
	WEAK symbol<dvar_t*(const char* dvarName, const char** valueList, int defaultIndex, unsigned int flags,
	                    const char* description)> Dvar_RegisterEnum{0x1404C0EC0};
	WEAK symbol<dvar_t*(const char* dvarName, float value, float min, float max, unsigned int flags,
	                    const char* description)> Dvar_RegisterFloat{0x1404C0FB0};
	WEAK symbol<dvar_t*(const char* dvarName, int value, int min, int max, unsigned int flags, const char* desc)>
	Dvar_RegisterInt{0x1404C1080};
	WEAK symbol<dvar_t*(const char* dvarName, const char* value, unsigned int flags, const char* description)>
	Dvar_RegisterString{0x1404C1450};
	WEAK symbol<dvar_t* (const char* dvarName, float x, float y, float min, float max,
		                 unsigned int flags, const char* description)> Dvar_RegisterVec2{0x1404C1520};
	WEAK symbol<dvar_t* (const char* dvarName, float x, float y, float z, float min, float max,
		                 unsigned int flags, const char* description)> Dvar_RegisterVec3{0x1404C1600};
	WEAK symbol<dvar_t*(const char* dvarName, float x, float y, float z, float w, float min, float max,
	                    unsigned int flags, const char* description)> Dvar_RegisterVec4{0x1404C1800};

	WEAK symbol<DWOnlineStatus()> dwGetLogOnStatus{0x14053CCB0};

	WEAK symbol<long long(const char* qpath, char** buffer)> FS_ReadFile{0x1404AF380};
	WEAK symbol<void(void* buffer)> FS_FreeFile{0x1404AF370};
	WEAK symbol<void (const char *gameName)> FS_Startup{0x1404AE930};
	WEAK symbol<void (const char *path, const char *dir, int bLanguageDirectory, int iLanguage)> FS_AddGameDirectory{0x1404ACF80};
	WEAK symbol<void (const char *path, const char *dir)> FS_AddLocalizedGameDirectory{0x1404AD170};

	WEAK symbol<char*(char* string)> I_CleanStr{0x1404C99A0};

	WEAK symbol<const char*(int, int, int)> Key_KeynumToString{0x140207C50};

	WEAK symbol<unsigned int(int)> Live_SyncOnlineDataFlags{0x140562830};

	WEAK symbol<const char*(scr_string_t stringValue)> SL_ConvertToString{0x1403F0F10};
	WEAK symbol<scr_string_t(const char* str)> SL_FindString{0x1403F11C0};
	WEAK symbol<scr_string_t(const char* str, unsigned int user)> SL_GetString{0x1403F1440};
	WEAK symbol<unsigned int(char const* str)> SL_GetCanonicalString{0x1403EDCA0};

	WEAK symbol<void(char* path, int pathSize, Sys_Folder folder, const char* filename, const char* ext)> Sys_BuildAbsPath{0x1404CC7E0};
	WEAK symbol<HANDLE(int folder, const char* baseFileName)> Sys_CreateFile{0x1404CC8A0};
	WEAK symbol<void(const char* error, ...)> Sys_Error{0x1404D6260};
	WEAK symbol<bool(const char* path)> Sys_FileExists{0x1404D6310};
	WEAK symbol<bool()> Sys_IsDatabaseReady2{0x1403E1840};
	WEAK symbol<int()> Sys_Milliseconds{0x1404D8730};
	WEAK symbol<bool(int, void const*, const netadr_s*)> Sys_SendPacket{0x1404D8460};
	WEAK symbol<void(Sys_Folder, const char* path)> Sys_SetFolder{0x1404CCA10};
	WEAK symbol<void()> Sys_ShowConsole{0x1404D98B0};
	WEAK symbol<bool()> Sys_IsMainThread{0x1403E1880};

	WEAK symbol<const char*()> SEH_GetCurrentLanguageName{0x1404745C0};

	WEAK symbol<void*(unsigned int size, unsigned int alignment, unsigned int type, int source)> PMem_AllocFromSource_NoDebug{0x1404C7BA0};
	WEAK symbol<void*(unsigned int size)> Hunk_AllocateTempMemoryHighInternal{0x1404B68B0};

	WEAK symbol<void*(jmp_buf* Buf, int Value)> longjmp{0x1406FD930};
	WEAK symbol<int(jmp_buf* Buf)> _setjmp{0x1406FE070};

	/***************************************************************
	 * Variables
	 **************************************************************/

	WEAK symbol<CmdArgs> cmd_args{0x1479ECB00};
	WEAK symbol<cmd_function_s*> cmd_functions{0x1479ECC58};

	WEAK symbol<int> dvarCount{0x14B32AA30};
	WEAK symbol<dvar_t*> sortedDvars{0x14B32AA50};
}
