#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/dvars.hpp"

#include "command.hpp"

#include <utils/hook.hpp>

namespace fastfiles
{
	namespace
	{
		utils::hook::detour db_init_load_x_file_hook;

		void db_init_load_x_file(/*DBFile**/std::uintptr_t file, std::uint64_t offset)
		{
			printf("Loading xfile %s\n", reinterpret_cast<const char*>(file + 32));
			return db_init_load_x_file_hook.invoke<void>(file, offset);
		}

		void skip_extra_zones_stub(utils::hook::assembler& a)
		{
			const auto skip = a.newLabel();
			const auto original = a.newLabel();

			a.pushad64();
			a.test(esi, 0x400); // allocFlags
			a.jnz(skip);

			a.bind(original);
			a.popad64();
			a.mov(rdx, 0x140823080);
			a.mov(rcx, rbp);
			a.call(0x140793730);
			a.jmp(0x1402BDAA0);

			a.bind(skip);
			a.popad64();
			a.mov(r14d, 0x400);
			a.not_(r14d);
			a.and_(esi, r14d);
			a.jmp(0x1402BDB7F);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
#ifdef DEBUG
			db_init_load_x_file_hook.create(0x14028DE30, &db_init_load_x_file);
#endif

			// Allow loading of unsigned fastfiles
			utils::hook::nop(0x14028DDB3, 2); // DB_InflateInit

			// Allow loading of mixed compressor types
			utils::hook::nop(0x14028E447, 2);

			// Don't load extra zones with loadzone
			utils::hook::nop(0x1402BDA91, 15);
			utils::hook::jump(0x1402BDA91, utils::hook::assemble(skip_extra_zones_stub), true);
		}
	};
}

REGISTER_COMPONENT(fastfiles::component)
