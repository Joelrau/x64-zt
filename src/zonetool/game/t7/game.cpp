#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace t7
{
	namespace game
	{
		game::CmdArgs* Cmd_Args()
		{
			return game::Sys_GetTLS()->cmdArgs;
		}

		int Cmd_Argc()
		{
			const auto cmd_args = Cmd_Args();
			return cmd_args->argc[cmd_args->nesting];
		}

		const char* Cmd_Argv(const int index)
		{
			const auto cmd_args = Cmd_Args();
			return cmd_args->argv[cmd_args->nesting][index];
		}

		int SV_Cmd_Argc()
		{
			return sv_cmd_args->argc[sv_cmd_args->nesting];
		}

		const char* SV_Cmd_Argv(const int index)
		{
			return sv_cmd_args->argv[sv_cmd_args->nesting][index];
		}
	}
}