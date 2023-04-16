#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/s1/game.hpp"

#include "dvars.hpp"

#include <utils/hook.hpp>

#include "zonetool/s1/zonetool.hpp"

namespace zonetool::s1
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
		reinterpret_cast<void(*)()>(0x1405A54F0)();
	}

	void remove_renderer()
	{
		// Disable VirtualLobby
		::s1::dvars::override::register_bool("virtualLobbyEnabled", false, ::s1::game::DVAR_FLAG_NONE | ::s1::game::DVAR_FLAG_READ);

		// Disable r_preloadShaders
		::s1::dvars::override::register_bool("r_preloadShaders", false, ::s1::game::DVAR_FLAG_NONE | ::s1::game::DVAR_FLAG_READ);

		// Hook R_SyncGpu
		utils::hook::jump(0x1405A7630, sync_gpu_stub);

		utils::hook::jump(0x14020C6B0, init_no_renderer);

		utils::hook::nop(0x1404AE6AE, 5); // don't load config file
		utils::hook::nop(0x1403AF719, 5); // ^
		utils::hook::set<uint8_t>(0x1403D2490, 0xC3); // don't save config file
		utils::hook::set<uint8_t>(0x14022AFC0, 0xC3); // disable self-registration
		utils::hook::set<uint8_t>(0x1404DA780, 0xC3); // init sound system (1)
		utils::hook::set<uint8_t>(0x14062BC10, 0xC3); // init sound system (2)
		utils::hook::set<uint8_t>(0x1405F31A0, 0xC3); // render thread
		utils::hook::set<uint8_t>(0x140213C20, 0xC3); // called from Com_Frame, seems to do renderer stuff
		utils::hook::set<uint8_t>(0x1402085C0, 0xC3);
		// CL_CheckForResend, which tries to connect to the local server constantly
		utils::hook::set<uint8_t>(0x14059B854, 0); // r_loadForRenderer default to 0
		utils::hook::set<uint8_t>(0x1404D6952, 0xC3); // recommended settings check - TODO: Check hook
		utils::hook::set<uint8_t>(0x1404D9BA0, 0xC3); // some mixer-related function called on shutdown
		utils::hook::set<uint8_t>(0x1403B2860, 0xC3); // dont load ui gametype stuff
		utils::hook::nop(0x14043ABB8, 6); // unknown check in SV_ExecuteClientMessage
		utils::hook::nop(0x140439F15, 4); // allow first slot to be occupied
		utils::hook::nop(0x14020E01C, 2); // properly shut down dedicated servers
		utils::hook::nop(0x14020DFE9, 2); // ^
		utils::hook::nop(0x14020E047, 5); // don't shutdown renderer

		utils::hook::set<uint8_t>(0x140057D40, 0xC3); // something to do with blendShapeVertsView
		utils::hook::nop(0x14062EA17, 8); // sound thing

		utils::hook::set<uint8_t>(0x1404D6960, 0xC3); // cpu detection stuff?
		utils::hook::set<uint8_t>(0x1405AEC00, 0xC3); // gfx stuff during fastfile loading
		utils::hook::set<uint8_t>(0x1405AEB10, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405AEBA0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140275640, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405AEB60, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140572640, 0xC3); // directx stuff
		utils::hook::set<uint8_t>(0x1405A1340, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140021D60, 0xC3); // ^ - mutex
		utils::hook::set<uint8_t>(0x1405A17E0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1400534F0, 0xC3); // rendering stuff
		utils::hook::set<uint8_t>(0x1405A1AB0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405A1BB0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405A21F0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405A2D60, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405A3400, 0xC3); // ^

		// shaders
		utils::hook::set<uint8_t>(0x140057BC0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140057B40, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1405EE040, 0xC3); // ^ - mutex

		utils::hook::set<uint8_t>(0x1404DAF30, 0xC3); // idk
		utils::hook::set<uint8_t>(0x1405736B0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1405A6E70, 0xC3); // R_Shutdown
		utils::hook::set<uint8_t>(0x1405732D0, 0xC3); // shutdown stuff
		utils::hook::set<uint8_t>(0x1405A6F40, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405A61A0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x14062C550, 0xC3); // sound crashes

		utils::hook::set<uint8_t>(0x140445070, 0xC3); // disable host migration

		utils::hook::set<uint8_t>(0x1403E1A50, 0xC3); // render synchronization lock
		utils::hook::set<uint8_t>(0x1403E1990, 0xC3); // render synchronization unlock

		utils::hook::set<uint8_t>(0x1400E517B, 0xEB); // LUI: Unable to start the LUI system due to errors in main.lua

		//utils::hook::nop(0x1404CC482, 5); // Disable sound pak file loading
		//utils::hook::nop(0x1404CC471, 2); // ^
		//utils::hook::set<uint8_t>(0x140279B80, 0xC3); // Disable image pak file loading

		utils::hook::set<uint8_t>(0x14027A200, 0xC3); // disable load/read of alwaysloaded assets ( streamed images )
		utils::hook::set<uint8_t>(0x14027A590, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140279E70, 0xC3); // DB_EnterStreamingTabulate

		utils::hook::set<uint8_t>(0x14027A270, 0xC3); // DB_LoadPackedLoadedSounds
		utils::hook::set<uint8_t>(0x14027A7C0, 0xC3); // DB_ReadPackedLoadedSounds

		// Reduce min required memory
		utils::hook::set<uint64_t>(0x1404D140D, 0x80000000);
		utils::hook::set<uint64_t>(0x1404D14BF, 0x80000000);
	}

	void load_common_zones()
	{
		static std::vector<std::string> defaultzones =
		{
			"code_post_gfx_mp",
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

		zonetool::s1::start();

		while (1)
		{
			std::this_thread::sleep_for(5ms);
			utils::hook::invoke<void>(0x1404D6150); // Sys_CheckQuitRequest
			utils::hook::invoke<void>(0x140274F10); // DB_Update
			utils::hook::invoke<void>(0x1403AEE30, 0, 0); // Cbuf_Execute
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			remove_renderer();

			// stop the game after loading common zones
			utils::hook::call(0x1405A562A, load_common_zones_stub);

			// disable splash
			utils::hook::nop(0x1404D74E3, 5);

			zonetool::s1::initialize();
		}
	};
}

REGISTER_COMPONENT_S1(zonetool::s1::component)
