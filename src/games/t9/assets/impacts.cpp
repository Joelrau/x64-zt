#include <std_include.hpp>
#include "impacts.hpp"

namespace zonetool::t9
{
	void impact_sounds_table::read(auto& reader, ImpactSoundsTable* asset)
	{
		reader.read_asset(ASSET_TYPE_SURFACESOUNDDEF, asset->surfaceSoundDef);
		for (auto& impacts : asset->entitySoundImpacts)
		{
			reader.read_asset(ASSET_TYPE_ENTITYSOUNDIMPACTS, impacts);
		}
	}

	void surface_fx_table::read(auto& reader, SurfaceFxTable* asset)
	{
		for (auto& effect : asset->effects)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}
	}

	void entity_fx_impacts::read(auto& reader, EntityFxImpacts* asset)
	{
		for (auto& group : asset->groups)
		{
			for (auto& effect : group.effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
		}
	}

	void fx_impact_table::read(auto& reader, FxImpactTable* asset)
	{
		reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, asset->surfaceFxTable);
		reader.read_asset(ASSET_TYPE_ENTITYFXIMPACTS, asset->entityFxImpacts);
	}
}

namespace zonetool::t9
{
	template void impact_sounds_table::read(zone_reader&, ImpactSoundsTable*);
	template void surface_fx_table::read(zone_reader&, SurfaceFxTable*);
	template void entity_fx_impacts::read(zone_reader&, EntityFxImpacts*);
	template void fx_impact_table::read(zone_reader&, FxImpactTable*);
	template void impact_sounds_table::read(zone_writer&, ImpactSoundsTable*);
	template void surface_fx_table::read(zone_writer&, SurfaceFxTable*);
	template void entity_fx_impacts::read(zone_writer&, EntityFxImpacts*);
	template void fx_impact_table::read(zone_writer&, FxImpactTable*);
}
