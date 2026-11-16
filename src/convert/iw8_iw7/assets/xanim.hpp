#pragma once
#include "../include.hpp"

namespace zonetool::iw8::converter::iw7::xanim
{
	struct anim_track
	{
		std::uint32_t name;
		zonetool::iw8::XAnimPartType type;
		std::vector<char> data_byte;
		std::vector<short> data_short;
		std::vector<float> data_float;
		std::vector<short> random_short;
		std::vector<char> random_byte;
		std::vector<unsigned short> indices;
	};

	struct anim_tracks
	{
		std::vector<anim_track> quats;
		std::vector<anim_track> trans;
	};

	void dump(XAnimParts* asset);
}
