#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"

#include "command.hpp"

#include <utils/hook.hpp>

namespace fastfiles
{
	namespace
	{
		utils::hook::detour db_try_load_x_file_internal_hook;

		void db_try_load_x_file_internal(const char* zone_name, const int flags)
		{
			printf("Loading xfile %s\n", zone_name);
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
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
#ifdef DEBUG
			db_try_load_x_file_internal_hook.create(0x1404173B0, db_try_load_x_file_internal);
#endif

			// Allow loading of unsigned fastfiles
			//utils::hook::nop(0x14028DDB3, 2); // DB_InflateInit

			// Allow loading of mixed compressor types
			utils::hook::nop(0x1403E66A7, 2);

			// Don't load extra zones with loadzone
			utils::hook::nop(0x140415DFC, 13);
			utils::hook::jump(0x140415DFC, utils::hook::assemble(skip_extra_zones_stub), true);
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
