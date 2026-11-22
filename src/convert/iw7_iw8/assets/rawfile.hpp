#pragma once

namespace zonetool::iw7::converter::iw8::rawfile
{
	std::string read_text(const RawFile* asset);
	std::string get_map_script_folder(const std::string& map);
	std::array<float, 3> get_sky_color_scale();
	void dump(RawFile* asset);
}
