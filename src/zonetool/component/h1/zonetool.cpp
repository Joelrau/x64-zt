#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/h1/game.hpp"

#include "dvars.hpp"

#include "zonetool/h1/zonetool.hpp"

#include <utils/flags.hpp>

namespace zonetool::h1
{
	void load_proto_stub(utils::hook::assembler& a)
	{
		a.pushad64();
		a.xor_(ecx, ecx);
		a.call_aligned(0x14029EF70);
		a.popad64();

		a.jmp(0x14029F286);
	};

	bool load_proto_unknown_patch_check(void* var_proto)
	{
		void** proto = (void**)var_proto;

		if (*proto == (void*)0xFDFDFDFFFFFFFFFFu)
		{
			return true;
		}
		else
		{
			//printf("%s: converting offset to pointer\n", __FUNCTION__);

			// DB_ConvertOffsetToPointer
			utils::hook::invoke<void>(0x1402C4AE0, var_proto);
		}

		return false;
	}

	void load_proto_unknown_stub(utils::hook::assembler& a)
	{
		auto is_proto_valid = a.newLabel();
		auto is_true = a.newLabel();

		a.cmp(qword_ptr(rcx), 0);
		a.jnz(is_proto_valid);
		a.jmp(0x14029EFE8);

		a.bind(is_proto_valid);
		a.pushad64();
		a.call_aligned(load_proto_unknown_patch_check);
		a.test(al, al);
		a.jnz(is_true);
		a.popad64();
		a.jmp(0x14029EFE8);

		a.bind(is_true);
		a.popad64();
		a.mov(rax, qword_ptr(0x145123660));
		a.jmp(0x14029EF9A);
	};

	constexpr unsigned int get_asset_type_size(const XAssetType type)
	{
		constexpr int asset_type_sizes[] =
		{
			1, 1, 1, 1, 1, 1, 1, 1, // 7
			592, 1, 1, 1, 1, 1, 1, 1, // 15
			104, 1, 1, 1, 1, 1, 1, 1, // 23
			1, 1, 1, 1, 1, 1, 1, 1, // 31
			1, 1, 1, 1, 1, 16, 1, 1, // 39
			1, 1, 1, 1, 1, 1, 1, 1, // 47
			1, 1, 1, 1, 1, 1, 1, 1, // 55
			1, 1, 1, 1, 1, 1, 1, 1, // 62
			1, 1, 1, 1, 1, 1, 1, 1  // 70
		};

		return asset_type_sizes[type];
	}

	template <XAssetType Type, size_t Size>
	char* reallocate_asset_pool()
	{
		//printf("asset size: %zi // %d(%p)\n", DB_GetXAssetTypeSize(Type), Type, g_assetPool[Type]);

		constexpr auto element_size = get_asset_type_size(Type);
		static char new_pool[element_size * Size] = { 0 };
		assert(get_asset_type_size(Type) == DB_GetXAssetTypeSize(Type));

		std::memmove(new_pool, g_assetPool[Type], g_poolSize[Type] * element_size);

		g_assetPool[Type] = new_pool;
		g_poolSize[Type] = Size;

		return new_pool;
	}

	void patch_asset_loading()
	{
		// reimplement inlined function that was missing an extra check for proto asset
		utils::hook::jump(0x14029F229, utils::hook::assemble(load_proto_stub), true);
		utils::hook::jump(0x14029EF8D, utils::hook::assemble(load_proto_unknown_stub), true);

		reallocate_asset_pool<ASSET_TYPE_LOCALIZE_ENTRY, 15000>();

		const auto* image_pool = reallocate_asset_pool<ASSET_TYPE_IMAGE, 30000>();
		utils::hook::inject(0x1402BBAA5, image_pool + 8);
		utils::hook::inject(0x1402BBAC3, image_pool + 8);

		const auto* material_pool = reallocate_asset_pool<ASSET_TYPE_MATERIAL, 18000>();
		utils::hook::inject(0x1402BBB02 + 3, material_pool + 8);
		utils::hook::inject(0x1402BBB20 + 3, material_pool + 8);
		utils::hook::inject(0x1402BBB6F + 3, material_pool + 8);
		utils::hook::inject(0x1402BF42A + 3, material_pool + 8);

		utils::hook::set<uint8_t>(0x1402C6060, 0xC3); // dcache zone

		utils::hook::set<uint8_t>(0x1402C6340, 0xC3); // alwaysloaded
		utils::hook::set<uint8_t>(0x1402C5F90, 0xC3); // ^^

		utils::hook::set<uint8_t>(0x14004EB80, 0xC3); // parse costume table

		// find empty stringtable, since "mp/costumeOverrideTable.csv" doesnt exist
		utils::hook::inject(0x14004E279, "mp/defaultstringtable.csv");

		// patch customization limits
		utils::hook::set<int32_t>(0x140810CE8, 0x2);	// gender
		utils::hook::set<int32_t>(0x140810CE8 + 4, 0x100);	// shirt
		utils::hook::set<int32_t>(0x140810CE8 + 8, 0x100);	// head
		utils::hook::set<int32_t>(0x140810CE8 + 12, 0x100);	// gloves

		utils::hook::set<int32_t>(0x140810CF8, 0x2);	// gender
		utils::hook::set<int32_t>(0x140810CF8 + 4, 0x100);	// shirt
		utils::hook::set<int32_t>(0x140810CF8 + 8, 0x100);	// head
		utils::hook::set<int32_t>(0x140810CF8 + 12, 0x100);	// gloves
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
		utils::hook::invoke<void>(0x1405DF4B0);
	}

	void remove_renderer()
	{
		//printf("Renderer disabled...\n");

		// Hook R_SyncGpu
		utils::hook::jump(0x1405E12F0, sync_gpu_stub, true);

		utils::hook::jump(0x140254800, init_no_renderer, true);

		// Disable VirtualLobby
		::h1::dvars::override::register_bool("virtualLobbyEnabled", false, ::h1::game::DVAR_FLAG_READ);

		// Disable r_preloadShaders
		::h1::dvars::override::register_bool("r_preloadShaders", false, ::h1::game::DVAR_FLAG_READ);

		utils::hook::nop(0x1404ED90E, 5); // don't load config file
		//utils::hook::nop(0x140403D92, 5); // ^ ( causes the game to take long to bootup )
		utils::hook::set<uint8_t>(0x1400DC1D0, 0xC3); // don't save config file
		utils::hook::set<uint8_t>(0x140274710, 0xC3); // disable self-registration
		utils::hook::set<uint8_t>(0x140515890, 0xC3); // init sound system (1)
		utils::hook::set<uint8_t>(0x1406574F0, 0xC3); // init sound system (2)
		utils::hook::set<uint8_t>(0x140620D10, 0xC3); // render thread
		utils::hook::set<uint8_t>(0x14025B850, 0xC3); // called from Com_Frame, seems to do renderer stuff
		utils::hook::set<uint8_t>(0x1402507B0, 0xC3); // CL_CheckForResend, which tries to connect to the local server constantly
		utils::hook::set<uint8_t>(0x1405D5178, 0x00); // r_loadForRenderer default to 0
		utils::hook::set<uint8_t>(0x14050C2D0, 0xC3); // recommended settings check - TODO: Check hook
		utils::hook::set<uint8_t>(0x140514C00, 0xC3); // some mixer-related function called on shutdown
		utils::hook::set<uint8_t>(0x140409830, 0xC3); // dont load ui gametype stuff

		utils::hook::nop(0x140481B06, 6); // unknown check in SV_ExecuteClientMessage
		utils::hook::nop(0x140480FAC, 4); // allow first slot to be occupied
		utils::hook::nop(0x14025619B, 2); // properly shut down dedicated servers
		utils::hook::nop(0x14025615E, 2); // ^
		utils::hook::nop(0x1402561C0, 5); // don't shutdown renderer

		utils::hook::set<uint8_t>(0x140091840, 0xC3); // something to do with blendShapeVertsView
		utils::hook::nop(0x140659A0D, 8); // sound thing

		// (COULD NOT FIND IN H1)
		// utils::hook::set<uint8_t>(0x1404D6960, 0xC3); // cpu detection stuff?
		utils::hook::set<uint8_t>(0x1405E97F0, 0xC3); // gfx stuff during fastfile loading
		utils::hook::set<uint8_t>(0x1405E9700, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405E9790, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1402C1180, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405E9750, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405AD5B0, 0xC3); // directx stuff
		utils::hook::set<uint8_t>(0x1405DB150, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140625220, 0xC3); // ^ - mutex
		utils::hook::set<uint8_t>(0x1405DB650, 0xC3); // ^

		utils::hook::set<uint8_t>(0x14008B5F0, 0xC3); // rendering stuff
		utils::hook::set<uint8_t>(0x1405DB8B0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DB9C0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DC050, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DCBA0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DD240, 0xC3); // ^ 

		// shaders
		utils::hook::set<uint8_t>(0x1400916A0, 0xC3); // ^
		utils::hook::set<uint8_t>(0x140091610, 0xC3); // ^
		utils::hook::set<uint8_t>(0x14061ACC0, 0xC3); // ^ - mutex

		utils::hook::set<uint8_t>(0x140516080, 0xC3); // idk
		utils::hook::set<uint8_t>(0x1405AE5F0, 0xC3); // ^

		utils::hook::set<uint8_t>(0x1405E0B30, 0xC3); // R_Shutdown
		utils::hook::set<uint8_t>(0x1405AE400, 0xC3); // shutdown stuff
		utils::hook::set<uint8_t>(0x1405E0C00, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1405DFE50, 0xC3); // ^

		// utils::hook::set<uint8_t>(0x1404B67E0, 0xC3); // sound crashes (H1 - questionable, function looks way different)

		utils::hook::set<uint8_t>(0x14048B660, 0xC3); // disable host migration

		utils::hook::set<uint8_t>(0x14042B2E0, 0xC3); // render synchronization lock
		utils::hook::set<uint8_t>(0x14042B210, 0xC3); // render synchronization unlock

		utils::hook::set<uint8_t>(0x140176D2D, 0xEB); // LUI: Unable to start the LUI system due to errors in main.lua

		utils::hook::set<uint8_t>(0x1402C5F90, 0xC3); // disable load/read of alwaysloaded assets ( streamed images )
		utils::hook::set<uint8_t>(0x1402C6340, 0xC3); // ^
		utils::hook::set<uint8_t>(0x1402C5C00, 0xC3); // DB_EnterStreamingTabulate

		utils::hook::set<uint8_t>(0x1402C6590, 0xC3); // DB_ReadPackedLoadedSounds
		utils::hook::set<uint8_t>(0x1402C6000, 0xC3); // DB_LoadPackedLoadedSounds

		utils::hook::set<uint8_t>(0x1402BF7F0, 0xC3); // some loop
		utils::hook::set<uint8_t>(0x14007E150, 0xC3); // related to shader caching / techsets / fastfiles

		// Reduce min required memory
		utils::hook::set<uint64_t>(0x14050C717, 0x80000000);
	}

	void load_common_zones()
	{
		std::vector<std::string> defaultzones;
		if (!utils::flags::has_flag("no_code_post_gfx"))
		{
			defaultzones.push_back(utils::flags::has_flag("sp") ? "code_post_gfx" : "code_post_gfx_mp");
		}
		if (!utils::flags::has_flag("no_common"))
		{
			defaultzones.push_back(utils::flags::has_flag("sp") ? "common" : "common_mp");

			defaultzones.push_back("techsets_common_mp");
			defaultzones.push_back("techsets_common");
		}

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

		zonetool::h1::start();

		while (1)
		{
			std::this_thread::sleep_for(5ms);
			utils::hook::invoke<void>(0x140511420); // Sys_CheckQuitRequest
			utils::hook::invoke<void>(0x1402C0DE0); // DB_Update
			utils::hook::invoke<void>(0x140403470, 0, 0); // Cbuf_Execute
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			patch_asset_loading();

			remove_renderer();

			// stop the game after loading common zones
			utils::hook::call(0x1405DF5C1, load_common_zones_stub);

			// disable splash
			utils::hook::set<uint8_t>(0x140513840, 0xC3);

			// disable demonware
			utils::hook::set<uint8_t>(0x140543730, 0xC3); // dwNetStart

			zonetool::h1::initialize();
		}
	};
}

REGISTER_COMPONENT_H1(zonetool::h1::component)
