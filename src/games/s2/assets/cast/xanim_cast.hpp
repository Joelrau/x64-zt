#pragma once

#include <formats/cast/cast_scene.hpp>

namespace zonetool::s2::cast_xanim
{
	struct quat_track
	{
		XAnimPartType type = PART_TYPE_NO_QUAT;
		std::vector<std::uint16_t> indices;
		std::vector<std::array<std::int16_t, 4>> frames;
	};

	struct trans_track
	{
		XAnimPartType type = PART_TYPE_NO_TRANS;
		std::vector<std::uint16_t> indices;
		float mins[3]{};
		float size[3]{};
		float constant[3]{};
		std::vector<std::array<std::uint16_t, 3>> frames;
	};

	struct bone_track
	{
		std::string name;
		quat_track quat;
		trans_track trans;
	};

	struct flat_data
	{
		std::vector<std::uint8_t> data_byte;
		std::vector<std::int16_t> data_short;
		std::vector<std::int32_t> data_int;
		std::vector<std::int16_t> random_data_short;
		std::vector<std::uint16_t> indices;
	};

	XAnimParts* parse(const std::string& name, zone_memory* mem);
	void dump(XAnimParts* parts);

	cast::animation_t to_animation(const XAnimParts* parts);
	XAnimParts* from_animation(const cast::animation_t& animation, zone_memory* mem);
}
