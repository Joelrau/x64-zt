#pragma once

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace rawfile
		{
			struct fog_set
			{
				float color_intensity = 1.0f;
				float sun_intensity = 1.0f;
				float sky_min_angle = 0.0f;
				float sky_max_angle = 90.0f;
				std::string hdr_override;
			};

			std::unordered_map<std::string, std::string> read_vision(const std::string& name);
			std::array<float, 4> get_sky_intensity();
			zonetool::h1::RawFile* convert(RawFile* asset, utils::memory::allocator& allocator);
			void dump(RawFile* asset);
		}
	}
}