#pragma once

#include <xsk/gsc/engine/s2.hpp>

namespace zonetool::s2::converter::h1::scriptfile
{
	xsk::gsc::s2::context& s2_context();

	void dump(zonetool::s2::ScriptFile* asset);
	void add_default_map_script(const std::string& name);
	void finish();
}
