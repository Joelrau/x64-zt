#include <std_include.hpp>

#include "zonetool/h1/converter/iw7/include.hpp"
#include "fxworld.hpp"
#include "zonetool/iw7/assets/fxworld.hpp"

namespace zonetool::h1
{
	namespace converter::iw7
	{
		namespace fxworld
		{
#define COPY_SOUND(__name__) \
			if(asset->__name__) \
			{ \
				new_asset->__name__ = asset->__name__->name; \
			}

#define COPY_EFFECT(__name__) \
			new_asset->__name__.type = zonetool::iw7::FX_COMBINED_VFX; \
			if (asset->__name__) \
			{ \
				new_asset->__name__.u.vfx = allocator.allocate<zonetool::iw7::ParticleSystemDef>(); \
				new_asset->__name__.u.vfx->name = asset->__name__->name; \
			}

			zonetool::iw7::FxWorld* convert(FxWorld* asset, utils::memory::allocator& allocator)
			{
				auto* new_asset = allocator.allocate<zonetool::iw7::FxWorld>();

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

				new_asset->glassSys.defs = allocator.allocate_array<zonetool::iw7::FxGlassDef>(new_asset->glassSys.defCount);
				for (unsigned int i = 0; i < new_asset->glassSys.defCount; i++)
				{
					COPY_VALUE(glassSys.defs[i].halfThickness);
					COPY_ARR(glassSys.defs[i].texVecs);
					COPY_VALUE_CAST(glassSys.defs[i].color);
					COPY_ASSET(glassSys.defs[i].material);
					COPY_ASSET(glassSys.defs[i].materialShattered);

					if (asset->glassSys.defs[i].physPreset)
					{
						new_asset->glassSys.defs[i].physicsAsset = allocator.allocate<zonetool::iw7::PhysicsAsset>();
						new_asset->glassSys.defs[i].physicsAsset->name = asset->glassSys.defs[i].physPreset->name;
					}

					COPY_EFFECT(glassSys.defs[i].pieceBreakEffect);
					COPY_EFFECT(glassSys.defs[i].shatterEffect);
					COPY_EFFECT(glassSys.defs[i].shatterSmallEffect);

					COPY_SOUND(glassSys.defs[i].damagedSound);
					COPY_SOUND(glassSys.defs[i].destroyedSound);
					COPY_SOUND(glassSys.defs[i].destroyedQuietSound);

					new_asset->glassSys.defs[i].highMipRadiusInvSq = asset->glassSys.defs[i].invHighMipRadius; // recalc?
					new_asset->glassSys.defs[i].shatteredHighMipRadiusInvSq = asset->glassSys.defs[i].shatteredInvHighMipRadius; // recalc?
					COPY_VALUE(glassSys.defs[i].numCrackRings);
					COPY_VALUE(glassSys.defs[i].isOpaque);
				}

				REINTERPRET_CAST_SAFE(glassSys.piecePlaces);
				REINTERPRET_CAST_SAFE(glassSys.pieceStates);
				//REINTERPRET_CAST_SAFE(glassSys.pieceDynamics); // this is fine, runtime data allocated in parse func
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

#undef COPY_SOUND
#undef COPY_EFFECT

			void dump(FxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::iw7::fx_world::dump(converted_asset);
			}
		}
	}
}