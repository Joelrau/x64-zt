#pragma once

#define WEAK __declspec(selectany)

namespace iw6
{
	namespace game
	{
		/***************************************************************
		* Functions
		**************************************************************/

		WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x1403F6B50 };
		WEAK symbol<void()> Com_Frame_Try_Block_Function{ 0x1404131A0 };
		WEAK symbol<void(const char* message)> Conbuf_AppendText{ 0x1405028C0 };
		WEAK symbol<void(const char* cmdName, void(), cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{ 0x1403F7070 };
		WEAK symbol<void(int localClientNum, int controllerIndex, const char* text)> Cmd_ExecuteSingleCommand{ 0x1403F7680 };

		WEAK symbol<void()> G_Glass_Update{ 0x140397450 };

		WEAK symbol<void()> Sys_ShowConsole{ 0x140503130 };
		WEAK symbol<bool()> Sys_IsDatabaseReady2{ 0x140423920 };

		WEAK symbol<const char* ()> SEH_GetCurrentLanguageCode{ 0x1404A56E0 };
		WEAK symbol<const char* ()> SEH_GetCurrentLanguageName{ 0x1404A5710 };

		/***************************************************************
		* Variables
		**************************************************************/

		WEAK symbol<CmdArgs> cmd_args{ 0x144518480 };
		WEAK symbol<CmdArgs> sv_cmd_args{ 0x144518530 };
		WEAK symbol<cmd_function_s*> cmd_functions{ 0x1445185D8 };
	}
}
