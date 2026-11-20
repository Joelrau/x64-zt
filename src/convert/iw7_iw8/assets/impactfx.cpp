#include <std_include.hpp>

#include "../include.hpp"
#include "impactfx.hpp"
#include "vfx.hpp"

#include <iw8/assets/impactfx.hpp>

namespace zonetool::iw7::converter::iw8::impactfx
{
	namespace
	{
		namespace target = zonetool::iw8;

		std::optional<unsigned int> source_entry(const unsigned int type) // sub_1408E98D0, the exit entry is the next one
		{
			switch (type)
			{
			case target::IMPACT_TYPE_BULLET_SMALL: return 0;
			case target::IMPACT_TYPE_BULLET_MEDIUM:
			case target::IMPACT_TYPE_BULLET_LARGE:
			case target::IMPACT_TYPE_BULLET_AP: return 2;
			case target::IMPACT_TYPE_SHOTGUN: return 5;
			default: return std::nullopt;
			}
		}

		bool is_vfx(const FxCombinedDef& effect)
		{
			return effect.type == FX_COMBINED_VFX && effect.u.vfx;
		}

		bool has_effects(const FxImpactEntry& entry)
		{
			return std::ranges::any_of(entry.nonflesh, is_vfx) || std::ranges::any_of(entry.flesh, is_vfx);
		}

		template <std::size_t Count>
		void convert_effects(const FxCombinedDef (&source)[Count], target::FxCombinedDef (&converted)[Count], utils::memory::allocator& allocator)
		{
			for (auto i = 0u; i < Count; i++)
			{
				if (is_vfx(source[i]))
				{
					auto* effect = allocator.allocate<target::ParticleSystemDef>();
					effect->name = source[i].u.vfx->name;
					converted[i].particleSystemDef = effect;
					vfx::dump(source[i].u.vfx);
				}
			}
		}

		target::FxImpactPack* convert_packs(const FxImpactEntry* entry, utils::memory::allocator& allocator)
		{
			auto* packs = allocator.allocate_array<target::FxImpactPack>(2);
			if (entry)
			{
				convert_effects(entry->nonflesh, packs[0].nonflesh, allocator);
				convert_effects(entry->flesh, packs[0].flesh, allocator);
			}

			return packs;
		}

		target::FxImpactTable* convert(const FxImpactTable* asset, utils::memory::allocator& allocator)
		{
			auto* converted = allocator.allocate<target::FxImpactTable>();
			converted->name = asset->name;
			converted->impactTypeCount = target::IMPACT_TYPE_COUNT;
			converted->table = allocator.allocate_array<target::FxImpactEntry>(target::IMPACT_TYPE_COUNT);
			converted->genericToRegistered = allocator.allocate_array<unsigned int>(target::IMPACT_TYPE_COUNT);
			converted->registeredToGeneric = allocator.allocate_array<int>(target::IMPACT_TYPE_REGISTERED_COUNT);

			for (auto type = 0u; type < target::IMPACT_TYPE_COUNT; type++)
			{
				const auto entry = source_entry(type);
				converted->table[type].normal = convert_packs(entry ? &asset->table[*entry] : nullptr, allocator);
				if (entry && has_effects(asset->table[*entry + 1]))
				{
					converted->table[type].exit = convert_packs(&asset->table[*entry + 1], allocator);
				}

				converted->genericToRegistered[type] = type < target::IMPACT_TYPE_REGISTERED_COUNT ? type : 0xFFFFFFFF;
			}

			for (auto type = 0; type < target::IMPACT_TYPE_REGISTERED_COUNT; type++)
			{
				converted->registeredToGeneric[type] = type;
			}

			return converted;
		}
	}

	void dump(FxImpactTable* asset)
	{
		if (!asset->table || !mark_dumped(ASSET_TYPE_IMPACT_FX, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		target::impact_fx::dump(convert(asset, allocator));
		add_csv_row("impactfx", asset->name);
	}
}
