#pragma once

#define WEAK __declspec(selectany)

namespace t7
{
	namespace game
	{
		/***************************************************************
		 * Functions
		 **************************************************************/

		WEAK symbol<void(int localClientNum, const char* text)> Cbuf_AddText{ 0x1404F75B0 };
		WEAK symbol<void(int localClientNum, ControllerIndex_t controllerIndex, const char* text, bool fromRemoteConsole)> Cmd_ExecuteSingleCommand{ 0x1404F8890 };
		WEAK symbol<void(const char* cmdName, xcommand_t function, cmd_function_s* allocedCmd)> Cmd_AddCommandInternal{ 0x1404F8210 };

		WEAK symbol<int()> Sys_Milliseconds{ 0x1405972F0 };
		WEAK symbol<void()> Sys_ShowConsole{ 0x140597E40 };
		WEAK symbol<TLSData* ()> Sys_GetTLS{ 0x140525EB0 };
		WEAK symbol<TLSData* ()> Sys_IsDatabaseReady{ 0x142183A60 };

		/***************************************************************
		 * Variables
		 **************************************************************/

		WEAK symbol<cmd_function_s> cmd_functions{ 0x14946F860 };
		WEAK symbol<CmdArgs> sv_cmd_args{ 0x14944C740 };
	}
}
