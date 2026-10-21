#include <std_include.hpp>
#include "s2.hpp"

#include <utils/flags.hpp>

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t delta_part_header_size = 8;

		std::size_t frame_index_size(const XAnimParts* asset)
		{
			return asset->numframes >= 256 ? sizeof(unsigned short) : sizeof(unsigned char);
		}

		void read_delta_trans(zone_reader& reader, XAnimPartTrans*& field, const std::size_t index_size)
		{
			if (!reader.read_stream(field, 7, delta_part_header_size))
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
			if (!reader.read_stream(field, 7, delta_part_header_size))
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
		reader.read_script_string_array(asset->names, asset->boneCount[PART_TYPE_ALL]);

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

		reader.read_array(asset->dataByte, 0, asset->dataByteCount);
		reader.read_array(asset->dataShort, 1, asset->dataShortCount);
		reader.read_array(asset->dataInt, 3, asset->dataIntCount);
		reader.read_array(asset->randomDataShort, 1, asset->randomDataShortCount);
		reader.read_array(asset->randomDataByte, 0, asset->randomDataByteCount);
		reader.read_array(asset->randomDataInt, 3, asset->randomDataIntCount);
		reader.read_stream(asset->indices.data, index_size - 1, index_size, asset->indexCount);
		reader.read_string(asset->secondaryName);
		reader.read_script_string_array(asset->blendShapeWeightNames, asset->blendShapeCount);
		reader.read_array(asset->blendShapeWeights, 3, asset->blendShapeCount * (asset->numframes + 1));
		reader.pop_stream();
	}

	namespace
	{
		std::size_t trans_frame_size(const XAnimPartTrans* trans)
		{
			return trans->smallTrans ? sizeof(*trans->u.frames.frames._1) : sizeof(*trans->u.frames.frames._2);
		}

		void write_delta_trans(zone_buffer* buf, XAnimPartTrans* trans, XAnimPartTrans*& dest_field, const std::size_t index_size)
		{
			if (!trans)
			{
				return;
			}

			buf->align(7);
			auto* dest = buf->at<XAnimPartTrans>();
			buf->write_stream(trans, delta_part_header_size);

			if (!trans->size)
			{
				buf->write_stream(&trans->u.frame0, sizeof(trans->u.frame0));
			}
			else
			{
				const auto frame_count = trans->size + 1;
				buf->write_stream(&trans->u.frames, offsetof(XAnimPartTransFrames, indices));
				buf->write_stream(&trans->u.frames.indices, index_size * frame_count);

				if (trans->smallTrans)
				{
					write_inline_bytes(buf, 0, trans->u.frames.frames._1, trans_frame_size(trans) * frame_count, dest->u.frames.frames._1);
				}
				else
				{
					write_inline_bytes(buf, 1, trans->u.frames.frames._2, trans_frame_size(trans) * frame_count, dest->u.frames.frames._2);
				}
			}

			buf->clear_pointer(&dest_field);
		}

		template <typename T>
		void write_delta_quat(zone_buffer* buf, T* quat, T*& dest_field, const std::size_t index_size)
		{
			if (!quat)
			{
				return;
			}

			buf->align(7);
			auto* dest = buf->at<T>();
			buf->write_stream(quat, delta_part_header_size);

			if (!quat->size)
			{
				buf->write_stream(&quat->u.frame0, sizeof(quat->u.frame0));
			}
			else
			{
				const auto frame_count = quat->size + 1;
				buf->write_stream(&quat->u.frames.frames, sizeof(quat->u.frames.frames));
				buf->write_stream(&quat->u.frames.indices, index_size * frame_count);
				write_inline_bytes(buf, 1, quat->u.frames.frames, sizeof(*quat->u.frames.frames) * frame_count, dest->u.frames.frames);
			}

			buf->clear_pointer(&dest_field);
		}

		std::string pack_delta_trans(const XAnimPartTrans* trans, const std::size_t index_size)
		{
			std::string data(reinterpret_cast<const char*>(trans), delta_part_header_size);
			if (!trans->size)
			{
				data.append(reinterpret_cast<const char*>(&trans->u.frame0), sizeof(trans->u.frame0));
				return data;
			}

			const auto frame_count = trans->size + 1;
			data.append(reinterpret_cast<const char*>(&trans->u.frames), offsetof(XAnimPartTransFrames, indices));
			data.append(reinterpret_cast<const char*>(&trans->u.frames.indices), index_size * frame_count);
			data.append(reinterpret_cast<const char*>(trans->u.frames.frames._1), trans_frame_size(trans) * frame_count);
			return data;
		}

		void unpack_delta_trans(XAnimPartTrans* trans, const std::size_t index_size)
		{
			if (trans->size)
			{
				const auto frames = reinterpret_cast<char*>(&trans->u.frames.indices) + index_size * (trans->size + 1);
				trans->u.frames.frames._1 = reinterpret_cast<unsigned char(*)[3]>(frames);
			}
		}

		template <typename T>
		std::string pack_delta_quat(const T* quat, const std::size_t index_size)
		{
			std::string data(reinterpret_cast<const char*>(quat), delta_part_header_size);
			if (!quat->size)
			{
				data.append(reinterpret_cast<const char*>(&quat->u.frame0), sizeof(quat->u.frame0));
				return data;
			}

			const auto frame_count = quat->size + 1;
			data.append(reinterpret_cast<const char*>(&quat->u.frames.frames), sizeof(quat->u.frames.frames));
			data.append(reinterpret_cast<const char*>(&quat->u.frames.indices), index_size * frame_count);
			data.append(reinterpret_cast<const char*>(quat->u.frames.frames), sizeof(*quat->u.frames.frames) * frame_count);
			return data;
		}

		template <typename T>
		void unpack_delta_quat(T* quat, const std::size_t index_size)
		{
			if (quat->size)
			{
				const auto frames = reinterpret_cast<char*>(&quat->u.frames.indices) + index_size * (quat->size + 1);
				quat->u.frames.frames = reinterpret_cast<decltype(quat->u.frames.frames)>(frames);
			}
		}

		template <typename T, typename Pack>
		void dump_delta(assetmanager::dumper& write, std::deque<std::string>& blobs, const T* part, Pack pack)
		{
			if (!part)
			{
				write.dump_array<char>(nullptr, 0);
				return;
			}

			const auto& blob = blobs.emplace_back(pack(part));
			write.dump_array(blob.data(), static_cast<std::uint32_t>(blob.size()));
		}
	}

	XAnimParts* xanim_parts::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open(dump_path(ASSET_TYPE_XANIM, name.data())))
		{
			return cast_xanim::parse(name, mem);
		}

		ZONETOOL_INFO("Parsing xanim \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimParts>();
		asset->name = read.read_string();
		asset->names = parse_script_string_array(read, mem, asset->boneCount[PART_TYPE_ALL]);

		asset->notify = read.read_array<XAnimNotifyInfo>();
		for (auto i = 0; asset->notify && i < asset->notifyCount; i++)
		{
			asset->notify[i].name = parse_script_string(read);
		}

		const auto index_size = frame_index_size(asset);
		asset->deltaPart = read.read_single<XAnimDeltaPart>();
		if (const auto delta_part = asset->deltaPart)
		{
			delta_part->trans = reinterpret_cast<XAnimPartTrans*>(read.read_array<char>());
			if (delta_part->trans)
			{
				unpack_delta_trans(delta_part->trans, index_size);
			}

			delta_part->quat2 = reinterpret_cast<XAnimDeltaPartQuat2*>(read.read_array<char>());
			if (delta_part->quat2)
			{
				unpack_delta_quat(delta_part->quat2, index_size);
			}

			delta_part->quat = reinterpret_cast<XAnimDeltaPartQuat*>(read.read_array<char>());
			if (delta_part->quat)
			{
				unpack_delta_quat(delta_part->quat, index_size);
			}
		}

		asset->dataByte = read.read_array<char>();
		asset->dataShort = read.read_array<short>();
		asset->dataInt = read.read_array<int>();
		asset->randomDataShort = read.read_array<short>();
		asset->randomDataByte = read.read_array<unsigned char>();
		asset->randomDataInt = read.read_array<int>();
		asset->indices.data = read.read_array<char>();
		asset->secondaryName = read.read_string();
		asset->blendShapeWeightNames = parse_script_string_array(read, mem, asset->blendShapeCount);
		asset->blendShapeWeights = read.read_array<float>();

		read.close();
		return asset;
	}

	void xanim_parts::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* asset = this->asset_;
		script_string_preparer prepare(buf);

		prepare(asset->names, asset->boneCount[PART_TYPE_ALL]);
		for (auto i = 0; asset->notify && i < asset->notifyCount; i++)
		{
			prepare(asset->notify[i].name);
		}
		prepare(asset->blendShapeWeightNames, asset->blendShapeCount);
	}

	void xanim_parts::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = this->write_name(buf);
		write_inline(buf, 3, data->names, data->boneCount[PART_TYPE_ALL], dest->names);
		write_inline(buf, 3, data->notify, data->notifyCount, dest->notify);

		const auto index_size = frame_index_size(data);
		if (data->deltaPart)
		{
			buf->align(7);
			auto* delta_part = buf->write(data->deltaPart);
			write_delta_trans(buf, data->deltaPart->trans, delta_part->trans, index_size);
			write_delta_quat(buf, data->deltaPart->quat2, delta_part->quat2, index_size);
			write_delta_quat(buf, data->deltaPart->quat, delta_part->quat, index_size);
			buf->clear_pointer(&dest->deltaPart);
		}

		write_inline(buf, 0, data->dataByte, data->dataByteCount, dest->dataByte);
		write_inline(buf, 1, data->dataShort, data->dataShortCount, dest->dataShort);
		write_inline(buf, 3, data->dataInt, data->dataIntCount, dest->dataInt);
		write_inline(buf, 1, data->randomDataShort, data->randomDataShortCount, dest->randomDataShort);
		write_inline(buf, 0, data->randomDataByte, data->randomDataByteCount, dest->randomDataByte);
		write_inline(buf, 3, data->randomDataInt, data->randomDataIntCount, dest->randomDataInt);
		write_inline_bytes(buf, index_size - 1, data->indices.data, index_size * data->indexCount, dest->indices.data);
		if (data->secondaryName)
		{
			dest->secondaryName = buf->write_str(data->secondaryName);
		}
		write_inline(buf, 3, data->blendShapeWeightNames, data->blendShapeCount, dest->blendShapeWeightNames);
		write_inline(buf, 3, data->blendShapeWeights, data->blendShapeCount * (data->numframes + 1), dest->blendShapeWeights);

		buf->pop_stream();
	}

	void xanim_parts::dump(XAnimParts* asset)
	{
		if (utils::flags::has_flag("castdump"))
		{
			cast_xanim::dump(asset);
		}

		assetmanager::dumper write;
		if (!write.open(dump_path(ASSET_TYPE_XANIM, asset->name)))
		{
			return;
		}

		std::deque<std::string> blobs;
		const auto index_size = frame_index_size(asset);

		write.dump_single(asset);
		write.dump_string(asset->name);
		dump_script_string_array(write, asset->names, asset->boneCount[PART_TYPE_ALL]);

		write.dump_array(asset->notify, asset->notifyCount);
		for (auto i = 0; asset->notify && i < asset->notifyCount; i++)
		{
			dump_script_string(write, asset->notify[i].name);
		}

		write.dump_single(asset->deltaPart);
		if (const auto delta_part = asset->deltaPart)
		{
			dump_delta(write, blobs, delta_part->trans, [&](const XAnimPartTrans* part) { return pack_delta_trans(part, index_size); });
			dump_delta(write, blobs, delta_part->quat2, [&](const XAnimDeltaPartQuat2* part) { return pack_delta_quat(part, index_size); });
			dump_delta(write, blobs, delta_part->quat, [&](const XAnimDeltaPartQuat* part) { return pack_delta_quat(part, index_size); });
		}

		write.dump_array(asset->dataByte, asset->dataByteCount);
		write.dump_array(asset->dataShort, asset->dataShortCount);
		write.dump_array(asset->dataInt, asset->dataIntCount);
		write.dump_array(asset->randomDataShort, asset->randomDataShortCount);
		write.dump_array(asset->randomDataByte, asset->randomDataByteCount);
		write.dump_array(asset->randomDataInt, asset->randomDataIntCount);
		write.dump_array(static_cast<char*>(asset->indices.data), static_cast<std::uint32_t>(index_size * asset->indexCount));
		write.dump_string(asset->secondaryName);
		dump_script_string_array(write, asset->blendShapeWeightNames, asset->blendShapeCount);
		write.dump_array(asset->blendShapeWeights, asset->blendShapeCount * (asset->numframes + 1));

		write.close();
	}
}
