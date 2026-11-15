#include <std_include.hpp>
#include "impactfx.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <std::size_t Count>
		void read_effects(zone_reader& reader, FxCombinedDef (&effects)[Count])
		{
			for (auto& effect : effects)
			{
				reader.read_asset(ASSET_TYPE_VFX, effect.particleSystemDef);
			}
		}

		void read_packs(zone_reader& reader, FxImpactPack*& field)
		{
			if (const auto packs = reader.read_array(field, 7, 2))
			{
				for (auto i = 0; i < 2; i++)
				{
					read_effects(reader, packs[i].nonflesh);
					read_effects(reader, packs[i].flesh);
				}
			}
		}
	}

	void impact_fx::read(zone_reader& reader, FxImpactTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->table, 7, asset->impactTypeCount))
		{
			for (auto i = 0u; i < asset->impactTypeCount; i++)
			{
				read_packs(reader, entries[i].normal);
				read_packs(reader, entries[i].exit);
			}
		}

		reader.read_array(asset->genericToRegistered, 3, asset->impactTypeCount);
		reader.read_array(asset->registeredToGeneric, 3, 33);
		reader.pop_stream();
	}

	void surface_fx::read(zone_reader& reader, SurfaceFxTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->table, 7, asset->numTableEntry))
		{
			for (auto i = 0; i < asset->numTableEntry; i++)
			{
				reader.read_asset(ASSET_TYPE_VFX, entries[i].baseEffect.particleSystemDef);
				read_effects(reader, entries[i].surfaceEffect);
			}
		}

		reader.push_stream(XFILE_BLOCK_RUNTIME);
		reader.read_array(asset->mapPoolBuffer, 3, asset->mapPoolBufferSize);
		reader.pop_stream();
		reader.pop_stream();
	}
}
