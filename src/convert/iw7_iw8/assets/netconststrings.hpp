#pragma once

namespace zonetool::iw7::converter::iw8::netconststrings
{
	void dump(NetConstStrings* asset);
	void add_level_model(const std::string& name);
	void dump_missing_level_strings();
}
