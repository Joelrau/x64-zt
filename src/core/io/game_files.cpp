#include <std_include.hpp>
#include "game_files.hpp"

#include <utils/io.hpp>

namespace zonetool::game_files
{
	namespace
	{
		std::unique_ptr<source> current_source;

		bool has_extension(const std::filesystem::path& path, const std::string_view extension)
		{
			return path.extension() == extension;
		}

		bool read_disk(const std::filesystem::path& path, const std::uint64_t offset, const std::size_t size, void* buffer)
		{
			std::ifstream stream(path, std::ios::binary);
			if (!stream)
			{
				return false;
			}

			stream.seekg(static_cast<std::streamoff>(offset));
			stream.read(static_cast<char*>(buffer), static_cast<std::streamsize>(size));
			return static_cast<std::size_t>(stream.gcount()) == size;
		}

		class folder_source final : public source
		{
		public:
			explicit folder_source(std::filesystem::path folder)
				: folder_(std::move(folder))
			{
			}

			std::string description() const override
			{
				return this->folder_.string();
			}

			std::vector<std::string> list(const std::string_view extension) const override
			{
				std::vector<std::string> names;
				if (!std::filesystem::is_directory(this->folder_))
				{
					return names;
				}

				for (const auto& entry : std::filesystem::recursive_directory_iterator(this->folder_))
				{
					if (entry.is_regular_file() && has_extension(entry.path(), extension))
					{
						names.emplace_back(entry.path().lexically_relative(this->folder_).string());
					}
				}

				return names;
			}

			bool exists(const std::string& name) const override
			{
				return std::filesystem::is_regular_file(this->folder_ / name);
			}

			std::filesystem::path path(const std::string& name) const override
			{
				return this->folder_ / name;
			}

			bool read(const std::string& name, std::string& data) const override
			{
				return utils::io::read_file((this->folder_ / name).string(), &data);
			}

			bool read(const std::string& name, const std::uint64_t offset, const std::size_t size, void* buffer) const override
			{
				return read_disk(this->folder_ / name, offset, size, buffer);
			}

		private:
			std::filesystem::path folder_;
		};
	}

	std::unique_ptr<source> open_folder(const std::filesystem::path& folder)
	{
		return std::make_unique<folder_source>(folder);
	}

	void set(std::unique_ptr<source> source)
	{
		current_source = std::move(source);
	}

	const source& get()
	{
		if (!current_source)
		{
			current_source = open_folder("zone");
		}

		return *current_source;
	}

	std::vector<std::filesystem::path> list_zones()
	{
		std::vector<std::filesystem::path> zones;
		for (const auto& name : get().list(".ff"))
		{
			zones.emplace_back(get().path(name));
		}

		return zones;
	}

	std::optional<std::filesystem::path> find_zone(const std::string_view name)
	{
		const auto file_name = std::filesystem::path(name).replace_extension(".ff");
		if (std::filesystem::is_regular_file(file_name))
		{
			return file_name;
		}

		if (get().exists(file_name.string()))
		{
			return get().path(file_name.string());
		}

		for (const auto& entry : get().list(".ff"))
		{
			if (std::filesystem::path(entry).filename() == file_name)
			{
				return get().path(entry);
			}
		}

		return {};
	}

	bool read(const std::filesystem::path& path, std::string& data)
	{
		if (std::filesystem::is_regular_file(path))
		{
			return utils::io::read_file(path.string(), &data);
		}

		return get().read(path.string(), data);
	}

	bool read(const std::filesystem::path& path, const std::uint64_t offset, const std::size_t size, void* buffer)
	{
		if (std::filesystem::is_regular_file(path))
		{
			return read_disk(path, offset, size, buffer);
		}

		return get().read(path.string(), offset, size, buffer);
	}
}
