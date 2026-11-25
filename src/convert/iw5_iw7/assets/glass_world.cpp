#include <std_include.hpp>
#include "glass_world.hpp"

namespace zonetool::iw5::converter::iw7
{
	namespace glass_world
	{
		zonetool::iw7::GlassWorld* convert(zonetool::iw5::GlassWorld* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::iw7::GlassWorld>();
			new_asset->name = asset->name.get();
			new_asset->g_glassData = allocator.allocate<zonetool::iw7::G_GlassData>();

			const auto data = asset->g_glassData.get();
			if (!data)
			{
				return new_asset;
			}

			auto& new_data = *new_asset->g_glassData;
			new_data.pieceCount = data->pieceCount;
			new_data.damageToWeaken = data->damageToWeaken;
			new_data.damageToDestroy = data->damageToDestroy;
			new_data.glassPieces = allocator.allocate_array<zonetool::iw7::G_GlassPiece>(data->pieceCount);
			for (auto i = 0u; i < data->pieceCount; i++)
			{
				const auto& piece = data->glassPieces[i];
				auto& new_piece = new_data.glassPieces[i];
				new_piece.damageTaken = piece.damageTaken;
				new_piece.collapseTime = piece.collapseTime;
				new_piece.lastStateChangeTime = piece.lastStateChangeTime;
				new_piece.impactDir = piece.impactDir;
				new_piece.impactPos[0] = piece.impactPos[0];
				new_piece.impactPos[1] = piece.impactPos[1];
			}

			new_data.glassNameCount = data->glassNameCount;
			new_data.glassNames = allocator.allocate_array<zonetool::iw7::G_GlassName>(data->glassNameCount);
			for (auto i = 0u; i < data->glassNameCount; i++)
			{
				const auto& name = data->glassNames[i];
				auto& new_name = new_data.glassNames[i];
				new_name.nameStr = name.nameStr.get();
				new_name.pieceCount = name.pieceCount;
				new_name.pieceIndices = name.pieceIndices.get();
			}

			return new_asset;
		}

		void dump(zonetool::iw5::GlassWorld* asset)
		{
			utils::memory::allocator allocator;
			const auto converted_asset = convert(asset, allocator);
			zonetool::iw7::glass_world::dump(converted_asset);
		}
	}
}
