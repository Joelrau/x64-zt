#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "command.hpp"

#include <utils/hook.hpp>

namespace iw6
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
				a.test(ebp, 0x80); // allocFlags
				a.jnz(skip);

				a.bind(original);
				a.popad64();
				a.mov(rdx, 0x140835F28);
				a.mov(rcx, rsi);
				a.call_aligned(strcmp);
				a.jmp(0x1403217C0);

				a.bind(skip);
				a.popad64();
				a.mov(r15d, 0x80);
				a.not_(r15d);
				a.and_(ebp, r15d);
				a.jmp(0x1403217F6);
			}
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
#ifdef DEBUG
				db_init_load_x_file_hook.create(0x1402FC550, &db_init_load_x_file);
#endif

				// Allow loading of unsigned fastfiles
				utils::hook::set<uint8_t>(0x1402FBF23, 0xEB); // main function
				utils::hook::nop(0x1402FC445, 2); // DB_AuthLoad_InflateInit

				// Don't load extra zones with loadzone
				utils::hook::nop(0x1403217B1, 12);
				utils::hook::jump(0x1403217B1, utils::hook::assemble(skip_extra_zones_stub), true);
			}
		};
	}
}

REGISTER_COMPONENT_IW6(iw6::fastfiles::component)
