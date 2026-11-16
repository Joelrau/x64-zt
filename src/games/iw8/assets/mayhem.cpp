#include <std_include.hpp>
#include "mayhem.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_frames(zone_reader& reader, MayhemAnim* anim)
		{
			const auto trans_bounds_count = anim->quantizeTrans * anim->numBones;
			if (anim->isSplineCompressed)
			{
				if (const auto frames = reader.read_single(anim->frames.splineCompressedFrames, 7))
				{
					reader.read_array(frames->diskQuat, 63, frames->totalCompressedQuatFrames * frames->quatStride);
					reader.read_array(frames->diskPos, 63, frames->totalCompressedPosFrames * frames->posStride);
					reader.read_array(frames->diskQuatFrames, 1, frames->totalCompressedQuatFrames / 3);
					reader.read_array(frames->diskPosFrames, 1, frames->totalCompressedPosFrames / 3);
					reader.read_array(frames->numDiskQuatFrames, 1, anim->numBones);
					reader.read_array(frames->numDiskPosFrames, 1, anim->numBones);
					reader.read_array(frames->transBounds, 3, trans_bounds_count);
				}
			}
			else if (const auto frames = reader.read_single(anim->frames.uncompressedFrames, 7))
			{
				reader.read_array(frames->diskQuat, 63, frames->quatStride * anim->numFrames);
				reader.read_array(frames->diskPos, 63, frames->posStride * anim->numFrames);
				reader.read_array(frames->transBounds, 3, trans_bounds_count);
			}
		}

		void read_data_channels(zone_reader& reader, MayhemAnim* anim)
		{
			if (anim->isSplineCompressed)
			{
				if (const auto keys = reader.read_single(anim->dataChannels.splineCompressedKeys, 7))
				{
					reader.read_array(keys->keys, 63, keys->keyStride * keys->totalCompressedKeyFrames);
					reader.read_array(keys->numKeys, 1, keys->numStreams);
					reader.read_array(keys->keyFrames, 1, keys->totalCompressedKeyFrames / 3);
				}
			}
			else if (const auto keys = reader.read_single(anim->dataChannels.uncompressedKeys, 7))
			{
				reader.read_array(keys->keys, 63, anim->numFrames * keys->keyStride * keys->numStreams);
			}
		}

		void read_anim(zone_reader& reader, MayhemAnim* anim)
		{
			read_frames(reader, anim);
			if (const auto notifies = reader.read_array(anim->notify, 3, anim->notifyCount))
			{
				for (auto i = 0u; i < anim->notifyCount; i++)
				{
					reader.read_script_string(notifies[i].name);
				}
			}

			read_data_channels(reader, anim);
		}
	}

	void mayhem::read(zone_reader& reader, MayhemData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->preModel);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->postModel);
		if (const auto models = reader.read_array(asset->models, 7, asset->numModels))
		{
			for (auto i = 0u; i < asset->numModels; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
			}
		}

		if (const auto anims = reader.read_array(asset->anims, 7, asset->numAnims))
		{
			for (auto i = 0u; i < asset->numAnims; i++)
			{
				read_anim(reader, &anims[i]);
			}
		}

		reader.read_array(asset->objects, 3, asset->numObjects);
		reader.pop_stream();
	}
}
