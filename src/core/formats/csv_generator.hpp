#pragma once

#include "mapents.hpp"

namespace csv_generator
{
	void generate_map_csv(const std::string& map, const mapents::token_name_callback& get_token_name, bool is_sp = false, game::game_mode game = game::game_mode::h1);
}
