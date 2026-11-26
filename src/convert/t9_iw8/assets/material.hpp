#pragma once

#include <formats/dxil.hpp>

namespace zonetool::t9::converter::iw8::material
{
	struct material_instance
	{
		std::span<const std::uint8_t> data;
		dxil::cbuffer layout;
		const MaterialImageSet* images;
	};

	enum class world_surface_list
	{
		opaque,
		decal,
		trans,
	};

	void dump(const Material* asset);
	std::string dump_model(const Material* asset);
	std::string dump_world(const Material* asset);
	void dump_2d(const GfxImage* image, const std::string& name);
	world_surface_list get_world_surface_list(const Material* asset);
	void dump_donor_techsets();
}
