#include <std_include.hpp>
#include "iwd.hpp"

#include <unzip.h>

namespace zonetool::iw3
{
	namespace
	{
		std::filesystem::path root;

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

	void set_iwd_root(const std::filesystem::path& game_root)
	{
		root = game_root;
	}

	std::string read_iwd_file(const std::string& name)
	{
		std::vector<std::filesystem::path> archives;
		for (const auto& folder : {root / "main", root / "zone" / "english"})
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
		for (const auto& archive : archives)
		{
			std::string data;
			if (read_from_archive(archive, name, data))
			{
				return data;
			}
		}

		return {};
	}
}
