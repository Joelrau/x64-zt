#include <std_include.hpp>
#include "fxworld.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_glass_globals(zone_reader& reader, FxGlassGlobals& globals)
		{
			if (const auto defs = reader.read_array(globals.defs, 7, globals.defCount))
			{
				for (auto i = 0u; i < globals.defCount; i++)
				{
					auto& def = defs[i];
					reader.read_asset(ASSET_TYPE_PHYSICSASSET, def.physicsAsset);
					reader.read_asset(ASSET_TYPE_MATERIAL, def.material);
					reader.read_asset(ASSET_TYPE_MATERIAL, def.materialShattered);
					reader.read_asset(ASSET_TYPE_VFX, def.pieceBreakEffect.particleSystemDef);
					reader.read_asset(ASSET_TYPE_VFX, def.shatterEffect.particleSystemDef);
					reader.read_asset(ASSET_TYPE_VFX, def.shatterSmallEffect.particleSystemDef);
					reader.read_asset(ASSET_TYPE_VFX, def.crackDecalEffect.particleSystemDef);
					reader.read_string(def.damagedSound.name);
					reader.read_string(def.destroyedSound.name);
					reader.read_string(def.destroyedQuietSound.name);
				}
			}

			reader.read_array(globals.glassTransientLookup, 1, globals.glassTransientLookupCount);

			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(globals.distances, 3, globals.defCount);
			reader.pop_stream();
		}

		void read_glass_system(zone_reader& reader, FxGlassSystem& system)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(system.piecePlaces, 3, system.pieceLimit);
			reader.read_array(system.pieceStates, 3, system.pieceLimit);
			reader.read_array(system.pieceDynamics, 3, system.pieceLimit);
			reader.read_array(system.geoData, 1, system.geoDataLimit);
			reader.read_array(system.isInUse, 3, system.pieceWordCount);
			reader.read_array(system.cellBits, 3, system.pieceWordCount * system.cellCount);
			reader.read_array(system.visData, 15, (system.pieceLimit + 15) & ~15u);
			reader.read_array(system.linkOrg, 3, system.pieceLimit);
			reader.read_array(system.halfThickness, 15, (system.pieceLimit + 3) & ~3u);
			reader.pop_stream();

			reader.read_array(system.initGeoData, 1, system.initGeoDataCount);
			reader.read_array(system.initPieceStates, 3, system.initPieceCount);
		}
	}

	void fx_world::read(zone_reader& reader, FxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_glass_globals(reader, asset->glassGlob);
		read_glass_system(reader, asset->glassSys);

		for (auto& zone : asset->transientZones)
		{
			reader.read_asset(ASSET_TYPE_FXWORLD_TRANSIENT_ZONE, zone);
		}
		reader.pop_stream();
	}

	void fx_world_transient_zone::read(zone_reader& reader, FxWorldTransientZone* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_glass_system(reader, asset->glassSys);
		reader.pop_stream();
	}
}
