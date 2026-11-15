#include <std_include.hpp>
#include "xanim.hpp"

namespace zonetool::iw8
{
	namespace
	{
		std::size_t frame_index_size(const XAnimParts* asset)
		{
			return asset->numframes >= 256 ? sizeof(unsigned short) : sizeof(unsigned char);
		}

		void read_delta_trans(zone_reader& reader, XAnimPartTrans*& field, const std::size_t index_size)
		{
			if (!reader.read_stream(field, 7, offsetof(XAnimPartTrans, u)))
			{
				return;
			}

			const auto trans = field;
			if (!trans->size)
			{
				reader.read_bytes(sizeof(trans->u.frame0));
				return;
			}

			reader.read_bytes(offsetof(XAnimPartTransFrames, indices));
			reader.read_bytes(index_size * (trans->size + 1));

			if (trans->smallTrans)
			{
				reader.read_stream(trans->u.frames.frames._1, 0, sizeof(*trans->u.frames.frames._1), trans->size + 1);
			}
			else
			{
				reader.read_stream(trans->u.frames.frames._2, 1, sizeof(*trans->u.frames.frames._2), trans->size + 1);
			}
		}

		template <typename T>
		void read_delta_quat(zone_reader& reader, T*& field, const std::size_t index_size)
		{
			if (!reader.read_stream(field, 7, offsetof(T, u)))
			{
				return;
			}

			const auto quat = field;
			if (!quat->size)
			{
				reader.read_bytes(sizeof(quat->u.frame0));
				return;
			}

			reader.read_bytes(sizeof(quat->u.frames.frames));
			reader.read_bytes(index_size * (quat->size + 1));
			reader.read_stream(quat->u.frames.frames, 1, sizeof(*quat->u.frames.frames), quat->size + 1);
		}
	}

	void xanim_parts::read(zone_reader& reader, XAnimParts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string_array(asset->names, asset->boneCount[9]);

		if (const auto notify = reader.read_array(asset->notify, 3, asset->notifyCount))
		{
			for (auto i = 0; i < asset->notifyCount; i++)
			{
				reader.read_script_string(notify[i].name);
			}
		}

		const auto index_size = frame_index_size(asset);
		if (const auto delta_part = reader.read_single(asset->deltaPart, 7))
		{
			read_delta_trans(reader, delta_part->trans, index_size);
			read_delta_quat(reader, delta_part->quat2, index_size);
			read_delta_quat(reader, delta_part->quat, index_size);
		}

		reader.read_stream(asset->dataByte, 0, sizeof(char), asset->dataByteCount);
		reader.read_stream(asset->dataShort, 1, sizeof(short), asset->dataShortCount);
		reader.read_stream(asset->dataInt, 3, sizeof(int), asset->dataIntCount);
		reader.read_stream(asset->randomDataShort, 1, sizeof(short), asset->randomDataShortCount);
		reader.read_stream(asset->randomDataByte, 0, sizeof(char), asset->randomDataByteCount);
		reader.read_stream(asset->randomDataInt, 3, sizeof(int), asset->randomDataIntCount);
		reader.read_stream(asset->indices.data, index_size - 1, index_size, asset->indexCount);
		reader.read_script_string_array(asset->blendShapeWeightNames, asset->blendShapeWeightCount);
		reader.read_array(asset->blendShapeWeights, 1, asset->blendShapeWeightCount * (asset->numframes + 1));
		reader.pop_stream();
	}
}
