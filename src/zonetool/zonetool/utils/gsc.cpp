#include <std_include.hpp>
#include "gsc.hpp"

namespace gsc
{
	namespace iw7
	{
		std::unique_ptr<xsk::gsc::iw7::context> gsc_ctx = std::make_unique<xsk::gsc::iw7::context>();
	}

	namespace iw6
	{
		std::unique_ptr<xsk::gsc::iw6_pc::context> gsc_ctx = std::make_unique<xsk::gsc::iw6_pc::context>();
	}

	namespace s1
	{
		std::unique_ptr<xsk::gsc::s1_pc::context> gsc_ctx = std::make_unique<xsk::gsc::s1_pc::context>();
	}

	namespace h1
	{
		std::unique_ptr<xsk::gsc::h1::context> gsc_ctx = std::make_unique<xsk::gsc::h1::context>();
	}

	namespace h2
	{
		std::unique_ptr<xsk::gsc::h2::context> gsc_ctx = std::make_unique<xsk::gsc::h2::context>();
	}
}