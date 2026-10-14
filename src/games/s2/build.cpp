#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	void build_zone(const std::filesystem::path&, const std::string& name)
	{
		throw std::runtime_error(std::format("buildzone is not supported for s2 yet ({})", name));
	}
}
