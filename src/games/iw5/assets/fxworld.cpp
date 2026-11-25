#include <std_include.hpp>
#include "fxworld.hpp"

namespace zonetool::iw5
{
	namespace
	{
		template <typename T>
		void read_runtime(zone_reader32& reader, x86::pointer<T>& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_RUNTIME);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}
	}

	void fx_world::read(zone_reader32& reader, FxWorld* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		auto& glass = asset->glassSys;
		if (const auto defs = reader.read_array(glass.defs, 3, glass.defCount))
		{
			for (auto i = 0u; i < glass.defCount; i++)
			{
				reader.read_asset(ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
				reader.read_asset(ASSET_TYPE_MATERIAL, defs[i].material);
				reader.read_asset(ASSET_TYPE_MATERIAL, defs[i].materialShattered);
			}
		}

		read_runtime(reader, glass.piecePlaces, 3, glass.pieceLimit);
		read_runtime(reader, glass.pieceStates, 3, glass.pieceLimit);
		read_runtime(reader, glass.pieceDynamics, 3, glass.pieceLimit);
		read_runtime(reader, glass.geoData, 3, glass.geoDataLimit);
		read_runtime(reader, glass.isInUse, 3, glass.pieceWordCount);
		read_runtime(reader, glass.cellBits, 3, glass.pieceWordCount * glass.cellCount);
		read_runtime(reader, glass.visData, 15, (glass.pieceLimit + 15) / 16 * 16);
		read_runtime(reader, glass.linkOrg, 3, glass.pieceLimit);
		read_runtime(reader, glass.halfThickness, 15, (glass.pieceLimit + 3) / 4 * 4);

		reader.read_array(glass.lightingHandles, 1, glass.initPieceCount);
		reader.read_array(glass.initPieceStates, 3, glass.initPieceCount);
		reader.read_array(glass.initGeoData, 3, glass.initGeoDataCount);
		reader.pop_stream();
	}
}
