#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/game.hpp"

#include "dvars.hpp"

#include "zonetool/zonetool.hpp"

namespace zonetool::h2
{
	namespace
	{
		void* sub_46148()
		{
			static uint64_t off_11C52460 = 0x140AD0C58;
			return &off_11C52460;
		}

		DECLSPEC_NORETURN void quit_stub()
		{
			component_loader::pre_destroy();
			exit(0);
		}

		void* pmem_alloc_stub(unsigned __int64 size, unsigned __int64 alignment, unsigned int type, int source)
		{
			return _aligned_malloc(size, alignment);
		}
	}

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
		utils::hook::invoke<void>(0x14074E150);
	}

	void remove_renderer()
	{
		// Hook R_SyncGpu
		utils::hook::jump(0x140750580, sync_gpu_stub);

		utils::hook::jump(0x1403D5AA0, init_no_renderer);

		utils::hook::set<uint8_t>(0x1405A6880, 0xC3); // don't save config file
		utils::hook::set<uint8_t>(0x1407C5550, 0xC3); // init sound system (2)
		utils::hook::set<uint8_t>(0x14079FE00, 0xC3); // render thread
		utils::hook::set<uint8_t>(0x1403D8E00, 0xC3); // called from Com_Frame, seems to do renderer stuff

		utils::hook::set<uint8_t>(0x1407425F9, 0x00); // r_loadForRenderer default to 0

		utils::hook::set<uint8_t>(0x1402A0780, 0xC3); // something to do with blendShapeVertsView
		utils::hook::set<uint8_t>(0x1402A0580, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1407587E0, 0xC3); // gfx stuff during fastfile loading
		utils::hook::set<uint8_t>(0x1407588B0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140758910, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1404181A0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140758830, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140717C80, 0xC3); // directx stuff
		utils::hook::set<uint8_t>(0x140749300, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1407A46E0, 0xC3); // ^ - mutex
		utils::hook::set<uint8_t>(0x140749880, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1402A6BA0, 0xC3); // rendering stuff
		utils::hook::set<uint8_t>(0x140749B00, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140749C10, 0xC3); // ^
		utils::hook::set<uint8_t>(0x14074A390, 0xC3); // ^
		utils::hook::set<uint8_t>(0x14074AEB0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x14074B550, 0xC3); // ^ 

		// shaders
		utils::hook::set<uint8_t>(0x1402A0580, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1402A04D0, 0xC3); // ^
		/*utils::hook::set<uint8_t>(0, 0xC3); // ^ - mutex

		utils::hook::set<uint8_t>(0, 0xC3); // idk
		utils::hook::set<uint8_t>(0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1405E0B30, 0xC3); // R_Shutdown
		utils::hook::set<uint8_t>(0x1405AE400, 0xC3); // shutdown stuff
		utils::hook::set<uint8_t>(0x1405E0C00, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DFE50, 0xC3); // ^

		// utils::hook::set<uint8_t>(0x1404B67E0, 0xC3); // sound crashes (H1 - questionable, function looks way different)

		utils::hook::set<uint8_t>(0x14042B2E0, 0xC3); // render synchronization lock
		utils::hook::set<uint8_t>(0x14042B210, 0xC3); // render synchronization unlock		*/

		utils::hook::set<uint8_t>(0x140328ECD, 0xEB); // LUI: Unable to start the LUI system due to errors in main.lua

		utils::hook::set<uint8_t>(0x14041D070, 0xC3); // disable load/read of alwaysloaded assets ( streamed images )
		utils::hook::set<uint8_t>(0x14041D220, 0xC3); // ^

		utils::hook::set<uint8_t>(0x14041D460, 0xC3); // DB_ReadPackedLoadedSounds
		/*
		utils::hook::set(0x1402BF7F0, 0xC3); // some loop
		utils::hook::set(0x14007E150, 0xC3); // related to shader caching / techsets / fastfiles

		// Reduce min required memory
		utils::hook::set<uint64_t>(0x14050C717, 0x80000000);*/
	}

	void load_common_zones()
	{
		static std::vector<std::string> defaultzones =
		{
			"code_post_gfx",
			//"ui_mp",
			//"common",
		};

		XZoneInfo zones[8]{0};

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

		zonetool::h2::start();

		while (1)
		{
			std::this_thread::sleep_for(5ms);
			utils::hook::invoke<void>(0x14064E6C0); // Sys_CheckQuitRequest
			utils::hook::invoke<void>(0x140417FA0); // DB_Update
			utils::hook::invoke<void>(0x14059A110, 0, 0); // Cbuf_Execute
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			// Fix startup crashes
			utils::hook::set(0x140633080, 0xC301B0);
			utils::hook::set(0x140272F70, 0xC301B0);
			utils::hook::jump(0x140046148, sub_46148, true);

			utils::hook::jump(0x14064EF10, quit_stub, true);

			// Disable battle net popup
			utils::hook::nop(0x1405F4496, 5);

			remove_renderer();

			// stop the game after loading common zones
			utils::hook::call(0x14074E22A, load_common_zones_stub);

			// disable splash
			utils::hook::set<uint8_t>(0x140650780, 0xC3);

			// stuck in a loop
			utils::hook::set<uint8_t>(0x14041CC70, 0xC3); // DB_EnterStreamingTabulate

			zonetool::h2::initialize();

			utils::hook::jump(0x14061E680, pmem_alloc_stub);
		}
	};
}

REGISTER_COMPONENT(zonetool::h2::component)
