#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class cinematic_camera
	{
	public:
		static void read(auto& reader, CinematicCamera* asset);
	};

	class cinematic_sequence
	{
	public:
		static void read(auto& reader, CinematicSequence* asset);
	};

	class spectate_camera
	{
	public:
		static void read(auto& reader, SpectateCamera* asset);
	};

	class highlight_reel_info_defines
	{
	public:
		static void read(auto& reader, HighlightReelInfoDefines* asset);
	};

	class highlight_reel_value_table
	{
	public:
		static void read(auto& reader, HighlightReelValueTable* asset);
	};
}
