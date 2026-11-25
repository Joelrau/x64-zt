#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace level
	{
		void dump_net_const_strings();
		void dump_empty_glass_world(const char* name);
		void dump_empty_fx_world(const char* name);
		void dump_script(const std::string& name, const std::string& source);
		std::string port_fx(const std::string& fx_source, const std::string& createfx_source);
		void dump_vision(const std::string& name, const std::string& source_text);
	}
}
