#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/h2/game.hpp"
#include "game/h2/dvars.hpp"

#include "command.hpp"
#include "imagefiles.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

namespace fastfiles
{
	static utils::concurrency::container<std::string> current_fastfile;

	namespace
	{
		utils::hook::detour db_try_load_x_file_internal_hook;
		utils::hook::detour db_unload_x_zones_hook;

		void db_try_load_x_file_internal(const char* zone_name, const int flags)
		{
			printf("Loading xfile %s\n", zone_name);
			current_fastfile.access([&](std::string& fastfile)
			{
				fastfile = zone_name;
			});
			return db_try_load_x_file_internal_hook.invoke<void>(zone_name, flags);
		}

		void skip_extra_zones_stub(utils::hook::assembler& a)
		{
			const auto skip = a.newLabel();
			const auto original = a.newLabel();

			a.pushad64();
			a.test(r15d, 0x1000); // allocFlags
			a.jnz(skip);

			a.bind(original);
			a.popad64();
			a.mov(r8d, 9);
			a.mov(rdx, 0x140933528);
			a.jmp(0x140415E09);

			a.bind(skip);
			a.popad64();
			a.mov(r14d, 0x1000);
			a.not_(r14d);
			a.and_(r15d, r14d);
			a.jmp(0x140415E29);
		}

		void db_unload_x_zones_stub(const unsigned short* unload_zones,
			const unsigned int unload_count, const bool create_default)
		{
			for (auto i = 0u; i < unload_count; i++)
			{
				const auto zone_name = h2::game::g_zones[unload_zones[i]].name;
				if (zone_name[0] != '\0')
				{
					imagefiles::close_handle(zone_name);
				}
			}

			db_unload_x_zones_hook.invoke<void>(unload_zones, unload_count, create_default);
		}
	}

	std::string get_current_fastfile()
	{
		std::string fastfile_copy;
		return current_fastfile.access<std::string>([&](std::string& fastfile)
		{
			return fastfile;
		});
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			db_try_load_x_file_internal_hook.create(0x1404173B0, db_try_load_x_file_internal);

			db_unload_x_zones_hook.create(0x140417D80, db_unload_x_zones_stub);

			// Allow loading of mixed compressor types
			utils::hook::nop(0x1403E66A7, 2);

			// Don't load extra zones with loadzone
			utils::hook::nop(0x140415DFC, 13);
			utils::hook::jump(0x140415DFC, utils::hook::assemble(skip_extra_zones_stub), true);
		}
	};
}

REGISTER_COMPONENT_H2(fastfiles::component)
