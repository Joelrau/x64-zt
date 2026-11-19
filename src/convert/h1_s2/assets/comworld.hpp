#pragma once

namespace zonetool::h1::converter::s2::comworld
{
	void dump(ComWorld* asset);
	const ComWorld* source_world();
	std::vector<std::uint16_t> env_light_indices(const ComWorld& com_world, const ComPrimaryLightEnv& env);
	void finish();
	void clear();
}
