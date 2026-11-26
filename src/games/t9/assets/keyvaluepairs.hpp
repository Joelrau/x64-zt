#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class key_value_pairs
	{
	public:
		static void read(auto& reader, KeyValuePairs* asset);
		static void dump(KeyValuePairs* asset);
		static KeyValuePairs* parse(const std::string& name, utils::memory::allocator& allocator);
	};

	class bg_cache
	{
	public:
		static void read(auto& reader, BGCache* asset);
	};
}
