#pragma once

#include <xsk/gsc/engine/iw6_pc.hpp>
#include <xsk/gsc/engine/s1_pc.hpp>
#include <xsk/gsc/engine/h1.hpp>
#include <xsk/gsc/engine/h2.hpp>

namespace gsc
{
	namespace iw6
	{
		extern std::unique_ptr<xsk::gsc::iw6_pc::context> gsc_ctx;
	}

	namespace s1
	{
		extern std::unique_ptr<xsk::gsc::s1_pc::context> gsc_ctx;
	}

	namespace h1
	{
		extern std::unique_ptr<xsk::gsc::h1::context> gsc_ctx;
	}

	namespace h2
	{
		extern std::unique_ptr<xsk::gsc::h2::context> gsc_ctx;
	}
}