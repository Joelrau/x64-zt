#include <std_include.hpp>
#include "iwd.hpp"

#include <utils/string.hpp>

#include <unzip.h>

namespace iwd
{
	namespace
	{
		std::unordered_map<std::string, std::filesystem::path> archive_of_file;

		std::string normalize_name(std::string name)
		{
			std::ranges::replace(name, '\\', '/');
			return utils::string::to_lower(name);
		}

		void index_archive(const std::filesystem::path& archive)
		{
			const auto file = unzOpen(archive.string().data());
			if (!file)
			{
				return;
			}

			for (auto result = unzGoToFirstFile(file); result == UNZ_OK; result = unzGoToNextFile(file))
			{
				char name[512]{};
				if (unzGetCurrentFileInfo(file, nullptr, name, sizeof(name), nullptr, 0, nullptr, 0) == UNZ_OK)
				{
					archive_of_file.try_emplace(normalize_name(name), archive);
				}
			}

			unzClose(file);
		}

		bool read_from_archive(const std::filesystem::path& archive, const std::string& name, std::string& out)
		{
			const auto file = unzOpen(archive.string().data());
			if (!file)
			{
				return false;
			}

			auto found = false;
			if (unzLocateFile(file, name.data(), 2) == UNZ_OK && unzOpenCurrentFile(file) == UNZ_OK)
			{
				unz_file_info info{};
				unzGetCurrentFileInfo(file, &info, nullptr, 0, nullptr, 0, nullptr, 0);
				out.resize(info.uncompressed_size);
				found = unzReadCurrentFile(file, out.data(), static_cast<unsigned int>(out.size())) == static_cast<int>(out.size());
				unzCloseCurrentFile(file);
			}

			unzClose(file);
			return found;
		}
	}

	void set_root(const std::filesystem::path& game_root)
	{
		std::vector<std::filesystem::path> archives;
		for (const auto& folder : {game_root / "main", game_root / "zone" / "english"})
		{
			std::error_code error;
			for (const auto& entry : std::filesystem::directory_iterator(folder, error))
			{
				if (entry.path().extension() == ".iwd")
				{
					archives.push_back(entry.path());
				}
			}
		}

		std::ranges::sort(archives, std::greater<>());

		archive_of_file.clear();
		for (const auto& archive : archives)
		{
			index_archive(archive);
		}
	}

	std::string read_file(const std::string& name)
	{
		const auto normalized = normalize_name(name);
		const auto archive = archive_of_file.find(normalized);
		if (archive == archive_of_file.end())
		{
			return {};
		}

		std::string data;
		read_from_archive(archive->second, normalized, data);
		return data;
	}
}
