#pragma once

namespace zonetool::s4::converter::iw8::material
{
	struct stock_texture
	{
		unsigned char index;
		std::string image;
	};

	struct render_state
	{
		std::set<unsigned short> depth_stencil_rasterizer;
		std::set<unsigned int> blend;

		bool operator==(const render_state&) const = default;
	};

	struct stock_material
	{
		std::string techset;
		bool techset_available;
		bool wind;
		render_state state;
		unsigned char layer_count;
		zonetool::iw8::MaterialGeometryType material_type;
		zonetool::iw8::CameraRegion camera_region;
		unsigned char sort_key;
		unsigned short flags;
		std::vector<stock_texture> textures;
		std::vector<zonetool::iw8::MaterialConstantDef> constants;
		int suffix_uses;
	};

	zonetool::iw8::Material* reference(const Material* asset, utils::memory::allocator& allocator);
	int layer_count(const Material* asset);
	bool uses_vertex_layer_mask(const Material* asset);
	bool dump(Material* asset);
	void dump_donor_techsets();
}
