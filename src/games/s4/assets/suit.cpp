#include <std_include.hpp>
#include "suit.hpp"

namespace zonetool::s4
{
	void suit_def::read(zone_reader& reader, SuitDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_CAMERA, asset->cam_helmetCam);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackage);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageL);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageR);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageRelaxed);
		reader.read_asset(ASSET_TYPE_SUITANIMPACKAGE, asset->animPackageSafe);
		reader.read_asset(ASSET_TYPE_AI_ASM, asset->asmAsset);
		reader.read_asset(ASSET_TYPE_AI_ANIM_SET, asset->animsetAsset);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptableDef);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->groundPound_activationRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->groundPound_landingRumble);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleLowHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleMediumHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleHighHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->landing_rumbleExtremeHeight);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->footstep_rumble);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_slide);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_creep_left);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_creep_right);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_walk_left);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_walk_right);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_run_left);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_run_right);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_sprint_left);
		reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, asset->footstepvfx_sprint_right);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->skydive_parachuteViewModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->skydive_parachuteWorldModel);
		reader.read_asset(ASSET_TYPE_CINEMATIC_MOTION, asset->skydive_freefallCinematicMotion);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallLookHorizForceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallStickHorizForceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallLookHorizDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallStickHorizDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallLookVertDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallStickVertDragIncreaseGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallStickVertDragReductionGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallTurnMultiplierGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_freefallTurnMultiplierPitchGraph);
		reader.read_asset(ASSET_TYPE_CINEMATIC_MOTION, asset->skydive_canopyCinematicMotion);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyLookHorizForceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyStickHorizForceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyLookHorizDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyStickHorizDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyLookVertDragGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyStickVertDragIncreaseGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyStickVertDragReductionGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyTurnMultiplierGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_canopyTurnMultiplierPitchGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_wmAnimFreefallLeftStickXToRollGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_wmAnimFreefallRightStickXToRollGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camFreefallPitchToDistanceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camFreefallPitchToForwardOffsetGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camParachutePitchToDistanceGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camParachutePitchToForwardOffsetGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camParachutePitchToModelPitchForwardOffsetGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camParachutePitchToModelRollSideOffsetGraph);
		reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, asset->skydive_camParachutePitchToModelRollUpOffsetGraph);

		for (auto& curve : asset->unknownCurves)
		{
			reader.read_asset(ASSET_TYPE_CURVE, curve);
		}

		scriptable_def::read_extra(reader, asset->unknownExtra);

		if (const auto table = reader.read_array(asset->unknownSoundTable, 7, 1))
		{
			if (const auto entries = reader.read_array(table->entries, 7, table->count))
			{
				for (auto i = 0u; i < table->count; i++)
				{
					reader.read_array(entries[i].aliases, 7, entries[i].count);
					reader.read_array(entries[i].values, 3, entries[i].count);
				}
			}

			reader.read_array(table->aliases, 7, table->count);
			reader.read_array(table->values, 3, table->count);
		}

		reader.pop_stream();
	}
}
