#pragma once

namespace iwd
{
	void set_root(const std::filesystem::path& game_root);
	std::string read_file(const std::string& name);
}
