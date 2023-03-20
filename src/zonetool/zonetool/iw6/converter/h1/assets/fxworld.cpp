#include <std_include.hpp>
#include "zonetool/iw6/converter/include.hpp"
#include "zonetool/iw6/converter/h1/include.hpp"
#include "fxworld.hpp"

#include "zonetool/h1/assets/fxworld.hpp"

namespace zonetool::iw6
{
	namespace converter::h1
	{
		namespace fxworld
		{
			zonetool::h1::FxWorld* convert(FxWorld* asset, ZoneMemory* mem)
			{
				auto* new_asset = mem->Alloc<zonetool::h1::FxWorld>();

				REINTERPRET_CAST_SAFE(name);

				COPY_VALUE(glassSys.time);
				COPY_VALUE(glassSys.prevTime);
				COPY_VALUE(glassSys.defCount);
				COPY_VALUE(glassSys.pieceLimit);
				COPY_VALUE(glassSys.pieceWordCount);
				COPY_VALUE(glassSys.cellCount);
				COPY_VALUE(glassSys.activePieceCount);
				COPY_VALUE(glassSys.firstFreePiece);
				COPY_VALUE(glassSys.geoDataLimit);
				COPY_VALUE(glassSys.geoDataCount);
				COPY_VALUE(glassSys.initGeoDataCount);
				COPY_VALUE(glassSys.needToCompactData);
				COPY_VALUE(glassSys.initCount);
				COPY_VALUE(glassSys.effectChanceAccum);
				COPY_VALUE(glassSys.lastPieceDeletionTime);
				COPY_VALUE(glassSys.initPieceCount);

				REINTERPRET_CAST_SAFE(glassSys.defs);
				REINTERPRET_CAST_SAFE(glassSys.piecePlaces);
				REINTERPRET_CAST_SAFE(glassSys.pieceStates);
				REINTERPRET_CAST_SAFE(glassSys.pieceDynamics);
				REINTERPRET_CAST_SAFE(glassSys.geoData);
				REINTERPRET_CAST_SAFE(glassSys.isInUse);
				REINTERPRET_CAST_SAFE(glassSys.cellBits);
				REINTERPRET_CAST_SAFE(glassSys.visData);
				REINTERPRET_CAST_SAFE(glassSys.linkOrg);
				REINTERPRET_CAST_SAFE(glassSys.halfThickness);
				REINTERPRET_CAST_SAFE(glassSys.lightingHandles);
				REINTERPRET_CAST_SAFE(glassSys.initPieceStates);
				REINTERPRET_CAST_SAFE(glassSys.initGeoData);

				return new_asset;
			}

			void dump(FxWorld* asset, ZoneMemory* mem)
			{
				auto* converted_asset = convert(asset, mem);
				zonetool::h1::IFxWorld::dump(converted_asset);
			}
		}
	}
}