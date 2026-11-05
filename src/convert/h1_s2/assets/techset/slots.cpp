#include <std_include.hpp>

#include "../../include.hpp"
#include "slots.hpp"

namespace zonetool::h1::converter::s2::techset
{
	namespace
	{
		bool has_hull_shader(const MaterialTechnique* technique)
		{
			return std::any_of(technique->passArray, technique->passArray + technique->hdr.passCount, [](const MaterialPass& pass)
			{
				return pass.hullShader != nullptr;
			});
		}
	}

	std::vector<std::int32_t> slot_candidates(const std::int32_t slot)
	{
		if (slot >= zonetool::s2::TECHNIQUE_LIT_BEGIN && slot < zonetool::s2::TECHNIQUE_LIT_END)
		{
			if ((slot - zonetool::s2::TECHNIQUE_LIT_BEGIN) % 2)
			{
				return {TECHNIQUE_LIT_DIR_SHADOW_DFOG, TECHNIQUE_LIT_DIR_DFOG, TECHNIQUE_LIT_DFOG};
			}

			return {TECHNIQUE_LIT_DIR_SHADOW, TECHNIQUE_LIT_DIR, TECHNIQUE_LIT};
		}

		if (slot >= zonetool::s2::TECHNIQUE_EMISSIVE && slot <= zonetool::s2::TECHNIQUE_EMISSIVE_SHADOW_DFOG)
		{
			return {TECHNIQUE_EMISSIVE + slot - zonetool::s2::TECHNIQUE_EMISSIVE};
		}

		switch (slot)
		{
		case zonetool::s2::TECHNIQUE_ZPREPASS:
			return {TECHNIQUE_ZPREPASS};
		case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_RIGID:
			return {TECHNIQUE_ZPREPASS_VELOCITY_RIGID, TECHNIQUE_ZPREPASS};
		case zonetool::s2::TECHNIQUE_ZPREPASS_VELOCITY_SKINNED:
			return {TECHNIQUE_ZPREPASS_VELOCITY_SKINNED, TECHNIQUE_ZPREPASS};
		case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR:
			return {TECHNIQUE_ZPREPASS_HIDIR};
		case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID:
			return {TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_RIGID, TECHNIQUE_ZPREPASS_HIDIR};
		case zonetool::s2::TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED:
			return {TECHNIQUE_ZPREPASS_HIDIR_VELOCITY_SKINNED, TECHNIQUE_ZPREPASS_HIDIR};
		case zonetool::s2::TECHNIQUE_BUILD_SHADOWMAP_DEPTH:
			return {TECHNIQUE_BUILD_SHADOWMAP_DEPTH};
		case zonetool::s2::TECHNIQUE_UNLIT:
			return {TECHNIQUE_UNLIT};
		case zonetool::s2::TECHNIQUE_WIREFRAME_SOLID:
			return {TECHNIQUE_WIREFRAME_SOLID};
		default:
			return {};
		}
	}

	std::int32_t find_group_technique(const MaterialTechniqueSet* techset, const std::int32_t group, const std::int32_t type)
	{
		const auto exists = [&](const std::int32_t index)
		{
			return techset->techniques[index] != nullptr;
		};

		const auto standard = TECHNIQUE_STANDARD_BEGIN + type;
		const auto instanced = TECHNIQUE_INSTANCED_BEGIN + type;
		const auto no_displacement = TECHNIQUE_NO_DISPLACEMENT_BEGIN + type;
		const auto tessellated = exists(standard) && has_hull_shader(techset->techniques[standard]);

		if (group == zonetool::s2::TECHNIQUE_INSTANCED_BEGIN)
		{
			return exists(instanced) ? instanced : -1;
		}

		if (group == zonetool::s2::TECHNIQUE_NO_TESS_BEGIN)
		{
			if (exists(no_displacement))
			{
				return no_displacement;
			}

			return exists(standard) && !tessellated ? standard : -1;
		}

		if (tessellated && exists(no_displacement))
		{
			return no_displacement;
		}

		return exists(standard) ? standard : -1;
	}
}
