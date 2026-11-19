#pragma once

#include "techset.hpp"

namespace zonetool::iw7::converter::iw8::material
{
	void dump(Material* asset);
	void dump_effect(Material* asset);
	std::string dump_lit(Material* asset, techset::generated_techset techset);
	std::string dump_sky(Material* asset, const GfxImage* sky_image);
	zonetool::iw8::GfxSModelSurfCullType get_smodel_cull_type(Material* asset);
}
