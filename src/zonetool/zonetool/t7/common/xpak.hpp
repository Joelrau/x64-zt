#pragma once

namespace zonetool::t7
{
	namespace xpak
	{
		std::vector<std::uint8_t> get_data_for_xpak_key(uint64_t key, const unsigned int expected_size);
		void clear_cache();
	}
}