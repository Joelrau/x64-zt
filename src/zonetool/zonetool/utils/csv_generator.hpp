#pragma once

#include "mapents.hpp"
#include "gsc.hpp"

namespace csv_generator
{
	void generate_map_csv(const std::string& map, const mapents::token_name_callback& get_token_name);

	template <typename T>
	std::function<void(const T& params)> create_command(const mapents::token_name_callback& get_token_name)
	{
		return [=](const T& params)
		{
			if (params.size() < 2)
			{
				ZONETOOL_INFO("Usage: generatecsv <map>");
				return;
			}

			generate_map_csv(params.get(1), get_token_name);
		};
	}
}
