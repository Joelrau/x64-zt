#pragma once

namespace zonetool::iw8::converter::iw7::gfxworld
{
	struct vertex_block
	{
		unsigned int first_vertex;
		unsigned int vertex_count;
	};

	struct world_geometry
	{
		std::vector<zonetool::iw7::GfxWorldVertex> vertices;
		std::vector<unsigned short> indices;
		std::vector<vertex_block> blocks;
		std::vector<unsigned int> zone_index_base;
		std::vector<unsigned char> layer_data;
		std::vector<unsigned int> surface_layer_offsets;
	};

	struct static_model_range
	{
		unsigned int first;
		unsigned int count;
	};

	struct static_model_instance
	{
		const XModel* model;
		const GfxSModelInstanceData* data;
	};

	struct light_grid_probe
	{
		std::array<float, 3> origin;
		std::array<float, 27> coefficients;
		float sun_visibility;
	};

	void dump(GfxWorld* asset);
}
