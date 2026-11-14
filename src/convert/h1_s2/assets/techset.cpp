#include <std_include.hpp>

#include "../include.hpp"
#include "techset.hpp"
#include "techset/common.hpp"
#include "techset/instancing.hpp"
#include "techset/light_passes.hpp"
#include "techset/passes.hpp"
#include "techset/slots.hpp"
#include "techset/velocity.hpp"

namespace zonetool::h1::converter::s2::techset
{
	utils::memory::allocator native_allocator;

	namespace
	{
		light_grid slot_light_grid(const std::int32_t slot)
		{
			const auto type = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			if (type >= zonetool::s2::TECHNIQUE_LIT_BEGIN + 8 && type < zonetool::s2::TECHNIQUE_LIT_END)
			{
				return light_grid::volume_3d;
			}

			if (type >= zonetool::s2::TECHNIQUE_LIT_BEGIN + 4 && type < zonetool::s2::TECHNIQUE_LIT_END)
			{
				return light_grid::array_2d;
			}

			return light_grid::probe_buffer;
		}

		std::map<std::pair<std::string, bool>, std::optional<native_techset>> native_techsets;

		bool needs_prepass_technique(const std::string_view name)
		{
			if (name.contains("_ndw") || name.contains("_cltrans") || name.starts_with("effect") || name.starts_with("particle"))
			{
				return false;
			}

			return name.contains("_atest") || name.contains("_flag") || name.contains("_t0c0") || name.contains("_a0c0");
		}

		bool needs_instanced_techniques(const std::string_view name)
		{
			return name.contains("_reactivemotion") || name.contains("_ambient_") || name.starts_with("ms_") || name.starts_with("msc_");
		}

		void convert_missing_prepass(const MaterialTechniqueSet* techset, const std::int32_t slot, const std::int32_t group, native_techset& result)
		{
			const auto name = std::string_view(techset->name);
			const auto source_type = name.contains("_unlit_") || name.contains("_ambient_") ? TECHNIQUE_LIT : TECHNIQUE_BUILD_SHADOWMAP_DEPTH;
			const auto source = find_group_technique(techset, group, source_type);
			if (source >= 0)
			{
				result.techset->techniques[slot] = convert_technique(techset->techniques[source], slot_light_grid(slot), slot, false);
				result.sources[slot] = source;
				result.state_sources[slot] = find_group_technique(techset, group, TECHNIQUE_LIT);
			}
		}

		void convert_slot(const MaterialTechniqueSet* techset, const std::int32_t slot, const bool sprite, native_techset& result)
		{
			if (!sprite && use_stock_velocity_technique(techset, slot, result))
			{
				return;
			}

			const auto group = slot - slot % zonetool::s2::TECHNIQUE_GROUP_SIZE;
			std::string error;
			for (const auto type : slot_candidates(slot % zonetool::s2::TECHNIQUE_GROUP_SIZE))
			{
				const auto source = find_group_technique(techset, group, type);
				if (source < 0)
				{
					continue;
				}

				try
				{
					result.techset->techniques[slot] = convert_technique(techset->techniques[source], slot_light_grid(slot), slot, sprite);
					result.sources[slot] = source;
					if (!sprite)
					{
						add_light_pass(techset, slot, result);
					}

					return;
				}
				catch (const std::exception& e)
				{
					error = utils::string::va("technique \"%s\": %s", techset->techniques[source]->hdr.name, e.what());
				}
			}

			const auto is_wireframe = slot % zonetool::s2::TECHNIQUE_GROUP_SIZE == zonetool::s2::TECHNIQUE_WIREFRAME_SOLID;
			if (!error.empty() && !is_wireframe)
			{
				throw std::runtime_error(error);
			}

			if (slot % zonetool::s2::TECHNIQUE_GROUP_SIZE == zonetool::s2::TECHNIQUE_ZPREPASS && !sprite && needs_prepass_technique(techset->name))
			{
				convert_missing_prepass(techset, slot, group, result);
			}
		}

		native_techset build_native(const std::string& name, const bool sprite)
		{
			const auto* techset = zonetool::h1::db_find_x_asset_header_safe(ASSET_TYPE_TECHNIQUE_SET, name).techniqueSet;
			if (!techset)
			{
				throw std::runtime_error("the h1 techset is not loaded");
			}

			if (needs_instanced_techniques(name))
			{
				techset = add_instanced_techniques(techset);
			}

			if (techset->worldVertFormat >= zonetool::s2::MTL_WORLDVERT_COUNT)
			{
				throw std::runtime_error(utils::string::va("world vertex format %u has no s2 mapping", techset->worldVertFormat));
			}

			native_techset result{native_allocator.allocate<zonetool::s2::MaterialTechniqueSet>()};
			result.sources.fill(-1);
			result.light_sources.fill(-1);
			result.state_sources.fill(-1);
			result.techset->name = native_allocator.duplicate_string(::game::add_source_postfix(sprite ? name + "_sprite" : name, ::game::h1));
			result.techset->flags = techset->flags;
			result.techset->worldVertFormat = techset->worldVertFormat;
			result.techset->preDisplacementOnlyCount = techset->preDisplacementOnlyCount;

			const auto slot_count = sprite ? zonetool::s2::TECHNIQUE_GROUP_SIZE : zonetool::s2::technique_count;
			for (auto slot = 0; slot < slot_count; slot++)
			{
				convert_slot(techset, slot, sprite, result);
			}

			const auto empty = std::ranges::all_of(result.sources, [](const std::int32_t source)
			{
				return source < 0;
			});

			if (empty)
			{
				throw std::runtime_error("no technique has an s2 slot");
			}

			return result;
		}
	}

	const native_techset* convert_native(const std::string& h1_techset, const bool sprite)
	{
		const auto key = std::make_pair(h1_techset, sprite);
		if (const auto entry = native_techsets.find(key); entry != native_techsets.end())
		{
			return entry->second ? &*entry->second : nullptr;
		}

		auto& entry = native_techsets[key];
		try
		{
			entry = build_native(h1_techset, sprite);
			ZONETOOL_INFO("techset %s -> %s (native)", h1_techset.data(), entry->techset->name);
		}
		catch (const std::exception& e)
		{
			ZONETOOL_WARNING("techset %s is not converted: %s", h1_techset.data(), e.what());
		}

		return entry ? &*entry : nullptr;
	}
}
