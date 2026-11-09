#pragma once

namespace zonetool::h1::converter::s2::netconststrings
{
	struct net_const_strings
	{
		zonetool::s2::NetConstStringType type;
		zonetool::s2::NetConstStringSource source;
		std::vector<std::string> strings;
	};

	void dump(NetConstStrings* asset);
	void add_level_strings();
	void finish();
}
