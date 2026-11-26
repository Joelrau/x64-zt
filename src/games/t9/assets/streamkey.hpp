#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class stream_key
	{
	public:
		static void read(auto& reader, StreamKey* asset);
	};
}
