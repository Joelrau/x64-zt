#include <std_include.hpp>
#include "cinematicmotion.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_shake(zone_reader& reader, CinematicShakeDef& shake)
		{
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, shake.moveSpeedModifiers.nonLinearMap);
			reader.read_asset(ASSET_TYPE_RUMBLE_GRAPH, shake.viewSpeedModifiers.nonLinearMap);
		}
	}

	void cinematic_motion::read(zone_reader& reader, CinematicMotionDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_shake(reader, asset->camera_shake_rotation);
		read_shake(reader, asset->camera_shake_translation);
		reader.pop_stream();
	}
}
