#pragma once

#include "mapents.hpp"
#include "gsc.hpp"

namespace csv_generator
{
	void generate_map_csv(const std::string& map, const mapents::token_name_callback& get_token_name, bool is_sp = false);

	template <typename T>
	std::function<void(const T& params)> create_command(const mapents::token_name_callback& get_token_name)
	{
		return [=](const T& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_INFO("Usage: generatecsv <map> [mode]");
				return;
			}

			auto is_sp = false;
			if (params.size() >= 3)
			{
				is_sp = params.get(2) == "sp"s;
			}

			generate_map_csv(params.get(1), get_token_name, is_sp);
		};
	}
}
