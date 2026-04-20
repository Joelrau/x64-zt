#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw8/game.hpp"

#include <utils/thread.hpp>

namespace iw8
{
	namespace console
	{
		static volatile bool ingame = false;
		static volatile bool exit = false;

		DWORD WINAPI console(LPVOID)
		{
			AllocConsole();
			freopen("CONIN$", "r", stdin);
			freopen("CONOUT$", "w", stdout);
			freopen("CONOUT$", "w", stderr);

			ShowWindow(GetConsoleWindow(), SW_SHOW);
			SetConsoleTitleA("IW8-ZoneTool");

			std::string cmd;
			exit = false;

			while (!exit)
			{
				std::getline(std::cin, cmd);
				if (ingame && !cmd.empty())
				{
					cmd.append("\n");
					game::Cbuf_AddText(cmd.data());
				}
			}

			return 0;
		}

		class component final : public component_interface
		{
		public:
			void post_start() override
			{
				const auto handle = CreateThread(0, 0, console, 0, 0, 0);
				utils::thread::set_name(handle, "Console");
			}

			void post_unpack() override
			{
				ingame = true;
			}

			void pre_destroy() override
			{
				ingame = false;
				exit = true;
			}
		};
	}
}

REGISTER_COMPONENT_IW8(iw8::console::component)
