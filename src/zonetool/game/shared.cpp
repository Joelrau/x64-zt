#include <std_include.hpp>

#include "loader/component_loader.hpp"

#include "shared.hpp"

namespace zonetool
{
	namespace
	{
		std::unordered_map<std::uint32_t, void*> x_gfx_globals_map;

		namespace iw6
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x14031F3A0);
				DB_FindXAssetEntry.set(0x14031F2D0);
				DB_IsXAssetDefault.set(0x1403204D0);

				SL_ConvertToString.set(0x1404317F0);

				stream_files.set(0x141E5BF40);
				stream_file_index.set(0x141E5BF14);
			}
		}

		namespace s1
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x14026F0F0);
				DB_FindXAssetEntry.set(0x14026F020);
				DB_IsXAssetDefault.set(0x140270320);

				SL_ConvertToString.set(0x1403F0F10);

				stream_files.set(0x141E57A30);
				stream_file_index.set(0x141E57A04);
			}
		}

		namespace h1
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x1402BAC70);
				DB_FindXAssetEntry.set(0x1402BABA0);
				DB_IsXAssetDefault.set(0x1402BC370);

				SL_ConvertToString.set(0x14043B170);

				stream_files.set(0x1432748B0);
				stream_file_index.set(0x143274884);
			}
		}

		namespace h2
		{
			void initialize()
			{
				DB_FindXAssetHeader.set(0x140412F60);
				DB_FindXAssetEntry.set(0x140412E90);
				DB_IsXAssetDefault.set(0x1404143C0);

				SL_ConvertToString.set(0x1405BFBB0);

				stream_files.set(0x1420B2820);
				stream_file_index.set(0x1420B27F4);
			}
		}
	}

	void* get_x_gfx_globals_for_zone(int zone)
	{
		return x_gfx_globals_map[zone];
	}

	void insert_x_gfx_globals_for_zone(int zone, void* globals)
	{
		x_gfx_globals_map.insert(std::make_pair(zone, globals));
	}

	class shared final : public component_interface
	{ 
	public:
		void post_start() override
		{
			select(iw6::initialize, s1::initialize, h1::initialize, h2::initialize)();
		}
	};

	REGISTER_COMPONENT(shared);
}
