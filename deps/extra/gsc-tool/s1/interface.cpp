#include "stdafx.hpp"

#include <xsk/s1.hpp>

#include "interface.hpp"

namespace gsc
{
	std::unique_ptr<xsk::gsc::compiler> compiler()
	{
		auto compiler = std::make_unique<xsk::gsc::s1::compiler>();
		compiler->mode(xsk::gsc::build::prod);
		return compiler;
	}

	std::unique_ptr<xsk::gsc::assembler> assembler()
	{
		return std::make_unique<xsk::gsc::s1::assembler>();
	}
}
