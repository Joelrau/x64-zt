#pragma once

namespace zonetool::t9::converter::iw8::gfxworld
{
	struct placement_transform
	{
		float axis[3][3];
		float origin[3];
		float scale;
	};

	struct placed_model
	{
		const xmodel::static_model_info* model;
		placement_transform transform;
		zonetool::iw8::Bounds bounds;
	};

	struct world_geometry
	{
		std::vector<char> positions;
		std::vector<char> aux;
		std::vector<unsigned short> indices;
		std::vector<zonetool::iw8::GfxWorldSurfData> surf_data;
		std::vector<zonetool::iw8::GfxSurface> surfaces;
		std::vector<zonetool::iw8::GfxSurfaceBounds> surface_bounds;
		unsigned int opaque_count;
		unsigned int decal_end;
	};

	using collision_triangle = std::array<std::array<float, 3>, 3>;

	std::vector<placed_model> get_placed_models(const Districts* asset, std::vector<StaticModelPlacement>& placements);
	std::array<float, 3> transform_point(const placement_transform& transform, const std::array<float, 3>& point);
	zonetool::iw8::Bounds get_world_bounds(const Districts* asset, const std::vector<placed_model>& models);
	std::vector<collision_triangle> get_collision_triangles(const Districts* asset);
	void dump(const Districts* asset, const std::vector<placed_model>& models, const std::vector<StaticModelPlacement>& placements,
		const std::optional<std::string>& sky_material, const std::string& map_name);
}
