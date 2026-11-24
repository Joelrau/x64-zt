#pragma once

namespace zonetool::iw7::converter::iw8::havok
{
	struct ladder
	{
		std::array<float, 3> bottom;
		float height;
		std::array<float, 3> facing;
		float width;
	};

	struct physics_asset
	{
		std::string havok_data;
		std::vector<std::uint32_t> body_contents;
	};

	std::string convert_world_shape(std::string_view packfile);
	std::string convert_ents_shape_list(std::string_view packfile);
	std::vector<ladder> find_ladders(std::string_view packfile);
	std::optional<physics_asset> convert_physics_asset(std::string_view packfile, const std::string& name);
}
