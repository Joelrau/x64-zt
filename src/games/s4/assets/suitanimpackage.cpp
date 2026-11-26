#include <std_include.hpp>
#include "suitanimpackage.hpp"

namespace zonetool::s4
{
	void suit_anim_package::read(zone_reader& reader, SuitAnimPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto classes = reader.read_array(asset->animOverrides, 7, 6))
		{
			for (auto i = 0; i < 6; i++)
			{
				if (const auto anims = reader.read_array(classes[i].anims, 7, classes[i].numAnims))
				{
					for (auto j = 0u; j < classes[i].numAnims; j++)
					{
						reader.read_asset(ASSET_TYPE_XANIMPARTS, anims[j].anim);
					}
				}

				if (const auto gestures = reader.read_array(classes[i].gestures, 7, classes[i].numGestures))
				{
					for (auto j = 0u; j < classes[i].numGestures; j++)
					{
						reader.read_asset(ASSET_TYPE_GESTURE, gestures[j].gesture);
					}
				}

				reader.read_asset(ASSET_TYPE_CINEMATIC_MOTION, classes[i].cinematicMotion);
			}
		}
		reader.pop_stream();
	}
}
