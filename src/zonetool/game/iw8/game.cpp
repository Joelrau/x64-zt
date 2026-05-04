#include <std_include.hpp>
#include "game.hpp"

#include <utils/flags.hpp>

namespace iw8
{
	namespace game
	{
		uint64_t base_address;

		void load_base_address()
		{
			const auto module = GetModuleHandle(NULL);
			base_address = uint64_t(module);
		}

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

		// IW8
		void Cbuf_AddText(int clientNum, const char* cmd_text)
		{
			auto& buf = game::s_cmd_textArray[clientNum];
			constexpr int CRITSECT_CBUF = 35;

			const int len = (int)strlen(cmd_text);
			const bool needsNewline = (len == 0 || cmd_text[len - 1] != '\n');
			const int total = len + (needsNewline ? 1 : 0);

			game::Sys_EnterCriticalSection(CRITSECT_CBUF);

			if (buf.cmdsize + total <= buf.maxsize)
			{
				char* dst = (char*)buf.data + buf.cmdsize;
				memcpy(dst, cmd_text, len);

				if (needsNewline) dst[len] = '\n';

				buf.cmdsize += total;
			}
			else
			{
				printf("Cbuf_AddText: overflow (adding '%s')\n", cmd_text);
			}

			game::Sys_LeaveCriticalSection(CRITSECT_CBUF);
		}
	}
}

size_t operator"" _b(const size_t ptr)
{
	return iw8::game::base_address + ptr;
}

size_t reverse_b(const size_t ptr)
{
	return ptr - iw8::game::base_address;
}

size_t reverse_b(const void* ptr)
{
	return reverse_b(reinterpret_cast<size_t>(ptr));
}
