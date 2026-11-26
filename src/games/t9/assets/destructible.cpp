#include <std_include.hpp>
#include "destructible.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_stage(auto& reader, DestructibleStage& stage)
		{
			reader.read_asset(ASSET_TYPE_FX, stage.effect);
			reader.read_asset(ASSET_TYPE_PHYSPRESET, stage.physPreset);
			reader.read_array(stage.records, 7, stage.recordCount);
		}
	}

	void destructible_def::read(auto& reader, DestructibleDef* asset)
	{
		for (auto& model : asset->models)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}

		const auto pieces = reader.read_array(asset->pieces, 7, asset->pieceCount);
		if (!pieces)
		{
			return;
		}

		for (auto i = 0; i < asset->pieceCount; i++)
		{
			for (auto& stage : pieces[i].stages)
			{
				read_stage(reader, stage);
			}

			reader.read_asset(ASSET_TYPE_PHYSCONSTRAINTS, pieces[i].physConstraints);
			reader.read_asset(ASSET_TYPE_FX, pieces[i].effect);
		}
	}

	void zbarrier::read(auto& reader, ZBarrier* asset)
	{
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model);
		for (auto& board : asset->boards)
		{
			for (auto& model : board.models)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, model);
			}

			for (auto& effect : board.effects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
		}
	}
}

namespace zonetool::t9
{
	template void destructible_def::read(zone_reader&, DestructibleDef*);
	template void zbarrier::read(zone_reader&, ZBarrier*);
	template void destructible_def::read(zone_writer&, DestructibleDef*);
	template void zbarrier::read(zone_writer&, ZBarrier*);
}
