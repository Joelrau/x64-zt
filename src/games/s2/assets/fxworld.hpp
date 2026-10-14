#pragma once

namespace zonetool::s2
{
	class fx_world
	{
	public:
		static void read(zone_reader& reader, FxWorld* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			read_glass_system(reader, &asset->glassSys);
			reader.pop_stream();
		}

	private:
		template <typename T>
		static void read_runtime(zone_reader& reader, T*& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}

		static void read_glass_system(zone_reader& reader, FxGlassSystem* glass)
		{
			if (const auto defs = reader.read_array(glass->defs, 7, glass->defCount))
			{
				for (auto i = 0u; i < glass->defCount; i++)
				{
					auto& def = defs[i];
					reader.read_asset(ASSET_TYPE_PHYSPRESET, def.physPreset);
					reader.read_asset(ASSET_TYPE_MATERIAL, def.material);
					reader.read_asset(ASSET_TYPE_MATERIAL, def.materialShattered);
					reader.read_asset(ASSET_TYPE_FX, def.pieceBreakEffect);
					reader.read_asset(ASSET_TYPE_FX, def.shatterEffect);
					reader.read_asset(ASSET_TYPE_FX, def.shatterSmallEffect);
					reader.read_asset(ASSET_TYPE_FX, def.crackDecalEffect);
					reader.read_name_reference(def.damagedSound);
					reader.read_name_reference(def.destroyedSound);
					reader.read_name_reference(def.destroyedQuietSound);
				}
			}

			const auto piece_limit = glass->pieceLimit;
			read_runtime(reader, glass->piecePlaces, 3, piece_limit);
			read_runtime(reader, glass->pieceStates, 3, piece_limit);
			read_runtime(reader, glass->pieceDynamics, 7, piece_limit);
			read_runtime(reader, glass->geoData, 1, glass->geoDataLimit);
			read_runtime(reader, glass->isInUse, 3, glass->pieceWordCount);
			read_runtime(reader, glass->cellBits, 3, glass->pieceWordCount * glass->cellCount);
			read_runtime(reader, glass->visData, 15, (piece_limit + 15) & ~15u);
			read_runtime(reader, glass->linkOrg, 3, piece_limit);
			read_runtime(reader, glass->halfThickness, 15, (piece_limit + 3) & ~3u);

			reader.read_array(glass->lightingHandles, 1, glass->initPieceCount);
			reader.read_array(glass->initGeoData, 1, glass->initGeoDataCount);
			reader.read_array(glass->initPieceStates, 3, glass->initPieceCount);
		}
	};
}
