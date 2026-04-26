#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw8/game.hpp"

#include "dvars.hpp"

#include <utils/hook.hpp>
#include <utils/flags.hpp>

#include "zonetool/iw8/zonetool.hpp"

namespace iw8
{
	namespace zonetool
	{
		utils::hook::detour sync_gpu_hook;
		void sync_gpu_stub()
		{
			std::this_thread::sleep_for(1ms);
		}

		void init_no_renderer()
		{
			// R_RegisterDvarsAndCmds
			utils::hook::invoke<void>(0x1942C90_b);

			// RB_Tonemap_RegisterDvars
			//utils::hook::invoke<void>(0x04B1320_b);

			static bool initialized = false;
			if (initialized) return;
			initialized = true;

			// R_LoadGraphicsAssets
			utils::hook::invoke<void>(0x1941A50_b); // -> 0x1941A50_b
		}

		void remove_renderer()
		{
			// Disable frontend
			//dvars::override::register_bool("frontEndSceneEnabled", false, game::DVAR_FLAG_READ);
			utils::hook::set<uint8_t>(0x10B29C0_b, 0xC3); // Com_FastFile_Frame_FrontEnd

			// Disable shader preload
			dvars::override::register_bool("r_preloadShaders", false, game::DVAR_FLAG_READ);

			// Disable load for renderer
			//dvars::override::register_bool("r_loadForRenderer", false, game::DVAR_FLAG_READ);

			dvars::override::register_bool("intro", false, game::DVAR_FLAG_READ);

			// Hook R_SyncGpu
			sync_gpu_hook.create(0x15C5940_b, sync_gpu_stub);

			utils::hook::jump(0x15C35C0_b, init_no_renderer, true);
			utils::hook::nop(0x193E33B_b, 5); // R_CreateWindow

			/*
			utils::hook::nop(0x13DAC88_b, 5);			// don't load config file
			//utils::hook::nop(0xB7CE46_b, 5);			// ^
			utils::hook::call(0x1297644_b, ret_true);	// ^ (new version of above)
			utils::hook::set<uint8_t>(0x12B58D0_b, 0xC3); // don't save config file
			*/

			utils::hook::set<uint8_t>(0x11977A0_b, 0xC3); // disable self-registration (PartyHost_AddLocalPlayer)
			utils::hook::set<uint8_t>(0x199B830_b, 0xC3); // render thread
			utils::hook::set<uint8_t>(0x15CAA50_b, 0xC3); // called from Com_Frame, seems to do renderer stuff
			utils::hook::set<uint8_t>(0x15E4FC0_b, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
			utils::hook::set<uint8_t>(0x13FCA00_b, 0xC3); // recommended settings check

			utils::hook::nop(0x136FFC6_b, 2); // unknown check in SV_ExecuteClientMessage
			//utils::hook::nop(0x0C4F407_b, 3); // allow first slot to be occupied
			utils::hook::nop(0x15C52D3_b, 2); // properly shut down dedicated servers
			//utils::hook::nop(0x034296F_b, 2); // ^
			utils::hook::set<uint8_t>(0x1943A80_b, 0xC3); // don't shutdown renderer

			// SOUND patches
			/*
			utils::hook::nop(0x0C93213_b, 5); // snd stream thread
			utils::hook::set<uint8_t>(0x0C93206_b, 0); // snd_active
			utils::hook::set<uint8_t>(0x0CB9150_b, 0xC3); // sound queue thing
			utils::hook::set<uint8_t>(0x0C75550_b, 0xC3); // SD_AllocInit
			utils::hook::set<uint8_t>(0x0C75CA0_b, 0xC3); // SD_Init

			utils::hook::set<uint8_t>(0x0CA30C0_b, 0xC3); // SNDL_AddBank
			utils::hook::set<uint8_t>(0x0CB9090_b, 0xC3); // SND_UpdateWait
			utils::hook::set<uint8_t>(0x03B7D70_b, 0xC3); // something sound related

			utils::hook::set(0x0CFDC40_b, 0xC3C033); // sound stream reading
			*/

			utils::hook::set<uint8_t>(0x1BD01E0_b, 0xC3); // Voice_Init

			// IMAGE patches
			// image stream (pak)
			//utils::hook::set<uint8_t>(0x13A5310_b, 0xC3); // AlwaysLoadedAssets (image file data)
			//utils::hook::nop(0x0A7DC7C_b, 2); // Skip opening pakfiles

			// UI patches
			utils::hook::set<uint8_t>(0x19D1E70_b, 0xC3); // LUI_CoD_Init

			// TODO: IW7 patches
			/*
			utils::hook::set(0x0E06060_b, 0xC3C033); // directx
			utils::hook::set(0x0E05B80_b, 0xC3C033); // ^
			utils::hook::set(0x0DD2760_b, 0xC3C033); // ^
			utils::hook::set(0x0E05E20_b, 0xC3C033); // ^ buffer
			utils::hook::set(0x0E11270_b, 0xC3C033); // ^
			utils::hook::set(0x0DD3C50_b, 0xC3C033); // ^
			utils::hook::set(0x00C1210_b, 0xC3C033); // ^ idk
			utils::hook::set(0x00C12B0_b, 0xC3C033); // ^ idk
			utils::hook::set(0x0E423A0_b, 0xC3C033); // directx
			utils::hook::set(0x0E04680_b, 0xC3C033); // ^

			utils::hook::set(0x0E00ED0_b, 0xC3C033); // Image_Create1DTexture_PC
			utils::hook::set(0x0E00FC0_b, 0xC3C033); // Image_Create2DTexture_PC
			utils::hook::set(0x0E011A0_b, 0xC3C033); // Image_Create3DTexture_PC
			utils::hook::set(0x0E015C0_b, 0xC3C033); // Image_CreateCubeTexture_PC
			utils::hook::set(0x0E01300_b, 0xC3C033); // Image_CreateArrayTexture_PC

			utils::hook::set(0x05F1EA0_b, 0xC3C033); // renderer
			utils::hook::set(0x00C1370_b, 0xC3C033); // ^
			utils::hook::set(0x0DD26E0_b, 0xC3C033); // directx
			utils::hook::set(0x05F0610_b, 0xC3C033); // ^
			utils::hook::set(0x05F0580_b, 0xC3C033); // ^
			utils::hook::set(0x05F0820_b, 0xC3C033); // ^
			utils::hook::set(0x05F0790_b, 0xC3C033); // ^

			utils::hook::set(0x0DD42A0_b, 0xC3C033); // shutdown
			utils::hook::set(0x0DD42E0_b, 0xC3C033); // ^
			utils::hook::set(0x0DD4280_b, 0xC3C033); // ^

			utils::hook::set(0x0DD4230_b, 0xC3C033); // ^
			*/

			// r_loadForRenderer
			//utils::hook::set<uint8_t>(0x0E114A0_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0E11380_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0E113D0_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0E476F0_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0E11420_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0DD2300_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0DD2610_b, 0xC3); // ^
			//utils::hook::set<uint8_t>(0x0E11F40_b, 0xC3); // ^

			// TODO skip R_GetFrameIndex check in DB_LoadLevelXAssets
			//utils::hook::set<uint8_t>(0x03B9E72_b, 0xEB);

			// TODO don't release buffer
			//utils::hook::set<uint8_t>(0x0DD4430_b, 0xC3);

			// TODO R_LoadWorld
			//utils::hook::set<uint8_t>(0x0DD14C0_b, 0xC3);

			// TODO something to do with vls?
			//utils::hook::set<uint8_t>(0x0D02CB0_b, 0xC3);

			// TODO recipe save threads
			//utils::hook::set<uint8_t>(0x0E7C970_b, 0xC3);

			// TODO don't set computeshader program to 0
			//utils::hook::set<uint8_t>(0x0E11520_b, 0xC3);
		}

		void load_code_pre_gfx_zone()
		{
			::zonetool::iw8::XZoneInfo zone{};
			zone.name = "code_pre_gfx";
			zone.allocFlags = ::zonetool::iw8::DB_ZONE_PERMANENT;
			return ::zonetool::iw8::DB_LoadXAssets(&zone, 1, ::zonetool::iw8::DB_LOAD_SYNC, 0);
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
				defaultzones.push_back("comms_mp");
			}

			::zonetool::iw8::XZoneInfo zones[8]{ 0 };

			// Load our custom zones
			for (std::size_t i = 0; i < defaultzones.size(); i++)
			{
				zones[i].name = defaultzones[i];
				zones[i].allocFlags = 1;
				zones[i].priority = 0xFFF;
			}

			::zonetool::iw8::DB_LoadXAssets(zones, static_cast<unsigned int>(defaultzones.size()), ::zonetool::iw8::DB_LOAD_SYNC, 0);
		}

		void load_init_zones_stub()
		{
			printf("load_init_zones_stub\n");
			if (!*reinterpret_cast<uint32_t*>(0xD44EB90_b))
			{
				utils::hook::set<uint32_t>(0xD44EB90_b, 1); // initialized
				//utils::hook::invoke<void>(0x0E75750_b); // idk
				printf("load_code_pre_gfx_zone\n");
				load_code_pre_gfx_zone();
			}

			printf("load_common_zones\n");
			load_common_zones();

			::zonetool::iw8::start();

			const auto do_forever = true;
			if (!do_forever)
			{
				return;
			}

			utils::hook::invoke<void>(0x10B2D50_b); // Com_FastFile_Init

			while (1)
			{
				std::this_thread::sleep_for(5ms);
				utils::hook::invoke<void>(0x13FF5A0_b); // Sys_CheckQuitRequest
				utils::hook::invoke<void>(0x11B18D0_b); // DB_Update
				utils::hook::invoke<void>(0x1295EE0_b, 0, 0); // Cbuf_Execute
			}
		}

		utils::hook::detour com_message_hook;
		void com_message_stub(int channel, const char* msg, int error)
		{
			printf("%s", msg);
			com_message_hook.invoke<void>(channel, msg, error);
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
				printf("yo\n");

				//remove_renderer();

				utils::hook::set(0x13FF110_b, 0xC300000001B8);

				// stop the game after loading init zones
				utils::hook::call(0x1941A54_b, load_init_zones_stub);

				com_message_hook.create(0x12B0660_b, com_message_stub);

				// disable splash
				//utils::hook::set<uint8_t>(0x0D58240_b, 0xC3); // Sys_CreateSplashWindow
				//utils::hook::set<uint8_t>(0x0D584F0_b, 0xC3); // Sys_DestroySplashWindow
				//utils::hook::set<uint8_t>(0x0D58530_b, 0xC3); // Sys_HideSplashWindow
				//utils::hook::set<uint8_t>(0x0D58550_b, 0xC3); // Sys_ShowSplashWindow

				// disable demonware
				utils::hook::set<uint8_t>(0x14EF270_b, 0xC3); // dwNetStart

				// don't load streamed assets
				//utils::hook::set<uint8_t>(0x0A79AE0_b, 0xC3);
				utils::hook::set<uint8_t>(0x0D8FBD0_b, 0xC3); // DB_StreamingInfo_Register (iw7 sorta similar)
				//utils::hook::set<uint8_t>(0x0A78910_b, 0xC3); 

				// don't fatal error on misc model error
				//utils::hook::nop(0x0571ECF_b, 5);

				::zonetool::iw8::initialize();
			}
		};
	}
}

REGISTER_COMPONENT_IW8(iw8::zonetool::component)
