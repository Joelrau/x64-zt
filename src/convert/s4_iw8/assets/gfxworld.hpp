#pragma once

namespace zonetool::s4::converter::iw8::gfxworld
{
	struct voxel_leaf
	{
		int index;
		float mins[3];
		float size;
	};

	struct world_geometry
	{
		std::vector<unsigned char> positions;
		std::vector<unsigned char> aux;
		std::vector<zonetool::iw8::GfxWorldSurfData> surf_data;
		std::vector<unsigned char> valid;
	};

	void dump(GfxWorld* asset);
}
