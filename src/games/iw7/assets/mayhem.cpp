#include <std_include.hpp>
#include "mayhem.hpp"

namespace zonetool::iw7
{
	namespace
	{
		constexpr auto disk_alignment = 63;

		void read_trans_bounds(zone_reader& reader, MayhemTransBounds*& bounds, const MayhemAnim& anim)
		{
			reader.read_array(bounds, 3, anim.quantizeTrans * anim.numBones);
		}

		void read_spline_frames(zone_reader& reader, MayhemAnimFramesSplineCompressed*& field, const MayhemAnim& anim)
		{
			const auto frames = reader.read_single(field, 7);
			if (!frames)
			{
				return;
			}

			reader.read_array(frames->diskQuat, disk_alignment, frames->quatStride * frames->totalCompressedQuatFrames);
			reader.read_array(frames->diskPos, disk_alignment, frames->posStride * frames->totalCompressedPosFrames);
			reader.read_array(frames->diskQuatFrames, 1, frames->totalCompressedQuatFrames / 3);
			reader.read_array(frames->diskPosFrames, 1, frames->totalCompressedPosFrames / 3);
			reader.read_array(frames->numDiskQuatFrames, 1, anim.numBones);
			reader.read_array(frames->numDiskPosFrames, 1, anim.numBones);
			read_trans_bounds(reader, frames->transBounds, anim);
		}

		void read_uncompressed_frames(zone_reader& reader, MayhemAnimFramesUncompressed*& field, const MayhemAnim& anim)
		{
			const auto frames = reader.read_single(field, 7);
			if (!frames)
			{
				return;
			}

			reader.read_array(frames->diskQuat, disk_alignment, anim.numFrames * frames->quatStride);
			reader.read_array(frames->diskPos, disk_alignment, anim.numFrames * frames->posStride);
			read_trans_bounds(reader, frames->transBounds, anim);
		}

		void read_spline_keys(zone_reader& reader, MayhemDataKeysSplineCompressed*& field)
		{
			const auto keys = reader.read_single(field, 7);
			if (!keys)
			{
				return;
			}

			reader.read_array(keys->keys, disk_alignment, keys->keyStride * keys->totalCompressedKeyFrames);
			reader.read_array(keys->numKeys, 1, keys->numStreams);
			reader.read_array(keys->keyFrames, 1, keys->totalCompressedKeyFrames / 3);
		}

		void read_uncompressed_keys(zone_reader& reader, MayhemDataKeysUncompressed*& field, const MayhemAnim& anim)
		{
			const auto keys = reader.read_single(field, 7);
			if (!keys)
			{
				return;
			}

			reader.read_array(keys->keys, disk_alignment, anim.numFrames * keys->keyStride * keys->numStreams);
		}

		void read_anim(zone_reader& reader, MayhemAnim& anim)
		{
			if (anim.isSplineCompressed)
			{
				read_spline_frames(reader, anim.frames.splineCompressedFrames, anim);
			}
			else
			{
				read_uncompressed_frames(reader, anim.frames.uncompressedFrames, anim);
			}

			if (const auto notify = reader.read_array(anim.notify, 3, anim.notifyCount))
			{
				for (auto i = 0u; i < anim.notifyCount; i++)
				{
					reader.read_script_string(notify[i].name);
				}
			}

			if (anim.isSplineCompressed)
			{
				read_spline_keys(reader, anim.dataChannels.splineCompressedKeys);
			}
			else
			{
				read_uncompressed_keys(reader, anim.dataChannels.uncompressedKeys, anim);
			}
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
				read_anim(reader, anims[i]);
			}
		}

		reader.read_array(asset->objects, 3, asset->numObjects);
		reader.pop_stream();
	}
}
