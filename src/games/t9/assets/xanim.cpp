#include <std_include.hpp>
#include "xanim.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_indices(auto& reader, const std::size_t count, const bool byte_indices)
		{
			reader.read_bytes(byte_indices ? count : count * sizeof(std::uint16_t));
		}

		void read_trans(auto& reader, XAnimPartTrans*& field, const bool byte_indices)
		{
			if (!reader.read_stream(field, 7, offsetof(XAnimPartTrans, u)))
			{
				return;
			}

			const auto trans = field;
			if (trans->size == 0)
			{
				reader.read_bytes(sizeof(trans->u.frame0));
				return;
			}

			reader.read_bytes(sizeof(trans->u.frames));
			read_indices(reader, trans->size + 1, byte_indices);

			if (trans->smallTrans)
			{
				reader.read_array(trans->u.frames.frames.byteFrames, 0, trans->size + 1);
			}
			else
			{
				reader.read_array(trans->u.frames.frames.shortFrames, 1, trans->size + 1);
			}
		}

		template <typename Quat>
		void read_quat(auto& reader, Quat*& field, const bool byte_indices, const std::size_t frame_alignment)
		{
			if (!reader.read_stream(field, 7, offsetof(Quat, u)))
			{
				return;
			}

			const auto quat = field;
			if (quat->size == 0)
			{
				reader.read_bytes(sizeof(quat->u.frame0));
				return;
			}

			reader.read_bytes(sizeof(quat->u.frames));
			read_indices(reader, quat->size + 1, byte_indices);
			reader.read_array(quat->u.frames, frame_alignment, quat->size + 1);
		}

		void read_delta_part(auto& reader, XAnimDeltaPart*& field, const bool byte_indices)
		{
			if (const auto part = reader.read_array(field, 7))
			{
				read_trans(reader, part->trans, byte_indices);
				read_quat(reader, part->quat2, byte_indices, 3);
				read_quat(reader, part->quat, byte_indices, 7);
			}
		}

		template <typename T>
		void read_memmapped(auto& reader, T*& field, const std::size_t alignment, const std::size_t count)
		{
			reader.push_stream(XFILE_BLOCK_MEMMAPPED);
			reader.read_array(field, alignment, count);
			reader.pop_stream();
		}
	}

	void xanim::read(auto& reader, XAnim* asset)
	{
		const auto byte_indices = asset->numframes < 0x100;

		reader.read_array(asset->boneNames, 3, asset->boneNameCount);
		reader.read_array(asset->records4, 7, asset->records4Count);
		reader.read_array(asset->records6, 7, asset->records6Count);
		reader.read_array(asset->records5, 7, asset->records5Count);
		reader.read_array(asset->records3, 3, asset->records3Count);
		read_delta_part(reader, asset->deltaPart, byte_indices);

		read_memmapped(reader, asset->dataByte, 0, asset->dataByteCount);
		read_memmapped(reader, asset->dataShort, 1, asset->dataShortCount);
		read_memmapped(reader, asset->dataInt, 3, asset->dataIntCount);
		read_memmapped(reader, asset->dataShort2, 1, asset->dataShort2Count);
		read_memmapped(reader, asset->dataByte3, 0, asset->dataByte3Count);
		read_memmapped(reader, asset->dataInt2, 3, asset->dataInt2Count);

		reader.read_array(asset->dataByte2, 0, asset->dataByte2Count);
		reader.read_stream(asset->indices, byte_indices ? 0 : 1, byte_indices ? 1 : sizeof(std::uint16_t), asset->indexCount);
		reader.read_array(asset->values2, 3, asset->values2Count);
		reader.read_array(asset->records1, 3, asset->records1Count);

		if (const auto extra = reader.read_array(asset->extra, 7))
		{
			reader.read_array(extra->records, 1, extra->recordCount);
		}

		if (const auto tracks = reader.read_array(asset->boneTracks, 7, asset->nameCount))
		{
			for (auto i = 0; i < asset->nameCount; i++)
			{
				reader.read_array(tracks[i].indices, 1, tracks[i].count);
				reader.read_array(tracks[i].values, 3, tracks[i].count);
			}
		}

		reader.read_array(asset->names, 3, asset->nameCount);
		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey);
	}

	void sanim::read(auto& reader, SAnim* asset)
	{
		reader.read_string(asset->name);

		if (const auto data = reader.read_array(asset->data, 7))
		{
			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(data->data, 0xFFFF, data->dataSize);
			reader.pop_stream();
		}

		reader.read_array(asset->values1, 3, asset->values1Count);
		reader.read_array(asset->values2, 3, asset->values2Count);
	}

	void xcam::read(auto& reader, XCam* asset)
	{
		if (const auto notes = reader.read_array(asset->notes, 7))
		{
			reader.read_array(notes->values, 3, static_cast<std::size_t>(notes->count));
		}

		const auto track_count = static_cast<std::size_t>(asset->trackCount);
		if (const auto tracks = reader.read_array(asset->tracks, 7, track_count))
		{
			for (auto i = 0u; i < track_count; i++)
			{
				reader.read_array(tracks[i].keys, 3, static_cast<std::size_t>(tracks[i].keyCount));
			}
		}

		reader.read_array(asset->values1, 3, static_cast<std::size_t>(asset->values1Count));
		reader.read_array(asset->names, 3, static_cast<std::size_t>(asset->nameCount));
		reader.read_array(asset->targets1, 7, asset->target1Count);
		reader.read_array(asset->targets2, 7, asset->target2Count);
		reader.read_array(asset->targets3, 7, asset->target3Count);
	}
}

namespace zonetool::t9
{
	template void xanim::read(zone_reader&, XAnim*);
	template void sanim::read(zone_reader&, SAnim*);
	template void xcam::read(zone_reader&, XCam*);
	template void xanim::read(zone_writer&, XAnim*);
	template void sanim::read(zone_writer&, SAnim*);
	template void xcam::read(zone_writer&, XCam*);
}
