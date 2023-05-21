#include <std_include.hpp>
#include "filesystem.hpp"

#include <utils/io.hpp>

namespace zonetool
{
	namespace filesystem
	{
		file::file(const std::string& filepath)
		{
			if (filepath.empty())
			{
				return;
			}

			this->filepath = std::filesystem::path(filepath);
			this->parent_path = this->filepath.parent_path().string();
			this->filename = this->filepath.filename().string();
		}

		file::file()
		{
			this->fp = nullptr;
		}

		file::~file()
		{
			this->close();
		}

		FILE* file::get_fp()
		{
			return this->fp;
		}

		bool file::exists()
		{
			this->open("rb");
			if (this->fp)
			{
				this->close();
				return true;
			}
			return false;
		}

		errno_t file::open(std::string mode, bool use_path, bool is_zone)
		{
			if (use_path)
			{
				if (mode[0] == 'r')
				{
					auto path = get_file_path(this->filepath.string());
					if (!path.empty())
					{
						return fopen_s(&this->fp, (path + this->filepath.string()).data(), mode.data());
					}
				}
				if (mode[0] == 'w' || mode[0] == 'a')
				{
					auto path = get_dump_path();
					auto dir = path + this->parent_path;
					create_directory(dir);
					return fopen_s(&this->fp, (path + this->filepath.string()).data(), mode.data());
				}
			}
			if (is_zone)
			{
				if (mode[0] == 'r')
				{
					auto path = get_zone_path(this->filepath.string());
					if (!path.empty())
					{
						return fopen_s(&this->fp, (path + this->filepath.string()).data(), mode.data());
					}
				}
				if (mode[0] == 'w' || mode[0] == 'a')
				{
					auto path = get_zone_path();
					return fopen_s(&this->fp, (path + this->filepath.string()).data(), mode.data());
				}
			}
			return fopen_s(&this->fp, this->filepath.string().data(), mode.data());
		}

		size_t file::write_string(const std::string& str)
		{
			if (this->fp)
			{
				return fwrite(str.data(), str.size() + 1, 1, this->fp);
			}
			return 0;
		}

		size_t file::write_string(const char* str)
		{
			return this->write_string(std::string(str));
		}

		size_t file::write(const void* buffer, size_t size, size_t count)
		{
			if (this->fp)
			{
				return fwrite(buffer, size, count, this->fp);
			}
			return 0;
		}

		size_t file::write(const std::string& str)
		{
			return this->write(str.data(), str.size(), 1);
		}

		inline size_t get_string_size(FILE* fp)
		{
			auto i = ftell(fp);
			char c;
			size_t size = 0;
			while (fread(&c, sizeof(char), 1, fp) == 1)
			{
				if (c == '\0') // null terminator
				{
					break;
				}
				size++;
			}
			fseek(fp, i, SEEK_SET);
			return size;
		}

		size_t file::read_string(std::string* str)
		{
			if (this->fp)
			{
				auto size = get_string_size(this->fp);
				str->resize(size);
				fread(str->data(), sizeof(char), size + 1, this->fp);
			}
			return 0;
		}

		size_t file::read(void* buffer, size_t size, size_t count)
		{
			if (this->fp)
			{
				return fread(buffer, size, count, this->fp);
			}
			return 0;
		}

		int file::close()
		{
			if (this->fp)
			{
				return fclose(this->fp);
			}
			return -1;
		}

		bool file::create_path()
		{
			return create_directory(this->parent_path);
		}

		std::size_t file::size()
		{
			if (this->fp)
			{
				auto i = ftell(this->fp);
				fseek(this->fp, 0, SEEK_END);

				auto ret = ftell(this->fp);
				fseek(this->fp, i, SEEK_SET);

				return ret;
			}

			return 0;
		}

		std::vector<std::uint8_t> file::read_bytes(std::size_t size)
		{
			if (this->fp && size)
			{
				// alloc vector
				std::vector<std::uint8_t> buffer;
				buffer.resize(size);

				// read data
				fread(&buffer[0], size, 1, this->fp);

				// return data
				return buffer;
			}

			return {};
		}

		std::vector<std::string> load_extra_search_paths()
		{
			std::vector<std::string> paths;
			const auto folders = utils::io::list_files("zonetool_paths");

			for (const auto& folder : folders)
			{
				if (utils::io::directory_exists(folder))
				{
					paths.emplace_back(folder + "\\");
				}
			}

			return paths;
		}

		std::vector<std::string>& get_search_paths()
		{
			static std::vector<std::string> paths;
			return paths;
		}

		void set_fastfile(const std::string& ff)
		{
			const auto extra_paths = load_extra_search_paths();
			auto& search_paths = get_search_paths();

			search_paths.clear();
			search_paths.emplace_back("zonetool\\" + ff + "\\");
			search_paths.emplace_back("zonetool\\");
			search_paths.insert(search_paths.end(), extra_paths.begin(), extra_paths.end());

			fastfile = ff;
		}

		const std::string& get_fastfile()
		{
			return fastfile;
		}

		std::string get_zone_path(const std::string& name)
		{
			auto path = "zone\\" + name;
			if (std::filesystem::exists(path))
			{
				return "zone\\";
			}

			return "";
		}

		std::string get_file_path(const std::string& name)
		{
			const auto& search_paths = get_search_paths();
			for (const auto& search_path : search_paths)
			{
				const auto full_path = search_path + "\\"s + name;
				if (std::filesystem::exists(full_path))
				{
					return search_path + "\\"s;
				}
			}

			return "";
		}

		std::string get_dump_path()
		{
			auto fastfile_dir = fastfile;
			if (fastfile.starts_with("mp_wm_") || fastfile.starts_with("mp_vm_"))
			{
				fastfile_dir = "weapon_models";
			}
			else if (fastfile.ends_with("_tr"))
			{
				fastfile_dir = "transient_assets";
			}

			auto path = "dump\\" + fastfile_dir + "\\";
			if (!std::filesystem::exists(path))
			{
				create_directory(path);
			}

			return path;
		}

		bool create_directory(const std::string& name)
		{
			if (!name.empty())
			{
				return std::filesystem::create_directories(name);
			}
			return false;
		}
	}
}