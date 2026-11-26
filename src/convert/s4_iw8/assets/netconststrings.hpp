#pragma once

namespace zonetool::s4::converter::iw8::netconststrings
{
	void dump(NetConstStrings* asset);
	void add_level_string(zonetool::iw8::NetConstStringType type, const std::string& string);
	void dump_level_strings();
}
