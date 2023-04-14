#include <std_include.hpp>
#include "loader/component_loader.hpp"

#include "game/h2/game.hpp"
#include "game/h2/dvars.hpp"

#include "command.hpp"

#include <utils/io.hpp>
#include <utils/hook.hpp>
#include <utils/string.hpp>
#include <utils/flags.hpp>

namespace database
{
	namespace
	{
		struct bnet_file_handle_t
		{
			std::unique_ptr<std::ifstream> stream;
			uint64_t offset{};
			std::string path;
		};

		std::unordered_map<h2::game::DB_IFileSysFile*, bnet_file_handle_t> bnet_file_handles{};
		utils::hook::detour bnet_fs_open_file_hook;
		utils::hook::detour bnet_fs_read_hook;
		utils::hook::detour bnet_fs_tell_hook;
		utils::hook::detour bnet_fs_size_hook;
		utils::hook::detour bnet_fs_close_hook;
		utils::hook::detour bnet_fs_exists_hook;

		utils::memory::allocator handle_allocator;

		std::optional<std::string> find_fastfile(const std::string& name)
		{
			std::string name_ = name;

			if (utils::io::file_exists(name))
			{
				return {name};
			}

			name_ = "zone/" + name;
			if (utils::io::file_exists(name_))
			{
				return {name_};
			}

			return {};
		}

		bool is_zone_file(const std::string& file)
		{
			return file.ends_with(".ff") || file.ends_with(".pak");
		}

		bool is_loc_folder(h2::game::Sys_Folder folder)
		{
			return folder == h2::game::SF_PAKFILE_LOC || folder == h2::game::SF_ZONE_LOC || folder == h2::game::SF_VIDEO_LOC;
		}

		h2::game::DB_IFileSysFile* bnet_fs_open_file_stub(h2::game::DB_FileSysInterface* this_,
			h2::game::Sys_Folder folder, const char* file)
		{
			std::string name = file;
			std::string file_ = file;

			const auto search_path = [&](const std::string& ext, const std::string& path)
			{
				if (name.ends_with(ext) && !utils::io::file_exists(name))
				{
					name = path + name;
				}
			};

			if (is_zone_file(name))
			{
				const auto found = find_fastfile(name);
				if (found.has_value())
				{
					name = found.value();
				}
			}

			if (!utils::io::file_exists(name))
			{
				return bnet_fs_open_file_hook.invoke<h2::game::DB_IFileSysFile*>(this_, folder, file_.data());
			}

			const auto handle = handle_allocator.allocate<h2::game::DB_IFileSysFile>();

			try
			{
				auto stream = std::make_unique<std::ifstream>();
				stream->open(name, std::ios::binary);

				bnet_file_handle_t bnet_handle{};
				bnet_handle.stream = std::move(stream);
				bnet_handle.path = name;
				bnet_file_handles[handle] = std::move(bnet_handle);
				return handle;
			}
			catch (const std::exception& e)
			{
				printf("[Database] Error opening file %s: %s\n", name.data(), e.what());
			}

			return handle;
		}

		h2::game::FileSysResult bnet_fs_read_stub(h2::game::DB_FileSysInterface* this_, h2::game::DB_IFileSysFile* handle,
			unsigned __int64 offset, unsigned __int64 size, void* dest)
		{
			if (bnet_file_handles.find(handle) == bnet_file_handles.end())
			{
				return bnet_fs_read_hook.invoke<h2::game::FileSysResult>(this_, handle, offset, size, dest);
			}
			else
			{
				auto& handle_ = bnet_file_handles[handle];
				if (!handle_.stream->is_open())
				{
					return h2::game::FILESYSRESULT_ERROR;
				}

				try
				{
					const auto start_pos = offset - handle_.offset;
					handle_.stream->seekg(0, std::ios::end);
					const auto end_pos = static_cast<uint64_t>(handle_.stream->tellg());
					handle_.stream->seekg(start_pos);

					const auto len = end_pos - start_pos;
					const auto bytes_to_read = len <= size ? len : size;

					const auto& res = handle_.stream->read(reinterpret_cast<char*>(dest), bytes_to_read);
					if (res.bad())
					{
						return h2::game::FILESYSRESULT_ERROR;
					}

					const auto bytes_read = static_cast<uint64_t>(res.gcount());
					handle->bytes_read += bytes_read;
					handle->last_read = bytes_read;

					return h2::game::FILESYSRESULT_SUCCESS;
				}
				catch (const std::exception& e)
				{
					printf("[Database] bnet_fs_read_stub: %s\n", e.what());
					return h2::game::FILESYSRESULT_ERROR;
				}
			}
		}

		h2::game::FileSysResult bnet_fs_tell_stub(h2::game::DB_FileSysInterface* this_, h2::game::DB_IFileSysFile* handle, uint64_t* bytes_read)
		{
			if (bnet_file_handles.find(handle) == bnet_file_handles.end())
			{
				return bnet_fs_tell_hook.invoke<h2::game::FileSysResult>(this_, handle, bytes_read);
			}
			else
			{
				auto& handle_ = bnet_file_handles[handle];
				if (!handle_.stream->is_open())
				{
					return h2::game::FILESYSRESULT_ERROR;
				}

				try
				{
					*bytes_read = handle->last_read;
					return h2::game::FILESYSRESULT_SUCCESS;
				}
				catch (const std::exception& e)
				{
					printf("[Database] bnet_fs_tell_stub: %s\n", e.what());
					return h2::game::FILESYSRESULT_ERROR;
				}
			}
		}

		uint64_t bnet_fs_size_stub(h2::game::DB_FileSysInterface* this_, h2::game::DB_IFileSysFile* handle)
		{
			if (bnet_file_handles.find(handle) == bnet_file_handles.end())
			{
				return bnet_fs_size_hook.invoke<uint64_t>(this_, handle);
			}
			else
			{
				auto& handle_ = bnet_file_handles[handle];
				try
				{
					handle_.stream->seekg(0, std::ios::end);
					const std::streamsize size = handle_.stream->tellg();
					handle_.stream->seekg(0, std::ios::beg);
					return static_cast<uint64_t>(size);
				}
				catch (const std::exception& e)
				{
					printf("[Database] bnet_fs_size_stub: %s\n", e.what());
					return 0;
				}
			}
		}

		void bnet_fs_close_stub(h2::game::DB_FileSysInterface* this_, h2::game::DB_IFileSysFile* handle)
		{
			if (bnet_file_handles.find(handle) == bnet_file_handles.end())
			{
				bnet_fs_close_hook.invoke<uint64_t>(this_, handle);
			}
			else
			{
				handle_allocator.free(handle);
				bnet_file_handles.erase(handle);
			}
		}

		bool bnet_fs_exists_stub(h2::game::DB_FileSysInterface* this_, h2::game::DB_IFileSysFile* handle, const char* filename)
		{
			std::string name = filename;

			const auto search_path = [&](const std::string& ext, const std::string& path)
			{
				if (!name.ends_with(ext))
				{
					return false;
				}

				return utils::io::file_exists(name) || utils::io::file_exists(path + name);
			};

			if (utils::io::file_exists(filename))
			{
				return true;
			}

			if (is_zone_file(name))
			{
				const auto found = find_fastfile(name);
				if (found.has_value())
				{
					return true;
				}
			}

			return bnet_fs_exists_hook.invoke<bool>(this_, handle, filename);
		}
	}

	class component final : public component_interface
	{
	public:
		void post_unpack() override
		{
			const auto bnet_interface = reinterpret_cast<h2::game::DB_FileSysInterface*>(0x140BE82F8);

			bnet_fs_open_file_hook.create(bnet_interface->vftbl->OpenFile, bnet_fs_open_file_stub);
			bnet_fs_read_hook.create(bnet_interface->vftbl->Read, bnet_fs_read_stub);
			bnet_fs_tell_hook.create(bnet_interface->vftbl->Tell, bnet_fs_tell_stub);
			bnet_fs_size_hook.create(bnet_interface->vftbl->Size, bnet_fs_size_stub);
			bnet_fs_close_hook.create(bnet_interface->vftbl->Close, bnet_fs_close_stub);
			bnet_fs_exists_hook.create(bnet_interface->vftbl->Exists, bnet_fs_exists_stub);
		}
	};
}

REGISTER_COMPONENT(database::component)
