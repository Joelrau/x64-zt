#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class raw_text_file
	{
	public:
		static void read(auto& reader, RawTextFile* asset);
	};

	class anim_tree
	{
	public:
		static void read(auto& reader, AnimTree* asset);
	};

	class raw_string
	{
	public:
		static void read(auto& reader, RawString* asset);
	};
}
