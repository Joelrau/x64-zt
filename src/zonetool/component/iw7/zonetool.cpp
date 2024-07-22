#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw7/game.hpp"

#include "dvars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

#include "zonetool/iw7/zonetool.hpp"

namespace iw7
{
	namespace zonetool
	{
		void sync_gpu_stub()
		{
			std::this_thread::sleep_for(1ms);
		}

		void init_no_renderer()
		{
			// R_RegisterDvars
			utils::hook::invoke<void>(0x140DF62C0);

			// R_RegisterCmds
			utils::hook::invoke<void>(0x140DD7E50);

			// RB_Tonemap_RegisterDvars
			utils::hook::invoke<void>(0x1404B1320);

			static bool initialized = false;
			if (initialized) return;
			initialized = true;

			// R_LoadGraphicsAssets
			utils::hook::invoke<void>(0x140E06220);
		}

		void remove_renderer()
		{
			// Disable frontend
			//dvars::override::register_bool("frontEndSceneEnabled", false, game::DVAR_FLAG_READ);
			utils::hook::set<uint8_t>(0x1405AC8F0, 0xC3); // Com_FastFile_Frame_FrontEnd

			// Disable shader preload
			dvars::override::register_bool("r_preloadShaders", false, game::DVAR_FLAG_READ);

			// Disable load for renderer
			dvars::override::register_bool("r_loadForRenderer", false, game::DVAR_FLAG_READ);

			//dvars::override::register_bool("intro", false, game::DVAR_FLAG_READ);

			// Hook R_SyncGpu
			utils::hook::call(0x1403428B1, sync_gpu_stub);

			utils::hook::jump(0x140341B60, init_no_renderer, true);

			utils::hook::nop(0x140CDD5D3, 5); // don't load config file
			utils::hook::nop(0x140B7CE46, 5); // ^
			utils::hook::set<uint8_t>(0x140BB0930, 0xC3); // don't save config file

			utils::hook::set<uint8_t>(0x1409D49C0, 0xC3); // disable self-registration
			utils::hook::set<uint8_t>(0x140E574E0, 0xC3); // render thread
			utils::hook::set<uint8_t>(0x1403471A0, 0xC3); // called from Com_Frame, seems to do renderer stuff
			utils::hook::set<uint8_t>(0x1409AA9A0, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
			utils::hook::set<uint8_t>(0x140D2EBB0, 0xC3); // recommended settings check

			utils::hook::nop(0x140C5007B, 6); // unknown check in SV_ExecuteClientMessage
			utils::hook::nop(0x140C4F407, 3); // allow first slot to be occupied
			utils::hook::nop(0x1403429A7, 2); // properly shut down dedicated servers
			utils::hook::nop(0x14034296F, 2); // ^
			utils::hook::set<uint8_t>(0x140E08360, 0xC3); // don't shutdown renderer

			// SOUND patches
			utils::hook::nop(0x140C93213, 5); // snd stream thread
			utils::hook::set<uint8_t>(0x140C93206, 0); // snd_active
			utils::hook::set<uint8_t>(0x140CB9150, 0xC3); // sound queue thing
			utils::hook::set<uint8_t>(0x140C75550, 0xC3); // SD_AllocInit
			utils::hook::set<uint8_t>(0x140C75CA0, 0xC3); // SD_Init

			utils::hook::set<uint8_t>(0x140CA30C0, 0xC3); // SNDL_AddBank
			utils::hook::set<uint8_t>(0x140CB9090, 0xC3); // SND_UpdateWait
			utils::hook::set<uint8_t>(0x1403B7D70, 0xC3); // something sound related

			utils::hook::set(0x140CFDC40, 0xC3C033); // sound stream reading

			utils::hook::set<uint8_t>(0x140D597C0, 0xC3); // Voice_Init

			// IMAGE patches
			// image stream (pak)
			utils::hook::set<uint8_t>(0x140A81D40, 0xC3); // AlwaysLoadedAssets (image file data)
			utils::hook::nop(0x140A7DC7C, 2); // Skip opening pakfiles

			// UI patches
			utils::hook::set<uint8_t>(0x140615090, 0xC3); // LUI_CoD_Init

			// IW7 patches
			utils::hook::set(0x140E06060, 0xC3C033); // directx
			utils::hook::set(0x140E05B80, 0xC3C033); // ^
			utils::hook::set(0x140DD2760, 0xC3C033); // ^
			utils::hook::set(0x140E05E20, 0xC3C033); // ^ buffer
			utils::hook::set(0x140E11270, 0xC3C033); // ^
			utils::hook::set(0x140DD3C50, 0xC3C033); // ^
			utils::hook::set(0x1400C1210, 0xC3C033); // ^ idk
			utils::hook::set(0x1400C12B0, 0xC3C033); // ^ idk
			utils::hook::set(0x140E423A0, 0xC3C033); // directx
			utils::hook::set(0x140E04680, 0xC3C033); // ^

			utils::hook::set(0x140E00ED0, 0xC3C033); // Image_Create1DTexture_PC
			utils::hook::set(0x140E00FC0, 0xC3C033); // Image_Create2DTexture_PC
			utils::hook::set(0x140E011A0, 0xC3C033); // Image_Create3DTexture_PC
			utils::hook::set(0x140E015C0, 0xC3C033); // Image_CreateCubeTexture_PC
			utils::hook::set(0x140E01300, 0xC3C033); // Image_CreateArrayTexture_PC

			utils::hook::set(0x1405F1EA0, 0xC3C033); // renderer
			utils::hook::set(0x1400C1370, 0xC3C033); // ^
			utils::hook::set(0x140DD26E0, 0xC3C033); // directx
			utils::hook::set(0x1405F0610, 0xC3C033); // ^
			utils::hook::set(0x1405F0580, 0xC3C033); // ^
			utils::hook::set(0x1405F0820, 0xC3C033); // ^
			utils::hook::set(0x1405F0790, 0xC3C033); // ^

			utils::hook::set(0x140DD42A0, 0xC3C033); // shutdown
			utils::hook::set(0x140DD42E0, 0xC3C033); // ^
			utils::hook::set(0x140DD4280, 0xC3C033); // ^

			utils::hook::set(0x140DD4230, 0xC3C033); // ^

			// r_loadForRenderer
			//utils::hook::set<uint8_t>(0x140E114A0, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140E11380, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140E113D0, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140E476F0, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140E11420, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140DD2300, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140DD2610, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x140E11F40, 0xC3); // ^

			// skip R_GetFrameIndex check in DB_LoadLevelXAssets
			utils::hook::set<uint8_t>(0x1403B9E72, 0xEB);

			// don't release buffer
			utils::hook::set<uint8_t>(0x140DD4430, 0xC3);

			// R_LoadWorld
			utils::hook::set<uint8_t>(0x140DD14C0, 0xC3);

			// something to do with vls?
			utils::hook::set<uint8_t>(0x140D02CB0, 0xC3);

			// recipe save threads
			utils::hook::set<uint8_t>(0x140E7C970, 0xC3);

			// don't set computeshader program to 0
			utils::hook::set<uint8_t>(0x140E11520, 0xC3);
		}

		void load_code_pre_gfx_zone()
		{
			::zonetool::iw7::XZoneInfo zone{};
			zone.name = "code_pre_gfx";
			zone.allocFlags = ::zonetool::iw7::DB_ZONE_PERMANENT;
			return ::zonetool::iw7::DB_LoadXAssets(&zone, 1, ::zonetool::iw7::DB_LOAD_SYNC);
		}

		void load_common_zones()
		{
			std::vector<const char*> defaultzones;
			if (!utils::flags::has_flag("no_code_post_gfx"))
			{
				defaultzones.push_back("code_post_gfx");
			}
			if (!utils::flags::has_flag("no_common"))
			{
				defaultzones.push_back("common");
			}

			::zonetool::iw7::XZoneInfo zones[8]{ 0 };

			// Load our custom zones
			for (std::size_t i = 0; i < defaultzones.size(); i++)
			{
				zones[i].name = defaultzones[i];
				zones[i].allocFlags = ::zonetool::iw7::DB_ZONE_PERMANENT;
			}

			return ::zonetool::iw7::DB_LoadXAssets(zones, static_cast<unsigned int>(defaultzones.size()), ::zonetool::iw7::DB_LOAD_SYNC);
		}

		void load_init_zones_stub()
		{
			if (!*reinterpret_cast<uint32_t*>(0x146006DAC))
			{
				utils::hook::set<uint32_t>(0x146006DAC, 1); // initialized
				utils::hook::invoke<void>(0x140E75750); // idk
				load_code_pre_gfx_zone();
			}

			load_common_zones();

			::zonetool::iw7::start();

			const auto do_forever = true;
			if (!do_forever)
			{
				return;
			}

			utils::hook::invoke<void>(0x1405AC970); // Com_FastFile_Init

			while (1)
			{
				std::this_thread::sleep_for(5ms);
				utils::hook::invoke<void>(0x140D33F80); // Sys_CheckQuitRequest
				utils::hook::invoke<void>(0x140A7C350); // DB_Update
				utils::hook::invoke<void>(0x140B7C350, 0, 0); // Cbuf_Execute
			}
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
				remove_renderer();

				utils::hook::set(0x140D33DC0, 0xC300000001B8);

				// stop the game after loading init zones
				utils::hook::call(0x140E06224, load_init_zones_stub);

				// disable splash
				utils::hook::set<uint8_t>(0x140D58240, 0xC3); // Sys_CreateSplashWindow
				utils::hook::set<uint8_t>(0x140D584F0, 0xC3); // Sys_DestroySplashWindow
				utils::hook::set<uint8_t>(0x140D58530, 0xC3); // Sys_HideSplashWindow
				utils::hook::set<uint8_t>(0x140D58550, 0xC3); // Sys_ShowSplashWindow

				// disable demonware
				utils::hook::set<uint8_t>(0x140D31744, 0xC3); // dwNetStart

				// don't load streamed assets
				utils::hook::set<uint8_t>(0x140A79AE0, 0xC3);
				utils::hook::set<uint8_t>(0x1403BB990, 0xC3);
				utils::hook::set<uint8_t>(0x140A78910, 0xC3);

				// don't fatal error on misc model error
				utils::hook::nop(0x140571ECF, 5);

				::zonetool::iw7::initialize();
			}
		};
	}
}

REGISTER_COMPONENT_IW7(iw7::zonetool::component)
