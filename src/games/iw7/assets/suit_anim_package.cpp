#include <std_include.hpp>
#include "suit_anim_package.hpp"

namespace zonetool::iw7
{
	void suit_anim_package::read(zone_reader& reader, SuitAnimPackage* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		for (auto& override : asset->animOverrides)
		{
			if (const auto anims = reader.read_array(override.anims, 7, override.numAnims))
			{
				for (auto i = 0u; i < override.numAnims; i++)
				{
					reader.read_asset(ASSET_TYPE_XANIMPARTS, anims[i].anim);
				}
			}

			if (const auto gestures = reader.read_array(override.gestures, 7, override.numGestures))
			{
				for (auto i = 0; i < override.numGestures; i++)
				{
					reader.read_asset(ASSET_TYPE_GESTURE, gestures[i].gesture);
				}
			}
		}
		reader.pop_stream();
	}
}
