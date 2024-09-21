#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace zonetool
{
	namespace filesystem
	{
		static std::string fastfile;

		class file
		{
		public:
			file(const std::string& filepath);

			file();

			file(const file& other);
			file(file&& other) noexcept;
			file& operator=(const file& other);
			file& operator=(file&& other) noexcept;

			~file();

			void initialize(const std::filesystem::path& filepath);

			FILE* get_fp();
			bool exists(bool use_path);
			bool exists(void);

			errno_t open(std::string mode = "wb", bool use_path = true, bool is_zone = false);

			size_t write_string(const std::string& str);
			size_t write_string(const char* str);
			size_t write(const std::string& str);
			size_t write(const void* buffer, size_t size, size_t count);

			template <typename T> size_t write(const T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->write(reinterpret_cast<const void*>(val), size, count);
			}

			int seek(size_t offset, int origin);
			size_t tell();

			size_t read_string(std::string* str);
			size_t read(void* buffer, size_t size, size_t count);

			template <typename T> size_t read(T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->read(reinterpret_cast<void*>(val), size, count);
			}

			template <typename T> size_t read(const T* val, size_t size = sizeof(T), size_t count = 1)
			{
				return this->read(const_cast<T*>(val), size, count);
			}

			int close();

			bool create_path();

			std::size_t size();
			std::vector<uint8_t> read_bytes(std::size_t size);

		private:
			FILE* fp = {};

			std::filesystem::path filepath;
			std::string parent_path;
			std::string filename;

		};

		void set_fastfile(const std::string& ff);
		const std::string& get_fastfile();
		std::string get_zone_path(const std::string& name = "");
		std::string get_file_path(const std::string& name);
		std::string get_dump_path();
		bool create_directory(const std::string& name);
		void add_paths_from_directory(const std::string& dir, bool insert_at_beginning = false);
		std::vector<std::string>& get_search_paths();
	}
}