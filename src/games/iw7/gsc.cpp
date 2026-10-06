#include <std_include.hpp>
#include "gsc.hpp"

namespace gsc::iw7
{
	std::unique_ptr<xsk::gsc::iw7::context> gsc_ctx = std::make_unique<xsk::gsc::iw7::context>(xsk::gsc::instance::server);
}
