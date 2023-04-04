#include <std_include.hpp>

#include "zonetool/h1/converter/h2/include.hpp"
#include "fxworld.hpp"
#include "zonetool/h2/assets/fxworld.hpp"

namespace zonetool::h1
{
	namespace converter::h2
	{
		namespace fxworld
		{
			zonetool::h2::FxWorld* convert(FxWorld* asset, utils::memory::allocator& allocator)
			{
				const auto new_asset = allocator.allocate<zonetool::h2::FxWorld>();
				std::memcpy(new_asset, asset, sizeof(FxWorld));

				new_asset->glassSys.piecePlaces = allocator.allocate_array<zonetool::h2::FxGlassPiecePlace>(asset->glassSys.pieceLimit);
				for (auto i = 0u; i < new_asset->glassSys.pieceLimit; i++)
				{
					std::memcpy(&new_asset->glassSys.piecePlaces[i].__s0.frame.quat, &asset->glassSys.piecePlaces[i].__s0.frame.quat,
						sizeof(new_asset->glassSys.piecePlaces[i].__s0.frame.quat));
					std::memcpy(&new_asset->glassSys.piecePlaces[i].__s0.frame.origin, &asset->glassSys.piecePlaces[i].__s0.frame.origin,
						sizeof(new_asset->glassSys.piecePlaces[i].__s0.frame.origin));

					new_asset->glassSys.piecePlaces[i].__s0.radius = asset->glassSys.piecePlaces[i].__s0.radius;
				}

				new_asset->glassSys.initPieceStates = allocator.allocate_array<zonetool::h2::FxGlassInitPieceState>(new_asset->glassSys.initPieceCount);
				for (auto i = 0u; i < new_asset->glassSys.initPieceCount; i++)
				{
					std::memcpy(&new_asset->glassSys.initPieceStates[i].frame.quat, &asset->glassSys.initPieceStates[i].frame.quat,
						sizeof(new_asset->glassSys.initPieceStates[i].frame.quat));

					std::memcpy(&new_asset->glassSys.initPieceStates[i].frame.origin, &asset->glassSys.initPieceStates[i].frame.origin,
						sizeof(new_asset->glassSys.initPieceStates[i].frame.origin));

					new_asset->glassSys.initPieceStates[i].radius = asset->glassSys.initPieceStates[i].radius;
					new_asset->glassSys.initPieceStates[i].texCoordOrigin[0] = asset->glassSys.initPieceStates[i].texCoordOrigin[0];
					new_asset->glassSys.initPieceStates[i].texCoordOrigin[1] = asset->glassSys.initPieceStates[i].texCoordOrigin[1];
					new_asset->glassSys.initPieceStates[i].supportMask = asset->glassSys.initPieceStates[i].supportMask;
					new_asset->glassSys.initPieceStates[i].areaX2 = asset->glassSys.initPieceStates[i].areaX2;
					new_asset->glassSys.initPieceStates[i].lightingIndex = asset->glassSys.initPieceStates[i].lightingIndex;
					new_asset->glassSys.initPieceStates[i].defIndex = asset->glassSys.initPieceStates[i].defIndex;
					new_asset->glassSys.initPieceStates[i].vertCount = asset->glassSys.initPieceStates[i].vertCount;
					new_asset->glassSys.initPieceStates[i].fanDataCount = asset->glassSys.initPieceStates[i].fanDataCount;
					*new_asset->glassSys.initPieceStates[i].pad = *asset->glassSys.initPieceStates[i].pad;
				}

				return new_asset;
			}

			void dump(zonetool::h1::FxWorld* asset)
			{
				utils::memory::allocator allocator;
				const auto converted_asset = convert(asset, allocator);
				zonetool::h2::fx_world::dump(converted_asset);
			}
		}
	}
}
