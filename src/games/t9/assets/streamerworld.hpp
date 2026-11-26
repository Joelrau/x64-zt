#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class streamer_world
	{
	public:
		static void read(auto& reader, StreamerWorld* asset);
	};

	class streamer_hint
	{
	public:
		static void read(auto& reader, StreamerHint* asset);
	};
}
