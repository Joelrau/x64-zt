#pragma once

namespace zonetool::iw3
{
	void set_iwd_root(const std::filesystem::path& game_root);
	std::string read_iwd_file(const std::string& name);
}
