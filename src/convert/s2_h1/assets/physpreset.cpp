#include <std_include.hpp>

#include "../include.hpp"
#include "physpreset.hpp"

#include <h1/assets/physpreset.hpp>

namespace zonetool::s2::converter::h1::physpreset
{
	namespace
	{
		std::unordered_map<std::string, std::deque<std::pair<float, std::string>>> dumped_presets;

		const char* get_sound_prefix(const std::string_view name)
		{
			constexpr std::array<std::pair<std::string_view, const char*>, 5> prefixes
			{{
				{"metal", "physics_metal"},
				{"wood", "physics_wood"},
				{"glass", "physics_glass"},
				{"ceramic", "physics_glass"},
				{"cardboard", "physics_cardboard"},
			}};

			for (const auto& [part, prefix] : prefixes)
			{
				if (name.find(part) != std::string_view::npos)
				{
					return prefix;
				}
			}

			return "";
		}

		void dump_preset(const PhysPreset* asset, const std::string& name, const float mass)
		{
			zonetool::h1::PhysPreset preset{};
			preset.name = name.data();
			preset.mass = mass;
			preset.bounce = asset->bounce;
			preset.friction = asset->friction;
			preset.bulletForceScale = asset->bulletForceScale;
			preset.explosiveForceScale = asset->explosiveForceScale;
			preset.contents = 0x280E491;
			preset.sndAliasPrefix = get_sound_prefix(asset->name);
			preset.minMomentum = 250.0f;
			preset.maxMomentum = 250.0f;
			preset.maxVolume = 1.0f;
			preset.minPitch = 1.0f;
			preset.maxPitch = 1.0f;

			zonetool::h1::phys_preset::dump(&preset);
			add_csv_row("physpreset", name);
		}
	}

	const char* reference(const PhysPreset* asset, const float volume)
	{
		if (!asset || !asset->name)
		{
			return nullptr;
		}

		if (asset->name == "default"sv)
		{
			if (dumped_presets.try_emplace(asset->name).second)
			{
				add_csv_row("physpreset", asset->name, true);
			}

			return asset->name;
		}

		const auto mass = volume > 0.0f ? asset->density * volume : 10.0f;
		auto& variants = dumped_presets[asset->name];
		const auto variant = std::ranges::find_if(variants, [&](const std::pair<float, std::string>& entry)
		{
			return mass <= entry.first * 1.25f && entry.first <= mass * 1.25f;
		});

		if (variant != variants.end())
		{
			return variant->second.data();
		}

		auto name = variants.empty() ? std::string(asset->name) : std::string(asset->name) + "_v" + std::to_string(variants.size());
		dump_preset(asset, name, mass);
		return variants.emplace_back(mass, std::move(name)).second.data();
	}
}
