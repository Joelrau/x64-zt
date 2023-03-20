#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw6/game.hpp"

#include "zonetool/iw6/zonetool.hpp"

namespace zonetool::iw6
{
	void sync_gpu_stub()
	{
		std::this_thread::sleep_for(1ms);
	}

	void init_no_renderer()
	{
		static bool initialized = false;
		if (initialized) return;
		initialized = true;

		// R_LoadGraphicsAssets
		utils::hook::invoke<void>(0x1405E6F80);
	}

	void remove_renderer()
	{
		//printf("Renderer disabled...\n");

		// Hook R_SyncGpu
		utils::hook::jump(0x1405E8530, sync_gpu_stub);

		utils::hook::jump(0x1402C89A0, init_no_renderer);

		utils::hook::nop(0x1404DDC2E, 5); // don't load config file
		utils::hook::set<uint8_t>(0x140416100, 0xC3); // don't save config file
		utils::hook::set<uint8_t>(0x1402E5830, 0xC3); // disable self-registration
		utils::hook::set<uint8_t>(0x1402C7935, 5); // make CL_Frame do client packets, even for game state 9
		utils::hook::set<uint8_t>(0x140503FF0, 0xC3); // init sound system (1)
		utils::hook::set<uint8_t>(0x140602380, 0xC3); // start render thread
		utils::hook::set<uint8_t>(0x140658580, 0xC3); // init sound system (2)
		utils::hook::set<uint8_t>(0x1402CF570, 0xC3); // called from Com_Frame, seems to do renderer stuff
		utils::hook::set<uint8_t>(0x1402C49B0, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
		utils::hook::set<uint8_t>(0x1405DAE1F, 0); // r_loadForRenderer default to 0
		utils::hook::set<uint8_t>(0x1404FFCE2, 0xC3); // recommended settings check - TODO: Check hook
		utils::hook::set<uint8_t>(0x140503420, 0xC3); // some mixer-related function called on shutdown
		utils::hook::set<uint8_t>(0x1404BEC10, 0xC3); // dont load ui gametype stuff
		utils::hook::nop(0x14047261C, 6); // unknown check in SV_ExecuteClientMessage
		utils::hook::nop(0x140471B6B, 4); // allow first slot to be occupied
		utils::hook::nop(0x1402CA0F5, 2); // properly shut down dedicated servers
		utils::hook::nop(0x1402CA0B9, 2); // ^
		utils::hook::nop(0x1402CA12D, 5); // don't shutdown renderer
		utils::hook::set<uint8_t>(0x1405E87DE, 0xEB); // ignore world being in use

		utils::hook::set<uint8_t>(0x1404FFCF0, 0xC3); // cpu detection stuff
		utils::hook::set<uint8_t>(0x1405F0620, 0xC3); // gfx stuff during fastfile loading
		utils::hook::set<uint8_t>(0x1405F0530, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405F05C0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140324F00, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405F0580, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405B81A0, 0xC3); // directx stuff
		utils::hook::set<uint8_t>(0x1405E0CF0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405E1530, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405E3E50, 0xC3); // ^ - mutex
		utils::hook::set<uint8_t>(0x1405E1050, 0xC3); // ^

		// shaders
		utils::hook::set<uint8_t>(0x140167E00, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140167D80, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1406492A0, 0xC3); // ^ - mutex

		utils::hook::set<uint8_t>(0x1405047A0, 0xC3); // idk
		utils::hook::set<uint8_t>(0x1405B8DB0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1405E7D20, 0xC3); // R_Shutdown
		utils::hook::set<uint8_t>(0x1405B8BD0, 0xC3); // shutdown stuff
		utils::hook::set<uint8_t>(0x1405E7DF0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405E76C0, 0xC3); // ^

		// UI
		utils::hook::set<uint8_t>(0x1402CFE10, 0xC3); // CL_InitUI
		utils::hook::set<uint8_t>(0x1402CFE60, 0xC3); // CL_ShutdownUI

		utils::hook::set<uint8_t>(0x14065EA00, 0xC3); // sound crashes

		utils::hook::set<uint8_t>(0x14047BE70, 0xC3); // disable host migration

		utils::hook::set<uint8_t>(0x140423B20, 0xC3); // render synchronization lock
		utils::hook::set<uint8_t>(0x140423A60, 0xC3); // render synchronization unlock

		utils::hook::jump(0x140610EB6, 0x140610F15); // ^
	}

	void load_common_zones()
	{
		static std::vector<std::string> defaultzones =
		{
			//"code_post_gfx_mp",
			//"ui_mp",
			"common_mp",
		};

		XZoneInfo zones[8]{ 0 };

		// Load our custom zones
		for (std::size_t i = 0; i < defaultzones.size(); i++)
		{
			zones[i].name = defaultzones[i].data();
			zones[i].allocFlags = DB_ZONE_COMMON;
			zones[i].freeFlags = 0;
		}

		return DB_LoadXAssets(zones, static_cast<unsigned int>(defaultzones.size()), DB_LOAD_ASYNC);
	}

	void load_common_zones_stub()
	{
		load_common_zones();

		zonetool::iw6::start();

		while (1)
		{
			std::this_thread::sleep_for(5ms);
			utils::hook::invoke<void>(0x1404FF3E0); // Sys_CheckQuitRequest
			utils::hook::invoke<void>(0x140324430); // DB_Update
			utils::hook::invoke<void>(0x1403F6C10, 0, 0); // Cbuf_Execute
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			remove_renderer();

			// stop the game after loading common zones
			utils::hook::call(0x1405E7113, load_common_zones_stub);

			// disable splash
			utils::hook::set<uint8_t>(0x140501D60, 0xC3);

			// disable demonware
			utils::hook::set<uint8_t>(0x140558E20, 0xC3); // dwNetStart

			zonetool::iw6::initialize();
		}
	};
}

REGISTER_COMPONENT_IW6(zonetool::iw6::component)
