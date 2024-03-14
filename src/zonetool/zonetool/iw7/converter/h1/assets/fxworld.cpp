#include <std_include.hpp>

#include "zonetool/iw7/converter/h1/include.hpp"
#include "fxworld.hpp"
#include "zonetool/h1/assets/fxworld.hpp"

namespace zonetool::iw7
{
	namespace converter::h1
	{
		namespace fxworld
		{
#define COPY_VALUE_FXWORLD(name) \
		new_asset->name = asset->name; \

#define COPY_ARR_FXWORLD(name) \
		std::memcpy(new_asset->name, asset->name, sizeof(new_asset->name)); \

#define COPY_ARR_FXWORLD_(name, iw7_name) \
		std::memcpy(new_asset->name, asset->iw7_name, sizeof(new_asset->name)); \

			zonetool::h1::FxWorld* convert(FxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h1::FxWorld>();
				std::memcpy(new_asset, asset, sizeof(FxWorld));

				// mismatch up to here
				COPY_VALUE_FXWORLD(glassSys.initPieceCount);
				COPY_VALUE_FXWORLD(glassSys.cellCount);
				COPY_VALUE_FXWORLD(glassSys.activePieceCount);
				COPY_VALUE_FXWORLD(glassSys.firstFreePiece);
				COPY_VALUE_FXWORLD(glassSys.geoDataLimit);
				COPY_VALUE_FXWORLD(glassSys.geoDataCount);
				COPY_VALUE_FXWORLD(glassSys.initGeoDataCount);

				// FxGlassDef (defs)
				COPY_ARR_FXWORLD(glassSys.defs);
				auto fx_glass_def = new_asset->glassSys.defs;

				fx_glass_def->physPreset = reinterpret_cast<zonetool::h1::PhysPreset*>(asset->glassSys.defs->physicsAsset); // NOT a proper conversion
				COPY_ARR_FXWORLD_(glassSys.defs->pieceBreakEffect, glassSys.defs->pieceBreakEffect.u.fx);
				COPY_ARR_FXWORLD_(glassSys.defs->shatterEffect, glassSys.defs->shatterEffect.u.fx);
				COPY_ARR_FXWORLD_(glassSys.defs->shatterSmallEffect, glassSys.defs->shatterSmallEffect.u.fx);
				COPY_ARR_FXWORLD_(glassSys.defs->crackDecalEffect, glassSys.defs->crackDecalEffect.u.fx);

				COPY_ARR_FXWORLD(glassSys.defs->damagedSound);			// SndAliasLookup = const char*
				COPY_ARR_FXWORLD(glassSys.defs->destroyedSound);		// ^
				COPY_ARR_FXWORLD(glassSys.defs->destroyedQuietSound);	// ^

				COPY_VALUE_FXWORLD(glassSys.defs->numCrackRings);
				COPY_VALUE_FXWORLD(glassSys.defs->isOpaque);
				//

				COPY_ARR_FXWORLD(glassSys.piecePlaces);
				COPY_ARR_FXWORLD(glassSys.pieceStates);
				COPY_ARR_FXWORLD(glassSys.pieceDynamics);
				COPY_ARR_FXWORLD(glassSys.geoData);

				COPY_ARR_FXWORLD(glassSys.isInUse);
				COPY_ARR_FXWORLD(glassSys.cellBits);
				COPY_ARR_FXWORLD(glassSys.visData);
				COPY_ARR_FXWORLD(glassSys.linkOrg);
				COPY_ARR_FXWORLD(glassSys.halfThickness);
				COPY_ARR_FXWORLD(glassSys.lightingHandles);
				COPY_ARR_FXWORLD(glassSys.initGeoData);
				COPY_VALUE_FXWORLD(glassSys.needToCompactData);
				COPY_VALUE_FXWORLD(glassSys.initCount);
				COPY_VALUE_FXWORLD(glassSys.effectChanceAccum);
				COPY_VALUE_FXWORLD(glassSys.lastPieceDeletionTime);
				COPY_VALUE_FXWORLD(glassSys.initPieceCount);
				COPY_ARR_FXWORLD(glassSys.initPieceStates);

				return new_asset;
			}

			void dump(zonetool::iw7::FxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h1::fx_world::dump(converted_asset);
			}
		}
	}
}
