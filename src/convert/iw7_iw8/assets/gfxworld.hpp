#pragma once

namespace zonetool::iw7::converter::iw8::gfxworld
{
	struct world_geometry
	{
		std::vector<unsigned char> positions;
		std::vector<unsigned char> aux;
		std::vector<unsigned short> indices;
		std::vector<zonetool::iw8::GfxWorldSurfData> surf_data;
		std::vector<unsigned int> base_index;
		std::vector<unsigned char> valid;
	};

	struct smodel_layout
	{
		const XModel* model;
		unsigned int first_surface;
		unsigned int surface_count;
		unsigned char lod_count;
		zonetool::iw8::GfxStaticModelLOD lod_info[6];
		float lod_dist[6];
	};

	void dump(GfxWorld* asset);
}
