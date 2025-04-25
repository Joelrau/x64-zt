#include <std_include.hpp>
#include "loader/component_loader.hpp"
#include "fastfiles.hpp"

#include "game/h1/game.hpp"

#include "command.hpp"
#include "imagefiles.hpp"

#include <utils/hook.hpp>
#include <utils/concurrency.hpp>

namespace h1
{
	namespace fastfiles
	{
		namespace
		{
			static utils::concurrency::container<std::string> current_fastfile;

			utils::hook::detour db_init_load_x_file_hook;
			utils::hook::detour db_try_load_x_file_internal_hook;

			void db_init_load_x_file(/*DBFile**/std::uintptr_t file, std::uint64_t offset)
			{
#ifdef DEBUG
				printf("Loading xfile %s\n", reinterpret_cast<const char*>(file + 32));
#endif
				return db_init_load_x_file_hook.invoke<void>(file, offset);
			}

			void db_try_load_x_file_internal(const char* zone_name, const int flags)
			{
				current_fastfile.access([&](std::string& fastfile)
				{
					fastfile = zone_name;
				});
				db_try_load_x_file_internal_hook.invoke<void>(zone_name, flags);
			}

			void skip_extra_zones_stub(utils::hook::assembler& a)
			{
				const auto skip = a.newLabel();
				const auto original = a.newLabel();

				a.pushad64();
				a.test(esi, game::DB_ZONE_CUSTOM); // allocFlags
				a.jnz(skip);

				a.bind(original);
				a.popad64();
				a.mov(rdx, 0x140823080);
				a.mov(rcx, rbp);
				a.call(0x140793730);
				a.jmp(0x1402BDAA0);

				a.bind(skip);
				a.popad64();
				a.mov(r14d, game::DB_ZONE_CUSTOM);
				a.not_(r14d);
				a.and_(esi, r14d);
				a.jmp(0x1402BDB7F);
			}

			const char* get_zone_name(const unsigned int index)
			{
				return h1::game::g_zones[index].name;
			}

			utils::hook::detour db_unload_x_zones_hook;
			void db_unload_x_zones_stub(const unsigned short* unload_zones,
				const unsigned int unload_count, const bool create_default)
			{
				for (auto i = 0u; i < unload_count; i++)
				{
					const auto zone_name = get_zone_name(unload_zones[i]);
					if (zone_name[0] != '\0')
					{
						//imagefiles::close_handle(zone_name);
					}
				}

				db_unload_x_zones_hook.invoke<void>(unload_zones, unload_count, create_default);
			}
		}

		std::string get_current_fastfile()
		{
			return current_fastfile.access<std::string>([&](std::string& fastfile)
			{
				return fastfile;
			});
		}

		class component final : public component_interface
		{
		public:
			void post_load() override
			{
				db_init_load_x_file_hook.create(0x14028DE30, &db_init_load_x_file);
				db_try_load_x_file_internal_hook.create(0x1402BFFE0, &db_try_load_x_file_internal);

				db_unload_x_zones_hook.create(0x1402C0BC0, db_unload_x_zones_stub);

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
}

REGISTER_COMPONENT_H1(h1::fastfiles::component)
