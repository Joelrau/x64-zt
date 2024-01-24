#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/iw7/game.hpp"

#include "command.hpp"

#include <utils/hook.hpp>

#include <zlib.h>

#include "zonetool/iw7/structs.hpp"

namespace iw7
{
	namespace fastfiles
	{
		namespace
		{
			utils::hook::detour db_init_load_x_file_hook;
			utils::hook::detour db_is_patch_hook;

#ifdef DEBUG
			utils::hook::detour db_read_x_file_hook;
			utils::hook::detour db_dirty_disc_error_hook;
			utils::hook::detour db_block_decompress_zlib_hook;
#endif

			void db_init_load_x_file_stub(const char* name, std::uint64_t offset)
			{
				printf("Loading xfile %s\n", name);
				return db_init_load_x_file_hook.invoke<void>(name, offset);
			}

			bool check_missing_content_func(const char* zone_name)
			{
				const char* lang_code = game::SEH_GetCurrentLanguageCode();
				char buffer[0x100]{ 0 };
				const auto len = sprintf_s(buffer, "%s_", lang_code);

				if (!strncmp(zone_name, buffer, len))
				{
					printf("Tried to load missing language zone: %s\n", zone_name);
					return true;
				}

				return false;
			}

			bool db_is_patch_stub(const char* name)
			{
				if (db_is_patch_hook.invoke<bool>(name)) return true;
				if (check_missing_content_func(name)) return true;
				return false;
			}

			void skip_extra_zones_stub(utils::hook::assembler& a)
			{
				const auto skip = a.newLabel();
				const auto original = a.newLabel();

				//a.pushad64();
				a.test(edi, game::DB_ZONE_CUSTOM); // allocFlags
				a.jnz(skip);

				a.bind(original);
				//a.popad64();
				a.call(0x1403BC450); // strnicmp_ffotd
				a.mov(r12d, edi);
				a.mov(rdx, 0x141467970); // "patch_"
				a.jmp(0x1403BA9C0);

				a.bind(skip);
				//a.popad64();
				a.mov(r12d, game::DB_ZONE_CUSTOM);
				a.not_(r12d);
				a.and_(edi, r12d);
				a.jmp(0x1403BAC06);
			}

#ifdef DEBUG
			void sys_err_read(const char* msg, ...)
			{
				char buffer[2048]{};
				va_list ap;

				va_start(ap, msg);
				vsnprintf_s(buffer, _TRUNCATE, msg + 1, ap);
				va_end(ap);

				game::Com_Error(game::errorParm::ERR_FATAL, "%s", buffer);
			}

			__int64 db_read_x_file_stub(unsigned __int8* pos, unsigned __int64 size)
			{
				return db_read_x_file_hook.invoke<__int64>(pos, size);
			}

			void db_dirty_disc_error_stub(int err1, int err2)
			{
				return db_dirty_disc_error_hook.invoke<void>(err1, err2);
			}
#endif
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
#ifdef DEBUG
				db_init_load_x_file_hook.create(0x1409E8D10, &db_init_load_x_file_stub);
#endif

				// Don't fatal on certain missing zones
				db_is_patch_hook.create(0x1403BC580, db_is_patch_stub);
				// Don't load extra zones with loadzone
				utils::hook::nop(0x1403BA9B1, 15);
				utils::hook::jump(0x1403BA9B1, utils::hook::assemble(skip_extra_zones_stub), true);

				// Allow loading of unsigned fastfiles
				utils::hook::set<uint8_t>(0x1409E8CAE, 0xEB); // DB_InflateInit

				// Skip signature validation
				utils::hook::set(0x1409E6390, 0xC301B0);

#ifdef DEBUG
				// debug stuff
				utils::hook::call(0x1409E9309, sys_err_read);
				db_read_x_file_hook.create(0x1409E91B0, db_read_x_file_stub);
				db_dirty_disc_error_hook.create(0x1409E89D0, db_dirty_disc_error_stub);
#endif
			}
		};
	}
}

REGISTER_COMPONENT_IW7(iw7::fastfiles::component)
