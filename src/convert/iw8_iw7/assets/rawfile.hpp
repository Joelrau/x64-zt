#pragma once

namespace zonetool::iw8::converter::iw7::rawfile
{
	struct vision_field
	{
		std::string_view iw8_name;
		std::string_view iw7_name;
		std::string_view iw8_default;
	};

	void dump(RawFile* asset);
}
