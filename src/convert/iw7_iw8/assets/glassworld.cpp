#include <std_include.hpp>

#include "../include.hpp"
#include "glassworld.hpp"

#include <iw8/assets/glassworld.hpp>

namespace zonetool::iw7::converter::iw8::glassworld
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::SpatialPartition_Tree* make_spatial_tree(const unsigned int piece_count, utils::memory::allocator& allocator)
		{
			auto* tree = allocator.allocate<target::SpatialPartition_Tree>();
			tree->alwaysListLength = piece_count;
			tree->alwaysList = allocator.allocate_array<unsigned int>(piece_count);
			for (auto i = 0u; i < piece_count; i++)
			{
				tree->alwaysList[i] = i;
			}

			return tree;
		}
	}

	void dump(GlassWorld* asset)
	{
		if (!asset->g_glassData)
		{
			return;
		}

		utils::memory::allocator allocator;

		const auto& data = *asset->g_glassData;
		auto* converted_data = allocator.allocate<target::G_GlassData>();
		converted_data->pieceCount = data.pieceCount;
		converted_data->glassPieces = reinterpret_cast<target::G_GlassPiece*>(data.glassPieces);
		converted_data->damageToWeaken = data.damageToWeaken;
		converted_data->damageToDestroy = data.damageToDestroy;
		converted_data->glassNameCount = data.glassNameCount;
		converted_data->glassNames = reinterpret_cast<target::G_GlassName*>(data.glassNames);
		converted_data->spatialTree = make_spatial_tree(data.pieceCount, allocator); // TODO: no tree nodes, every piece is in alwaysList

		target::GlassWorld converted{};
		converted.name = asset->name;
		converted.g_glassData = converted_data;

		target::glass_world::dump(&converted);
		add_csv_row("glass_map", asset->name);
	}
}
