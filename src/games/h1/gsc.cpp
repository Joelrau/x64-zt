#include <std_include.hpp>
#include "gsc.hpp"

namespace gsc::h1
{
	std::unique_ptr<xsk::gsc::h1::context> gsc_ctx = std::make_unique<xsk::gsc::h1::context>(xsk::gsc::instance::server);
}
