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

			utils::hook::detour db_read_x_file_hook;
			utils::hook::detour db_dirty_disc_error_hook;
			utils::hook::detour db_block_decompress_zlib_hook;

			utils::hook::detour db_auth_load_inflate_hook;
			utils::hook::detour db_auth_load_inflate_init_hook;
			utils::hook::detour db_auth_load_inflate_end_hook;

			std::uint8_t db_zipMemory[0x20000];

			void db_init_load_x_file_stub(const char* name, std::uint64_t offset)
			{
				printf("Loading xfile %s\n", name);
				return db_init_load_x_file_hook.invoke<void>(name, offset);
			}

			bool check_missing_content_func(const char* zone_name)
			{
				const char* lang_code = game::SEH_GetCurrentLanguageCode();
				char buffer[0x100]{ 0 };
				sprintf_s(buffer, "%s_", lang_code);

				if (!strncmp(zone_name, buffer, strlen(buffer)))
				{
					printf("Tried to load missing language zone: %s\n", zone_name);
					return true;
				}

				return false;
			}

			bool db_is_patch_stub(const char* name)
			{
				if (check_missing_content_func(name)) return true;

				return db_is_patch_hook.invoke<bool>(name);
			}

			void sys_err_read(const char* msg, ...)
			{
#ifdef DEBUG
				__debugbreak();
#endif

				char buffer[2048]{};
				va_list ap;

				va_start(ap, msg);
				vsnprintf_s(buffer, _TRUNCATE, msg + 1, ap);
				va_end(ap);

				game::Com_Error(game::errorParm::ERR_FATAL, buffer);
			}

			__int64 db_read_x_file_stub(unsigned __int8* pos, unsigned __int64 size)
			{
				return db_read_x_file_hook.invoke<__int64>(pos, size);
			}

			void db_dirty_disc_error_stub(int err1, int err2)
			{
				return db_dirty_disc_error_hook.invoke<void>(err1, err2);
			}

			void db_block_decompress_zlib_stub(int a1, int a2, int a3, int a4)
			{
				return db_block_decompress_zlib_hook.invoke<void>(a1, a2, a3, a4);
			}

			template <class T1, class T2>
			void stream_copy(T1 dest, T2 src)
			{
				dest->next_in = static_cast<decltype(dest->next_in)>(src->next_in);
				dest->avail_in = static_cast<decltype(dest->avail_in)>(src->avail_in);
				dest->total_in = static_cast<decltype(dest->total_in)>(src->total_in);
				dest->next_out = static_cast<decltype(dest->next_out)>(src->next_out);
				dest->avail_out = static_cast<decltype(dest->avail_out)>(src->avail_out);
				dest->total_out = static_cast<decltype(dest->total_out)>(src->total_out);
			}

			__int64 db_auth_load_inflate_stub(game::DB_ReadStream* stream)
			{
				__int64 result{};

				if (!game::g_load->file->isSecure)
				{
					stream_copy(game::db_zlib_stream, stream);
					if (game::db_zlib_stream->avail_in)
					{
						result = game::db_inflate(game::db_zlib_stream, Z_SYNC_FLUSH);
						if (result == 1 && !game::db_zlib_stream->avail_out) result = 0;
					}
					stream_copy(stream, game::db_zlib_stream);
					return result;
				}

				result = db_auth_load_inflate_hook.invoke<__int64>(stream);
				return result;
			}

			void db_auth_load_inflate_init_stub(game::DB_ReadStream* stream, int isSecure, const char* filename)
			{
				db_auth_load_inflate_init_hook.invoke<void>(stream, isSecure, filename);

				if (!isSecure)
				{
					memset(game::db_zlib_stream, 0, sizeof(game::db_z_stream_s));
					stream_copy(game::db_zlib_stream, stream);

					game::DB_Zip_InitThreadMemory(game::db_zlib_stream, db_zipMemory, 0x20000);
					game::db_inflateInit_(game::db_zlib_stream, "1.1.4", 88);

					stream_copy(stream, game::db_zlib_stream);
				}
			}

			void db_auth_load_inflate_end_stub()
			{
				db_auth_load_inflate_end_hook.invoke<void>();

				if (!game::g_load->file->isSecure)
				{
					game::db_inflateEnd(game::db_zlib_stream);
					game::DB_Zip_ShutdownThreadMemory(game::db_zlib_stream);
				}
			}

			utils::hook::detour db_patch_mem_fix_stream_alignment_hook;
			unsigned __int64 db_patch_mem_fix_stream_alignment_stub(int alignment) // probably unneeded
			{
				if (!game::g_load->file->isSecure)
				{
					*game::g_streamPos = (~alignment & (alignment + *game::g_streamPos));
					return *game::g_streamPos;
				}

				return db_patch_mem_fix_stream_alignment_hook.invoke<unsigned __int64>(alignment);
			}
		}

		class component final : public component_interface
		{
		public:
			void post_unpack() override
			{
				db_patch_mem_fix_stream_alignment_hook.create(0x1400A0A80, db_patch_mem_fix_stream_alignment_stub);
#ifdef DEBUG
				db_init_load_x_file_hook.create(0x1409E8D10, &db_init_load_x_file_stub);
#endif

				db_is_patch_hook.create(0x1403BC580, db_is_patch_stub);

				utils::hook::call(0x1409E9309, sys_err_read);
				db_read_x_file_hook.create(0x1409E91B0, db_read_x_file_stub);
				db_dirty_disc_error_hook.create(0x1409E89D0, db_dirty_disc_error_stub);
				db_block_decompress_zlib_hook.create(0x1409E6DA0, db_block_decompress_zlib_stub);

				// Allow loading of unsigned fastfiles
				utils::hook::set<uint8_t>(0x1409E8CAE, 0xEB); // DB_InflateInit
				db_auth_load_inflate_hook.create(0x1409E6AE0, db_auth_load_inflate_stub);
				db_auth_load_inflate_init_hook.create(0x1409E6970, db_auth_load_inflate_init_stub);
				db_auth_load_inflate_end_hook.create(0x1409E8CF0, db_auth_load_inflate_end_stub);

				// Don't load extra zones with loadzone
				//utils::hook::nop(0x1403217B1, 12);
				//utils::hook::jump(0x1403217B1, utils::hook::assemble(skip_extra_zones_stub), true);
			}
		};
	}
}

REGISTER_COMPONENT_IW7(iw7::fastfiles::component)
