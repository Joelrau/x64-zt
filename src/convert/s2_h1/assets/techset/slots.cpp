#include <std_include.hpp>

#include "../../include.hpp"
#include "slots.hpp"

namespace zonetool::s2::converter::h1::techset
{
	std::int32_t s2_base_slot(const std::int32_t h1_base)
	{
		switch (h1_base)
		{
		case zonetool::h1::TECHNIQUE_UNLIT:
			return TECHNIQUE_UNLIT;
		case zonetool::h1::TECHNIQUE_EMISSIVE:
			return TECHNIQUE_EMISSIVE;
		case zonetool::h1::TECHNIQUE_EMISSIVE_DFOG:
			return TECHNIQUE_EMISSIVE_ALFOG;
		case zonetool::h1::TECHNIQUE_EMISSIVE_SHADOW:
			return TECHNIQUE_EMISSIVE_SHADOW;
		case zonetool::h1::TECHNIQUE_EMISSIVE_SHADOW_DFOG:
			return TECHNIQUE_EMISSIVE_SHADOW_ALFOG;
		case zonetool::h1::TECHNIQUE_WIREFRAME_SOLID:
		case zonetool::h1::TECHNIQUE_WIREFRAME_SHADED:
			return TECHNIQUE_WIREFRAME_SOLID;
		case zonetool::h1::TECHNIQUE_VELOCITY_RIGID:
			return TECHNIQUE_VELOCITY_RIGID;
		case zonetool::h1::TECHNIQUE_VELOCITY_SKINNED:
			return TECHNIQUE_VELOCITY_SKINNED;
		default:
			return h1_base <= zonetool::h1::TECHNIQUE_BUILD_SHADOWMAP_COLOR ? h1_base : -1;
		}
	}

	std::int32_t s2_group(const std::int32_t h1_group)
	{
		switch (h1_group)
		{
		case zonetool::h1::TECHNIQUE_STANDARD_BEGIN:
			return TECHNIQUE_STANDARD_BEGIN;
		case zonetool::h1::TECHNIQUE_INSTANCED_BEGIN:
			return TECHNIQUE_INSTANCED_BEGIN;
		default:
			return TECHNIQUE_NO_TESS_BEGIN;
		}
	}

	bool is_light_pass(const std::int32_t h1_base)
	{
		return h1_base >= zonetool::h1::TECHNIQUE_LIGHT_SPOT && h1_base <= zonetool::h1::TECHNIQUE_LIGHT_OMNI_STENCIL_DFOG;
	}

	bool uses_lit_state(const std::int32_t h1_base)
	{
		return (h1_base >= zonetool::h1::TECHNIQUE_LIT && h1_base <= zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG)
			|| (h1_base >= zonetool::h1::TECHNIQUE_FAKELIGHT_NORMAL && h1_base <= zonetool::h1::TECHNIQUE_CASE_TEXTURE)
			|| h1_base == zonetool::h1::TECHNIQUE_THERMAL || h1_base == zonetool::h1::TECHNIQUE_CTQ || h1_base == zonetool::h1::TECHNIQUE_DEBUG_BUMPMAP;
	}

	std::int32_t find_lit_slot(const MaterialTechniqueSet* techset, const std::int32_t s2_group_begin)
	{
		for (const auto base : {TECHNIQUE_LIT, TECHNIQUE_LIT_GRID_2D, TECHNIQUE_LIT_GRID_3D})
		{
			if (techset->techniques[s2_group_begin + base])
			{
				return s2_group_begin + base;
			}
		}

		return -1;
	}

	std::int32_t find_source_slot(const MaterialTechniqueSet* techset, const std::int32_t h1_slot)
	{
		const auto h1_group = h1_slot / zonetool::h1::TECHNIQUE_INSTANCED_BEGIN * zonetool::h1::TECHNIQUE_INSTANCED_BEGIN;
		const auto h1_base = h1_slot - h1_group;
		if (h1_group == zonetool::h1::TECHNIQUE_SUBDIV_PATCH_BEGIN)
		{
			return -1;
		}

		switch (h1_base) // TODO: velocity techniques need the s2 previous frame vertex stream
		{
		case zonetool::h1::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
		case zonetool::h1::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED:
		case zonetool::h1::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
		case zonetool::h1::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED:
		case zonetool::h1::TECHNIQUE_VELOCITY_RIGID:
		case zonetool::h1::TECHNIQUE_VELOCITY_SKINNED:
			return -1;
		default:
			break;
		}

		const auto group = s2_group(h1_group);
		if (h1_base >= zonetool::h1::TECHNIQUE_LIT && h1_base <= zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG)
		{
			return find_lit_slot(techset, group);
		}

		const auto base = s2_base_slot(h1_base);
		return base >= 0 && techset->techniques[group + base] ? group + base : -1;
	}

	sun_mode_t sun_mode(const std::int32_t h1_slot)
	{
		switch (h1_slot % zonetool::h1::TECHNIQUE_INSTANCED_BEGIN)
		{
		case zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW:
		case zonetool::h1::TECHNIQUE_LIT_DIR_SHADOW_DFOG:
			return SUN_MODE_SHADOW;
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG:
			return SUN_MODE_SHADOW_LIGHT_ARRAY;
		case zonetool::h1::TECHNIQUE_LIT_DIR:
		case zonetool::h1::TECHNIQUE_LIT_DIR_DFOG:
			return SUN_MODE_LIT;
		default:
			return SUN_MODE_NONE;
		}
	}

	light_source_t light_source(const std::int32_t h1_slot)
	{
		switch (h1_slot % zonetool::h1::TECHNIQUE_INSTANCED_BEGIN)
		{
		case zonetool::h1::TECHNIQUE_LIT_SPOT:
		case zonetool::h1::TECHNIQUE_LIT_SPOT_DFOG:
			return LIGHT_SOURCE_SPOT;
		case zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW:
		case zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS: // TODO: cucoloris
		case zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_SPOT_SHADOW_CUCOLORIS_DFOG:
			return LIGHT_SOURCE_SPOT_SHADOW;
		case zonetool::h1::TECHNIQUE_LIT_OMNI:
		case zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW:
		case zonetool::h1::TECHNIQUE_LIT_OMNI_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_OMNI_SHADOW_DFOG:
			return LIGHT_SOURCE_OMNI;
		case zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS:
		case zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING:
		case zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_CUCOLORIS_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_CUCOLORIS_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_DYNAMIC_BRANCHING_DFOG:
		case zonetool::h1::TECHNIQUE_LIT_SUN_DYNAMIC_BRANCHING_DFOG:
			return LIGHT_SOURCE_ARRAY;
		default:
			return LIGHT_SOURCE_NONE;
		}
	}

	float sun_mode_w(const sun_mode_t mode)
	{
		switch (mode)
		{
		case SUN_MODE_LIT:
			return -1.0f;
		case SUN_MODE_SHADOW:
		case SUN_MODE_SHADOW_LIGHT_ARRAY:
			return 1.0f;
		default:
			return 0.0f;
		}
	}
}
