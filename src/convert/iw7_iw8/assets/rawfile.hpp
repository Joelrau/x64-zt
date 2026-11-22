#pragma once

namespace zonetool::iw7::converter::iw8::rawfile
{
	std::string read_text(const RawFile* asset);
	std::array<float, 3> get_sky_color_scale();
	void dump(RawFile* asset);
}
