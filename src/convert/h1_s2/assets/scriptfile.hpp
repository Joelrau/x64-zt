#pragma once

#include <xsk/gsc/engine/h1.hpp>

namespace zonetool::h1::converter::s2::scriptfile
{
	xsk::gsc::h1::context& h1_context();
	std::string patch(const std::string& name, const std::string& source);
	void dump(ScriptFile* asset);
}
