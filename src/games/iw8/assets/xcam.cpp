#include <std_include.hpp>
#include "xcam.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_notifies(zone_reader& reader, XAnimNotifyInfo*& field, const unsigned char count)
		{
			if (const auto notifies = reader.read_array(field, 3, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_script_string(notifies[i].name);
				}
			}
		}
	}

	void xcam::read(zone_reader& reader, XCam* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->parent_scene);
		if (const auto target = reader.read_single(asset->targetModelBoneRoots, 7))
		{
			reader.read_string(target->name);
			reader.read_array(target->animationFrames, 3, target->frames);
		}

		if (const auto cameras = reader.read_array(asset->cameras, 7, asset->cameraCount))
		{
			for (auto i = 0; i < asset->cameraCount; i++)
			{
				reader.read_string(cameras[i].name);
				reader.read_array(cameras[i].animationFrames, 3, cameras[i].frames);
			}
		}

		reader.read_array(asset->cameraSwitchParams, 3, asset->frames);
		if (const auto names = reader.read_array(asset->frameNames, 7, asset->frameNameCount))
		{
			for (auto i = 0; i < asset->frameNameCount; i++)
			{
				reader.read_string(names[i].frameName);
			}
		}

		read_notifies(reader, asset->notify, asset->notifyCount);
		read_notifies(reader, asset->startupNotify, asset->startupNotifyCount);
		read_notifies(reader, asset->shutdownNotify, asset->shutdownNotifyCount);
		reader.pop_stream();
	}

	void execution_def::read(zone_reader& reader, ExecutionDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->propWeaponAttachTag);
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcamVictimStand);
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcamVictimProne);
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcamVictimLastStand);
		reader.pop_stream();
	}
}
