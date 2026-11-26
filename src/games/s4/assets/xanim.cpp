#include <std_include.hpp>
#include "xanim.hpp"

namespace zonetool::s4
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

		std::size_t trans_size(const XAnimPartTrans* trans, const std::size_t index_size)
		{
			const auto data_size = trans->size ? offsetof(XAnimPartTransFrames, indices) + index_size * (trans->size + 1) : sizeof(trans->u.frame0);
			return offsetof(XAnimPartTrans, u) + data_size;
		}

		template <typename T>
		std::size_t quat_size(const T* quat, const std::size_t index_size)
		{
			const auto data_size = quat->size ? sizeof(quat->u.frames.frames) + index_size * (quat->size + 1) : sizeof(quat->u.frame0);
			return offsetof(T, u) + data_size;
		}

		template <typename T>
		void write_stream_array(zone_buffer* buf, const T* source, T*& target, const std::size_t size, const std::uint32_t alignment)
		{
			if (source)
			{
				buf->align(alignment);
				buf->write_stream(source, size);
				buf->clear_pointer(&target);
			}
		}

		void write_delta_trans(zone_buffer* buf, const XAnimPartTrans* source, XAnimPartTrans*& target, const std::size_t index_size)
		{
			if (!source)
			{
				return;
			}

			buf->align(7);
			auto* trans = buf->at<XAnimPartTrans>();
			buf->write_stream(source, trans_size(source, index_size));

			if (source->size)
			{
				const auto count = source->size + 1u;
				if (source->smallTrans)
				{
					write_stream_array(buf, source->u.frames.frames._1, trans->u.frames.frames._1, sizeof(*source->u.frames.frames._1) * count, 0);
				}
				else
				{
					write_stream_array(buf, source->u.frames.frames._2, trans->u.frames.frames._2, sizeof(*source->u.frames.frames._2) * count, 1);
				}
			}

			buf->clear_pointer(&target);
		}

		template <typename T>
		void write_delta_quat(zone_buffer* buf, const T* source, T*& target, const std::size_t index_size)
		{
			if (!source)
			{
				return;
			}

			buf->align(7);
			auto* quat = buf->at<T>();
			buf->write_stream(source, quat_size(source, index_size));

			if (source->size)
			{
				write_stream_array(buf, source->u.frames.frames, quat->u.frames.frames, sizeof(*source->u.frames.frames) * (source->size + 1), 1);
			}

			buf->clear_pointer(&target);
		}

		void dump_script_strings(assetmanager::dumper& dump, const scr_string_t* strings, const std::size_t count)
		{
			dump.dump_array(strings, static_cast<std::uint32_t>(count));
			if (strings)
			{
				for (std::size_t i = 0; i < count; i++)
				{
					dump.dump_string(SL_ConvertToString(strings[i]));
				}
			}
		}

		void dump_delta_trans(assetmanager::dumper& dump, XAnimPartTrans* trans, const std::size_t index_size)
		{
			dump.dump_raw(trans, trans ? static_cast<std::uint32_t>(trans_size(trans, index_size)) : 0);
			if (trans && trans->size)
			{
				const auto count = trans->size + 1u;
				if (trans->smallTrans)
				{
					dump.dump_raw(trans->u.frames.frames._1, static_cast<std::uint32_t>(sizeof(*trans->u.frames.frames._1) * count));
				}
				else
				{
					dump.dump_raw(trans->u.frames.frames._2, static_cast<std::uint32_t>(sizeof(*trans->u.frames.frames._2) * count));
				}
			}
		}

		template <typename T>
		void dump_delta_quat(assetmanager::dumper& dump, T* quat, const std::size_t index_size)
		{
			dump.dump_raw(quat, quat ? static_cast<std::uint32_t>(quat_size(quat, index_size)) : 0);
			if (quat && quat->size)
			{
				dump.dump_raw(quat->u.frames.frames, static_cast<std::uint32_t>(sizeof(*quat->u.frames.frames) * (quat->size + 1)));
			}
		}

		void parse_delta_trans(assetmanager::reader& read, XAnimPartTrans*& trans)
		{
			if ((trans = read.read_raw<XAnimPartTrans>()) && trans->size)
			{
				if (trans->smallTrans)
				{
					trans->u.frames.frames._1 = read.read_raw<unsigned char[3]>();
				}
				else
				{
					trans->u.frames.frames._2 = read.read_raw<unsigned short[3]>();
				}
			}
		}

		template <typename T>
		void parse_delta_quat(assetmanager::reader& read, T*& quat)
		{
			if ((quat = read.read_raw<T>()) && quat->size)
			{
				quat->u.frames.frames = read.read_raw<std::remove_pointer_t<decltype(quat->u.frames.frames)>>();
			}
		}
	}

	void xanim_parts::parse_script_strings(assetmanager::reader& read, scr_string_t*& strings, const std::size_t count)
	{
		strings = read.read_array<scr_string_t>();
		if (strings)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				this->script_strings_[&strings[i]] = read.read_string();
			}
		}
	}

	void xanim_parts::prepare_script_strings(zone_buffer* buf, scr_string_t* strings, const std::size_t count)
	{
		if (strings)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				strings[i] = static_cast<scr_string_t>(buf->write_scriptstring(this->script_strings_[&strings[i]]));
			}
		}
	}

	XAnimParts* xanim_parts::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("xanim\\"s + name + ".xab"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xanim \"%s\"...", name.data());

		auto* asset = read.read_single<XAnimParts>();
		asset->name = read.read_string();
		this->parse_script_strings(read, asset->names, asset->boneCount[9]);

		if ((asset->notify = read.read_array<XAnimNotifyInfo>()))
		{
			for (auto i = 0; i < asset->notifyCount; i++)
			{
				this->script_strings_[&asset->notify[i].name] = read.read_string();
			}
		}

		if ((asset->deltaPart = read.read_single<XAnimDeltaPart>()))
		{
			parse_delta_trans(read, asset->deltaPart->trans);
			parse_delta_quat(read, asset->deltaPart->quat2);
			parse_delta_quat(read, asset->deltaPart->quat);
		}

		if (const auto extra = asset->extraChannels = read.read_single<XAnimExtraChannels>())
		{
			this->parse_script_strings(read, extra->names, extra->nameCount);
			extra->values = read.read_array<unsigned int>();
			extra->frameData = read.read_array<unsigned char>();
		}

		asset->dataByte = read.read_array<char>();
		asset->dataShort = read.read_array<short>();
		asset->dataInt = read.read_array<int>();
		asset->randomDataShort = read.read_array<short>();
		asset->randomDataByte = read.read_array<char>();
		asset->randomDataInt = read.read_array<int>();

		if (frame_index_size(asset) == sizeof(unsigned short))
		{
			asset->indices._2 = read.read_array<unsigned short>();
		}
		else
		{
			asset->indices._1 = read.read_array<unsigned char>();
		}

		this->parse_script_strings(read, asset->blendShapeWeightNames, asset->blendShapeWeightCount);
		asset->blendShapeWeights = read.read_array<unsigned short>();

		read.close();
		return asset;
	}

	void xanim_parts::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XAnimParts>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void xanim_parts::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		this->prepare_script_strings(buf, data->names, data->boneCount[9]);

		if (data->notify)
		{
			for (auto i = 0; i < data->notifyCount; i++)
			{
				auto* name = &data->notify[i].name;
				*name = static_cast<scr_string_t>(buf->write_scriptstring(this->script_strings_[name]));
			}
		}

		if (data->extraChannels)
		{
			this->prepare_script_strings(buf, data->extraChannels->names, data->extraChannels->nameCount);
		}

		this->prepare_script_strings(buf, data->blendShapeWeightNames, data->blendShapeWeightCount);
	}

	void xanim_parts::load_depending(zone_base* zone)
	{
	}

	std::string xanim_parts::name()
	{
		return this->name_;
	}

	std::int32_t xanim_parts::type()
	{
		return ASSET_TYPE_XANIMPARTS;
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

		if (const auto extra = reader.read_single(asset->extraChannels, 7))
		{
			reader.read_script_string_array(extra->names, extra->nameCount);
			reader.read_stream(extra->values, 3, sizeof(unsigned int), extra->valueCount);
			reader.read_stream(extra->frameData, 0, sizeof(unsigned char), extra->frameDataStride * (asset->numframes + 1));
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

	void xanim_parts::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_stream_array(buf, data->names, dest->names, sizeof(scr_string_t) * data->boneCount[9], 3);
		write_stream_array(buf, data->notify, dest->notify, sizeof(XAnimNotifyInfo) * data->notifyCount, 3);

		const auto index_size = frame_index_size(data);
		if (auto* delta_part = data->deltaPart)
		{
			buf->align(7);
			auto* delta_dest = buf->write(delta_part);
			write_delta_trans(buf, delta_part->trans, delta_dest->trans, index_size);
			write_delta_quat(buf, delta_part->quat2, delta_dest->quat2, index_size);
			write_delta_quat(buf, delta_part->quat, delta_dest->quat, index_size);
			buf->clear_pointer(&dest->deltaPart);
		}

		if (auto* extra = data->extraChannels)
		{
			buf->align(7);
			auto* extra_dest = buf->write(extra);
			write_stream_array(buf, extra->names, extra_dest->names, sizeof(scr_string_t) * extra->nameCount, 3);
			write_stream_array(buf, extra->values, extra_dest->values, sizeof(unsigned int) * extra->valueCount, 3);
			write_stream_array(buf, extra->frameData, extra_dest->frameData, sizeof(unsigned char) * extra->frameDataStride * (data->numframes + 1), 0);
			buf->clear_pointer(&dest->extraChannels);
		}

		write_stream_array(buf, data->dataByte, dest->dataByte, sizeof(char) * data->dataByteCount, 0);
		write_stream_array(buf, data->dataShort, dest->dataShort, sizeof(short) * data->dataShortCount, 1);
		write_stream_array(buf, data->dataInt, dest->dataInt, sizeof(int) * data->dataIntCount, 3);
		write_stream_array(buf, data->randomDataShort, dest->randomDataShort, sizeof(short) * data->randomDataShortCount, 1);
		write_stream_array(buf, data->randomDataByte, dest->randomDataByte, sizeof(char) * data->randomDataByteCount, 0);
		write_stream_array(buf, data->randomDataInt, dest->randomDataInt, sizeof(int) * data->randomDataIntCount, 3);
		write_stream_array(buf, data->indices.data, dest->indices.data, index_size * data->indexCount, static_cast<std::uint32_t>(index_size - 1));
		write_stream_array(buf, data->blendShapeWeightNames, dest->blendShapeWeightNames, sizeof(scr_string_t) * data->blendShapeWeightCount, 3);
		write_stream_array(buf, data->blendShapeWeights, dest->blendShapeWeights, sizeof(unsigned short) * data->blendShapeWeightCount * (data->numframes + 1), 1);
		buf->pop_stream();
	}

	void xanim_parts::dump(XAnimParts* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("xanim\\"s + asset->name + ".xab"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump_script_strings(dump, asset->names, asset->boneCount[9]);

		dump.dump_array(asset->notify, asset->notifyCount);
		if (asset->notify)
		{
			for (auto i = 0; i < asset->notifyCount; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->notify[i].name));
			}
		}

		const auto index_size = frame_index_size(asset);
		dump.dump_single(asset->deltaPart);
		if (asset->deltaPart)
		{
			dump_delta_trans(dump, asset->deltaPart->trans, index_size);
			dump_delta_quat(dump, asset->deltaPart->quat2, index_size);
			dump_delta_quat(dump, asset->deltaPart->quat, index_size);
		}

		dump.dump_single(asset->extraChannels);
		if (const auto extra = asset->extraChannels)
		{
			dump_script_strings(dump, extra->names, extra->nameCount);
			dump.dump_array(extra->values, extra->valueCount);
			dump.dump_array(extra->frameData, extra->frameDataStride * (asset->numframes + 1u));
		}

		dump.dump_array(asset->dataByte, asset->dataByteCount);
		dump.dump_array(asset->dataShort, asset->dataShortCount);
		dump.dump_array(asset->dataInt, asset->dataIntCount);
		dump.dump_array(asset->randomDataShort, asset->randomDataShortCount);
		dump.dump_array(asset->randomDataByte, asset->randomDataByteCount);
		dump.dump_array(asset->randomDataInt, asset->randomDataIntCount);

		if (index_size == sizeof(unsigned short))
		{
			dump.dump_array(asset->indices._2, asset->indexCount);
		}
		else
		{
			dump.dump_array(asset->indices._1, asset->indexCount);
		}

		dump_script_strings(dump, asset->blendShapeWeightNames, asset->blendShapeWeightCount);
		dump.dump_array(asset->blendShapeWeights, asset->blendShapeWeightCount * (asset->numframes + 1u));
		dump.close();
	}
}
