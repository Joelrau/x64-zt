#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace iw7
{
	namespace game
	{
		int Cmd_Argc()
		{
			return cmd_args->argc[cmd_args->nesting];
		}

		const char* Cmd_Argv(const int index)
		{
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

		const char* G_GAME_MODE_STRINGS[] =
		{
			"none",
			"sp",
			"mp",
			"cp",
		};

		const char* Com_GameMode_GetActiveGameModeStr()
		{
			return G_GAME_MODE_STRINGS[game::Com_GameMode_GetActiveGameMode()];
		}

		const char* Com_GameMode_GetGameModeStr(GameModeType gameMode)
		{
			return G_GAME_MODE_STRINGS[gameMode];
		}
	}
}