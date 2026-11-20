#pragma once

#include "techset.hpp"

namespace zonetool::iw7::converter::iw8::material
{
	enum class donor_kind
	{
		lit,
		flag_wave,
		glass,
		frosted_glass,
		emissive,
		layered,
	};

	void dump(Material* asset);
	void dump_effect(Material* asset);
	void dump_sprite(Material* asset);
	std::string dump_decal(Material* asset);
	void dump_2d(Material* asset);
	std::string dump_lit(Material* asset, techset::generated_techset techset);
	std::string dump_sky(Material* asset, const GfxImage* sky_image, const std::array<float, 3>& rotation_angles);
	zonetool::iw8::GfxSModelSurfCullType get_smodel_cull_type(Material* asset);
	zonetool::iw8::GfxSModelSurfEmitType get_smodel_emit_type(Material* asset);
	unsigned int get_world_layer_count(Material* asset);
	bool has_world_layer_normal_transform(Material* asset, unsigned int layer);
	void dump_donor_techsets();
}
