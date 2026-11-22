#pragma once

namespace zonetool::s2::converter::h1::comworld
{
	void dump(zonetool::s2::ComWorld* asset);
	const zonetool::s2::ComWorld* source_world();
	std::uint16_t get_light_env(std::vector<std::uint16_t> lights);
	std::vector<std::uint16_t> get_light_env_lights(std::uint16_t index);
	unsigned int get_light_env_count();
	void finish();
}
