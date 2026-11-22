#include <std_include.hpp>
#include "xanimparts.hpp"

namespace zonetool::iw3
{
	namespace
	{
		std::size_t index_size(const XAnimParts* asset)
		{
			return asset->numframes < 256 ? sizeof(unsigned char) : sizeof(std::uint16_t);
		}

		void read_translation(zone_reader32& reader, const XAnimParts* parts, x86::pointer<XAnimPartTrans>& field)
		{
			const auto trans = reader.read_dynamic(field, 3, offsetof(XAnimPartTrans, u), [parts](const XAnimPartTrans* data)
			{
				if (data->size > 0)
				{
					return offsetof(XAnimPartTrans, u) + offsetof(XAnimPartTransFrames, indices) + (data->size + 1) * index_size(parts);
				}

				return offsetof(XAnimPartTrans, u) + sizeof(vec3_t);
			});

			if (!trans || trans->size == 0)
			{
				return;
			}

			const auto count = trans->size + 1;
			auto& frames = trans->u.frames.frames;
			if (trans->smallTrans)
			{
				reader.read_array(frames._1, 0, count);
			}
			else
			{
				reader.read_array(frames._2, 3, count);
			}
		}

		void read_quaternion(zone_reader32& reader, const XAnimParts* parts, x86::pointer<XAnimDeltaPartQuat>& field)
		{
			const auto quat = reader.read_dynamic(field, 3, offsetof(XAnimDeltaPartQuat, u), [parts](const XAnimDeltaPartQuat* data)
			{
				if (data->size > 0)
				{
					return offsetof(XAnimDeltaPartQuat, u) + offsetof(XAnimDeltaPartQuatDataFrames, indices) + (data->size + 1) * index_size(parts);
				}

				return offsetof(XAnimDeltaPartQuat, u) + sizeof(XQuat2);
			});

			if (quat && quat->size > 0)
			{
				reader.read_array(quat->u.frames.frames, 3, quat->size + 1);
			}
		}
	}

	void xanim_parts::read(zone_reader32& reader, XAnimParts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto names = reader.read_array(asset->names, 1, asset->boneCount[PART_TYPE_ALL]))
		{
			reader.read_script_strings(names, asset->boneCount[PART_TYPE_ALL]);
		}

		if (const auto notify = reader.read_array(asset->notify, 3, asset->notifyCount))
		{
			for (auto i = 0; i < asset->notifyCount; i++)
			{
				reader.read_script_string(notify[i].name);
			}
		}

		if (const auto delta = reader.read_single(asset->deltaPart, 3))
		{
			read_translation(reader, asset, delta->trans);
			read_quaternion(reader, asset, delta->quat);
		}

		reader.read_array(asset->dataByte, 0, asset->dataByteCount);
		reader.read_array(asset->dataShort, 1, asset->dataShortCount);
		reader.read_array(asset->dataInt, 3, asset->dataIntCount);
		reader.read_array(asset->randomDataShort, 1, asset->randomDataShortCount);
		reader.read_array(asset->randomDataByte, 0, asset->randomDataByteCount);
		reader.read_array(asset->randomDataInt, 3, asset->randomDataIntCount);

		if (asset->numframes < 256)
		{
			reader.read_array(asset->indices._1, 0, asset->indexCount);
		}
		else
		{
			reader.read_array(asset->indices._2, 1, asset->indexCount);
		}

		reader.pop_stream();
	}
}
