#pragma once

namespace zonetool::xfile::oodle
{
	void load(const std::filesystem::path& game_folder);
	void decompress(std::string_view source, char* destination, std::size_t size);
	std::string compress(std::string_view source);
}
