#include <std_include.hpp>

#include "../include.hpp"
#include "physpreset.hpp"

namespace zonetool::h1::converter::s2::physpreset
{
	namespace
	{
		const std::unordered_set<std::string> s2_global_phys_presets
		{
			"default", "phys_brick", "phys_cloth", "phys_flesh", "phys_fruit", "phys_glass", "phys_metal_chain",
			"phys_metal_hollow", "phys_metal_hollow_sml", "phys_metal_solid", "phys_metal_thin", "phys_metal_vehicle",
			"phys_metal_weapon", "phys_plastic_hollow", "phys_plastic_solid", "phys_wood_debris", "phys_wood_solid",
			"phys_wood_solid_donotcollide",
		};

		constexpr auto default_density = 0.0361f;
		constexpr float default_unk_0x20[2]{20.0f, 300.0f};
		constexpr float default_unk_0x70[4]{176.0f, 1728.0f, 1728000.0f, 13824000.0f};

		std::unordered_set<std::string> dumped_presets;

		float shape_volume(const XModel* model)
		{
			if (!model)
			{
				return 0.0f;
			}

			if (const auto* collmap = model->physCollmap)
			{
				auto volume = 0.0f;
				for (auto i = 0u; collmap->geoms && i < collmap->count; i++)
				{
					volume += collmap->geoms[i].data ? collmap->geoms[i].data->m_volume : 0.0f;
				}

				return volume;
			}

			const auto& half_size = model->bounds.halfSize;
			return 8.0f * half_size[0] * half_size[1] * half_size[2];
		}

		void dump_preset(const PhysPreset* preset, const XModel* model)
		{
			utils::memory::allocator allocator;
			const auto new_preset = allocator.allocate<zonetool::s2::PhysPreset>();
			new_preset->name = preset->name;

			const auto volume = shape_volume(model); // s2 takes the mass from density * shape volume
			new_preset->density = volume > 0.0f && preset->mass > 0.0f ? preset->mass / volume : default_density;
			new_preset->bounce = preset->bounce;
			new_preset->friction = preset->friction;
			new_preset->unk_0x14 = 1.0f;
			new_preset->bulletForceScale = preset->bulletForceScale;
			new_preset->explosiveForceScale = preset->explosiveForceScale;
			std::ranges::copy(default_unk_0x20, new_preset->unk_0x20);
			new_preset->collisionMask = 0xFFFF;
			new_preset->unk_0x2B = 1;
			std::ranges::copy(default_unk_0x70, new_preset->unk_0x70);

			zonetool::s2::phys_preset::dump(new_preset);
			add_csv_row("physpreset", preset->name);
		}
	}

	zonetool::s2::PhysPreset* reference(const PhysPreset* preset, const XModel* model, utils::memory::allocator& allocator)
	{
		if (!preset || !preset->name || s2_global_phys_presets.contains(preset->name))
		{
			const auto name = preset && preset->name && s2_global_phys_presets.contains(preset->name) ? preset->name : "default";
			add_csv_row("physpreset", name, true);
			return make_stub<zonetool::s2::PhysPreset>(name, allocator);
		}

		// the preset can be a name only stub of another zone
		if (dumped_presets.insert(preset->name).second)
		{
			const auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_PHYSPRESET, preset->name).physPreset;
			dump_preset(loaded ? loaded : preset, model);
		}

		return make_stub<zonetool::s2::PhysPreset>(preset->name, allocator);
	}

	void clear()
	{
		dumped_presets.clear();
	}
}
