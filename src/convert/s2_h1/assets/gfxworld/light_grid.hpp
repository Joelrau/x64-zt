#pragma once

#include <formats/light_grid.hpp>

namespace zonetool::s2::converter::h1::gfxworld
{
	using probe_values = std::array<float, light_grid::probe_sh_count + 1>;

	struct probe_tetrahedron
	{
		unsigned int probes[4];
		float origin[3];
		float inverse[3][3];
	};

	class probe_palette
	{
	public:
		unsigned int add(const GfxSHProbeData& probe);
		unsigned int missing_index() const;
		probe_values average() const;
		void write(zonetool::h1::GfxLightGrid& grid, utils::memory::allocator& allocator) const;

	private:
		std::vector<probe_values> probes_;
		std::unordered_map<std::string, unsigned int> indices_;
	};

	void convert_light_grid(GfxWorld* asset, zonetool::h1::GfxWorld* world, utils::memory::allocator& allocator);
}
