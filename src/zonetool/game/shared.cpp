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
				SL_ConvertToString.set(0x1404317F0);
			}
		}

		namespace s1
		{
			void initialize()
			{
				SL_ConvertToString.set(0x1403F0F10);
			}
		}

		namespace h1
		{
			void initialize()
			{
				SL_ConvertToString.set(0x14043B170);
			}
		}

		namespace h2
		{
			void initialize()
			{
				SL_ConvertToString.set(0x1405BFBB0);
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
