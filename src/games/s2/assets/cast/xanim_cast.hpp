#pragma once

#include <formats/cast/cast_scene.hpp>

namespace zonetool::s2::cast_xanim
{
	XAnimParts* parse(const std::string& name, zone_memory* mem);
	void dump(XAnimParts* parts);

	cast::animation_t to_animation(const XAnimParts* parts); // bone tracks only
	XAnimParts* from_animation(const cast::animation_t& animation, zone_memory* mem);
}
