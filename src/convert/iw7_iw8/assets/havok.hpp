#pragma once

namespace zonetool::iw7::converter::iw8::havok
{
	std::string convert_world_shape(std::string_view packfile);
	std::string convert_ents_shape_list(std::string_view packfile);
}
