#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/s1/dvars.hpp"

#include "command.hpp"

#include <utils/hook.hpp>

namespace s1
{
	namespace fastfiles
	{
		namespace
		{
			utils::hook::detour db_init_load_x_file_hook;

			void db_init_load_x_file(/*DBFile**/std::uintptr_t file, std::uint64_t offset)
			{
				printf("Loading xfile %s\n", reinterpret_cast<const char*>(file + 24));
				return db_init_load_x_file_hook.invoke<void>(file, offset);
			}

			void skip_extra_zones_stub(utils::hook::assembler& a)
			{
				const auto skip = a.newLabel();
				const auto original = a.newLabel();

				a.pushad64();
				a.test(esi, 0x1000); // allocFlags
				a.jnz(skip);

				a.bind(original);
				a.popad64();
				a.mov(rdx, 0x140809D40);
				a.mov(rcx, rbp);
				a.call(0x1406FE120);
				a.jmp(0x140271B63);

				a.bind(skip);
				a.popad64();
				a.mov(r13d, 0x1000);
				a.not_(r13d);
				a.and_(esi, r13d);
				a.jmp(0x140271D02);
			}
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
#ifdef DEBUG
				db_init_load_x_file_hook.create(0x1402428B0, &db_init_load_x_file);
#endif

				// Allow loading of unsigned fastfiles
				utils::hook::nop(0x1402427A5, 2); // DB_InflateInit

				// Allow loading of mixed compressor types
				utils::hook::nop(0x140242DF7, 2);

				// Don't load extra zones with loadzone
				utils::hook::nop(0x140271B54, 15);
				utils::hook::jump(0x140271B54, utils::hook::assemble(skip_extra_zones_stub), true);
			}
		};
	}
}

REGISTER_COMPONENT_S1(s1::fastfiles::component)
