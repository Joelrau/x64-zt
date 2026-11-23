#pragma once
#include "../include.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace gfx_image
	{
		struct decoded_image
		{
			unsigned short width = 0;
			unsigned short height = 0;
			bool has_alpha = false;
			std::vector<std::vector<std::uint8_t>> mips; // rgba, largest first
		};

		struct bc7_mode6_fit
		{
			int low[4]{};
			int high[4]{};
			int low_p = 0;
			int high_p = 0;
			int indices[16]{};
			float error = FLT_MAX;
		};

		inline std::function<std::string(const std::string&)> read_file;

		std::uint8_t convert_semantic(std::uint8_t semantic);
		zonetool::iw7::GfxImage* convert(zonetool::iw5::GfxImage* asset, utils::memory::allocator& allocator);
		void dump(zonetool::iw5::GfxImage* asset);

		bool decode(zonetool::iw5::GfxImage* asset, decoded_image& out);
		zonetool::iw7::GfxImage* build_packed_cs(const char* name, const decoded_image& colour, utils::memory::allocator& allocator);
		zonetool::iw7::GfxImage* build_packed_ng(const char* name, const decoded_image& normal, const decoded_image* spec,
			utils::memory::allocator& allocator);
		zonetool::iw7::GfxImage* build_packed_a(const char* name, const decoded_image& colour, utils::memory::allocator& allocator);

		zonetool::iw7::GfxImage* generate_lightmap_second_unorm(const char* name, unsigned short width, unsigned short height,
			utils::memory::allocator& allocator);
		zonetool::iw7::GfxImage* generate_ies_lookup(utils::memory::allocator& allocator);
		zonetool::iw7::GfxImage* generate_reflection_probe_array(const std::vector<zonetool::iw7::GfxImage*>& probes,
			utils::memory::allocator& allocator);
	}
}
