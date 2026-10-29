#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr auto disk_alignment = 63;

		std::size_t trans_bounds_count(const MayhemAnim& anim)
		{
			return anim.quantizeTrans * anim.numBones;
		}

		std::size_t uncompressed_keys_size(const MayhemAnim& anim, const MayhemDataKeysUncompressed& keys)
		{
			return anim.numFrames * keys.keyStride; // S2 has no numStreams factor here, unlike IW7
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
			reader.read_array(frames->transBounds, 3, trans_bounds_count(anim));
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
			reader.read_array(frames->transBounds, 3, trans_bounds_count(anim));
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

			reader.read_array(keys->keys, disk_alignment, uncompressed_keys_size(anim, *keys));
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

		void parse_spline_frames(assetmanager::reader& read, zone_memory* mem, MayhemAnimFramesSplineCompressed*& field, const MayhemAnim& anim)
		{
			const auto frames = read.read_single<MayhemAnimFramesSplineCompressed>();
			field = frames;
			if (!frames)
			{
				return;
			}

			parse_array(read, mem, frames->diskQuat, frames->quatStride * frames->totalCompressedQuatFrames);
			parse_array(read, mem, frames->diskPos, frames->posStride * frames->totalCompressedPosFrames);
			parse_array(read, mem, frames->diskQuatFrames, frames->totalCompressedQuatFrames / 3);
			parse_array(read, mem, frames->diskPosFrames, frames->totalCompressedPosFrames / 3);
			parse_array(read, mem, frames->numDiskQuatFrames, anim.numBones);
			parse_array(read, mem, frames->numDiskPosFrames, anim.numBones);
			parse_array(read, mem, frames->transBounds, trans_bounds_count(anim));
		}

		void parse_uncompressed_frames(assetmanager::reader& read, zone_memory* mem, MayhemAnimFramesUncompressed*& field, const MayhemAnim& anim)
		{
			const auto frames = read.read_single<MayhemAnimFramesUncompressed>();
			field = frames;
			if (!frames)
			{
				return;
			}

			parse_array(read, mem, frames->diskQuat, anim.numFrames * frames->quatStride);
			parse_array(read, mem, frames->diskPos, anim.numFrames * frames->posStride);
			parse_array(read, mem, frames->transBounds, trans_bounds_count(anim));
		}

		void parse_spline_keys(assetmanager::reader& read, zone_memory* mem, MayhemDataKeysSplineCompressed*& field)
		{
			const auto keys = read.read_single<MayhemDataKeysSplineCompressed>();
			field = keys;
			if (!keys)
			{
				return;
			}

			parse_array(read, mem, keys->keys, keys->keyStride * keys->totalCompressedKeyFrames);
			parse_array(read, mem, keys->numKeys, keys->numStreams);
			parse_array(read, mem, keys->keyFrames, keys->totalCompressedKeyFrames / 3);
		}

		void parse_uncompressed_keys(assetmanager::reader& read, zone_memory* mem, MayhemDataKeysUncompressed*& field, const MayhemAnim& anim)
		{
			const auto keys = read.read_single<MayhemDataKeysUncompressed>();
			field = keys;
			if (!keys)
			{
				return;
			}

			parse_array(read, mem, keys->keys, uncompressed_keys_size(anim, *keys));
		}

		void parse_anim(assetmanager::reader& read, zone_memory* mem, MayhemAnim& anim)
		{
			if (anim.isSplineCompressed)
			{
				parse_spline_frames(read, mem, anim.frames.splineCompressedFrames, anim);
			}
			else
			{
				parse_uncompressed_frames(read, mem, anim.frames.uncompressedFrames, anim);
			}

			if (const auto notify = parse_array(read, mem, anim.notify, anim.notifyCount))
			{
				for (auto i = 0u; i < anim.notifyCount; i++)
				{
					notify[i].name = parse_script_string(read);
				}
			}

			if (anim.isSplineCompressed)
			{
				parse_spline_keys(read, mem, anim.dataChannels.splineCompressedKeys);
			}
			else
			{
				parse_uncompressed_keys(read, mem, anim.dataChannels.uncompressedKeys, anim);
			}
		}

		void write_spline_frames(zone_buffer* buf, MayhemAnimFramesSplineCompressed* frames, MayhemAnimFramesSplineCompressed*& dest_field, const MayhemAnim& anim)
		{
			const auto dest = write_array(buf, 7, frames, 1, dest_field);
			if (!dest)
			{
				return;
			}

			write_array(buf, disk_alignment, frames->diskQuat, frames->quatStride * frames->totalCompressedQuatFrames, dest->diskQuat);
			write_array(buf, disk_alignment, frames->diskPos, frames->posStride * frames->totalCompressedPosFrames, dest->diskPos);
			write_array(buf, 1, frames->diskQuatFrames, frames->totalCompressedQuatFrames / 3, dest->diskQuatFrames);
			write_array(buf, 1, frames->diskPosFrames, frames->totalCompressedPosFrames / 3, dest->diskPosFrames);
			write_array(buf, 1, frames->numDiskQuatFrames, anim.numBones, dest->numDiskQuatFrames);
			write_array(buf, 1, frames->numDiskPosFrames, anim.numBones, dest->numDiskPosFrames);
			write_array(buf, 3, frames->transBounds, trans_bounds_count(anim), dest->transBounds);
			dest->quat.buffer = nullptr;
			dest->pos.buffer = nullptr;
		}

		void write_uncompressed_frames(zone_buffer* buf, MayhemAnimFramesUncompressed* frames, MayhemAnimFramesUncompressed*& dest_field, const MayhemAnim& anim)
		{
			const auto dest = write_array(buf, 7, frames, 1, dest_field);
			if (!dest)
			{
				return;
			}

			write_array(buf, disk_alignment, frames->diskQuat, anim.numFrames * frames->quatStride, dest->diskQuat);
			write_array(buf, disk_alignment, frames->diskPos, anim.numFrames * frames->posStride, dest->diskPos);
			write_array(buf, 3, frames->transBounds, trans_bounds_count(anim), dest->transBounds);
			dest->quat.buffer = nullptr;
			dest->pos.buffer = nullptr;
		}

		void write_spline_keys(zone_buffer* buf, MayhemDataKeysSplineCompressed* keys, MayhemDataKeysSplineCompressed*& dest_field)
		{
			const auto dest = write_array(buf, 7, keys, 1, dest_field);
			if (!dest)
			{
				return;
			}

			write_array(buf, disk_alignment, keys->keys, keys->keyStride * keys->totalCompressedKeyFrames, dest->keys);
			write_array(buf, 1, keys->numKeys, keys->numStreams, dest->numKeys);
			write_array(buf, 1, keys->keyFrames, keys->totalCompressedKeyFrames / 3, dest->keyFrames);
		}

		void write_uncompressed_keys(zone_buffer* buf, MayhemDataKeysUncompressed* keys, MayhemDataKeysUncompressed*& dest_field, const MayhemAnim& anim)
		{
			const auto dest = write_array(buf, 7, keys, 1, dest_field);
			if (!dest)
			{
				return;
			}

			write_array(buf, disk_alignment, keys->keys, uncompressed_keys_size(anim, *keys), dest->keys);
		}

		void write_anim(zone_buffer* buf, MayhemAnim& anim, MayhemAnim& dest)
		{
			if (anim.isSplineCompressed)
			{
				write_spline_frames(buf, anim.frames.splineCompressedFrames, dest.frames.splineCompressedFrames, anim);
			}
			else
			{
				write_uncompressed_frames(buf, anim.frames.uncompressedFrames, dest.frames.uncompressedFrames, anim);
			}

			write_array(buf, 3, anim.notify, anim.notifyCount, dest.notify);

			if (anim.isSplineCompressed)
			{
				write_spline_keys(buf, anim.dataChannels.splineCompressedKeys, dest.dataChannels.splineCompressedKeys);
			}
			else
			{
				write_uncompressed_keys(buf, anim.dataChannels.uncompressedKeys, dest.dataChannels.uncompressedKeys, anim);
			}
		}

		void dump_anim(assetmanager::dumper& write, const MayhemAnim& anim)
		{
			if (anim.isSplineCompressed)
			{
				const auto frames = anim.frames.splineCompressedFrames;
				write.dump_single(frames);
				if (frames)
				{
					write.dump_array(frames->diskQuat, frames->quatStride * frames->totalCompressedQuatFrames);
					write.dump_array(frames->diskPos, frames->posStride * frames->totalCompressedPosFrames);
					write.dump_array(frames->diskQuatFrames, frames->totalCompressedQuatFrames / 3);
					write.dump_array(frames->diskPosFrames, frames->totalCompressedPosFrames / 3);
					write.dump_array(frames->numDiskQuatFrames, anim.numBones);
					write.dump_array(frames->numDiskPosFrames, anim.numBones);
					write.dump_array(frames->transBounds, static_cast<std::uint32_t>(trans_bounds_count(anim)));
				}
			}
			else
			{
				const auto frames = anim.frames.uncompressedFrames;
				write.dump_single(frames);
				if (frames)
				{
					write.dump_array(frames->diskQuat, anim.numFrames * frames->quatStride);
					write.dump_array(frames->diskPos, anim.numFrames * frames->posStride);
					write.dump_array(frames->transBounds, static_cast<std::uint32_t>(trans_bounds_count(anim)));
				}
			}

			write.dump_array(anim.notify, anim.notifyCount);
			for (auto i = 0u; anim.notify && i < anim.notifyCount; i++)
			{
				dump_script_string(write, anim.notify[i].name);
			}

			if (anim.isSplineCompressed)
			{
				const auto keys = anim.dataChannels.splineCompressedKeys;
				write.dump_single(keys);
				if (keys)
				{
					write.dump_array(keys->keys, keys->keyStride * keys->totalCompressedKeyFrames);
					write.dump_array(keys->numKeys, keys->numStreams);
					write.dump_array(keys->keyFrames, keys->totalCompressedKeyFrames / 3);
				}
			}
			else
			{
				const auto keys = anim.dataChannels.uncompressedKeys;
				write.dump_single(keys);
				if (keys)
				{
					write.dump_array(keys->keys, static_cast<std::uint32_t>(uncompressed_keys_size(anim, *keys)));
				}
			}
		}
	}

	void mayhem::read(zone_reader& reader, Mayhem* asset)
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

	Mayhem* mayhem::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_MAYHEM, name.data())))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing mayhem \"%s\"...", name.data());

		auto* asset = read.read_single<Mayhem>();
		asset->name = read.read_string();
		asset->preModel = read.read_asset<XModel>();
		asset->postModel = read.read_asset<XModel>();

		if (const auto models = parse_array(read, mem, asset->models, asset->numModels))
		{
			for (auto i = 0u; i < asset->numModels; i++)
			{
				models[i].model = read.read_asset<XModel>();
			}
		}

		if (const auto anims = parse_array(read, mem, asset->anims, asset->numAnims))
		{
			for (auto i = 0u; i < asset->numAnims; i++)
			{
				parse_anim(read, mem, anims[i]);
			}
		}

		parse_array(read, mem, asset->objects, asset->numObjects);
		read.close();

		return asset;
	}

	void mayhem::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		script_string_preparer prepare(buf);

		for (auto i = 0u; data->anims && i < data->numAnims; i++)
		{
			const auto& anim = data->anims[i];
			for (auto j = 0u; anim.notify && j < anim.notifyCount; j++)
			{
				prepare(anim.notify[j].name);
			}
		}
	}

	void mayhem::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_dependency(zone, ASSET_TYPE_XMODEL, data->preModel);
		add_dependency(zone, ASSET_TYPE_XMODEL, data->postModel);
		for (auto i = 0u; data->models && i < data->numModels; i++)
		{
			add_dependency(zone, ASSET_TYPE_XMODEL, data->models[i].model);
		}
	}

	void mayhem::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		dest->preModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->preModel);
		dest->postModel = asset_reference(zone, ASSET_TYPE_XMODEL, data->postModel);

		if (const auto dest_models = write_array(buf, 7, data->models, data->numModels, dest->models))
		{
			for (auto i = 0u; i < data->numModels; i++)
			{
				dest_models[i].model = asset_reference(zone, ASSET_TYPE_XMODEL, data->models[i].model);
			}
		}

		if (const auto dest_anims = write_array(buf, 7, data->anims, data->numAnims, dest->anims))
		{
			for (auto i = 0u; i < data->numAnims; i++)
			{
				write_anim(buf, data->anims[i], dest_anims[i]);
			}
		}

		write_array(buf, 3, data->objects, data->numObjects, dest->objects);

		buf->pop_stream();
	}

	void mayhem::dump(Mayhem* asset)
	{
		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_MAYHEM, asset->name)))
		{
			return;
		}

		write.dump_single(asset);
		write.dump_string(asset->name);
		write.dump_asset(asset->preModel);
		write.dump_asset(asset->postModel);

		write.dump_array(asset->models, asset->numModels);
		for (auto i = 0u; asset->models && i < asset->numModels; i++)
		{
			write.dump_asset(asset->models[i].model);
		}

		write.dump_array(asset->anims, asset->numAnims);
		for (auto i = 0u; asset->anims && i < asset->numAnims; i++)
		{
			dump_anim(write, asset->anims[i]);
		}

		write.dump_array(asset->objects, asset->numObjects);
		write.close();
	}
}
