#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class rawfile
	{
	public:
		static void read(auto& reader, RawFile* asset);
		static void dump(RawFile* asset);
	};

	class script_parse_tree
	{
	public:
		static void read(auto& reader, ScriptParseTree* asset);
		static void dump(ScriptParseTree* asset);
	};

	class script_using
	{
	public:
		static void read(auto&, ScriptUsing*)
		{
		}
	};
}
