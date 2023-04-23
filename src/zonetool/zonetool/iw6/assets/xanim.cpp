#include "std_include.hpp"
#include "xanim.hpp"

namespace zonetool::iw6
{
	void xanim_parts::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* xanim_parts::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	XAnimParts* xanim_parts::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "xanim\\"s + name + ".xab";

		assetmanager::reader reader(mem);
		if (!reader.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xanim \"%s\"...", name.data());

		auto* asset = reader.read_single<XAnimParts>();
		asset->name = reader.read_string();

		if (asset->names)
		{
			asset->names = mem->allocate<scr_string_t>(asset->boneCount[9]);
			for (unsigned char bone = 0; bone < asset->boneCount[9]; bone++)
			{
				this->add_script_string(&asset->names[bone], reader.read_string());
			}
		}

		if (asset->dataByte)
		{
			asset->dataByte = reader.read_array<char>();
		}
		if (asset->dataShort)
		{
			asset->dataShort = reader.read_array<short>();
		}
		if (asset->dataInt)
		{
			asset->dataInt = reader.read_array<int>();
		}
		if (asset->randomDataShort)
		{
			asset->randomDataShort = reader.read_array<short>();
		}
		if (asset->randomDataByte)
		{
			asset->randomDataByte = reader.read_array<unsigned char>();
		}
		if (asset->randomDataInt)
		{
			asset->randomDataInt = reader.read_array<int>();
		}
		if (asset->indices.data)
		{
			if (asset->numframes >= 256)
			{
				asset->indices._2 = reader.read_array<unsigned short>();
			}
			else
			{
				asset->indices._1 = reader.read_array<unsigned char>();
			}
		}

		if (asset->notify)
		{
			asset->notify = reader.read_array<XAnimNotifyInfo>();
			for (unsigned char i = 0; i < asset->notifyCount; i++)
			{
				this->add_script_string(&asset->notify[i].name, reader.read_string());
			}
		}

		if (asset->deltaPart)
		{
			asset->deltaPart = reader.read_single<XAnimDeltaPart>();

			if (asset->deltaPart->trans)
			{
				asset->deltaPart->trans = reader.read_raw<XAnimPartTrans>();

				if (asset->deltaPart->trans->size)
				{
					if (asset->deltaPart->trans->u.frames.frames._1)
					{
						if (asset->deltaPart->trans->smallTrans)
						{
							asset->deltaPart->trans->u.frames.frames._1 = reader.read_raw<unsigned char[3]>();
						}
						else
						{
							asset->deltaPart->trans->u.frames.frames._2 = reader.read_raw<unsigned short[3]>();
						}
					}
				}
			}

			if (asset->deltaPart->quat2)
			{
				asset->deltaPart->quat2 = reader.read_raw<XAnimDeltaPartQuat2>();

				if (asset->deltaPart->quat2->size && asset->deltaPart->quat2->u.frames.frames)
				{
					asset->deltaPart->quat2->u.frames.frames = reader.read_raw<short[2]>();
				}
			}

			if (asset->deltaPart->quat)
			{
				asset->deltaPart->quat = reader.read_raw<XAnimDeltaPartQuat>();

				if (asset->deltaPart->quat->size && asset->deltaPart->quat->u.frames.frames)
				{
					asset->deltaPart->quat->u.frames.frames = reader.read_raw<short[4]>();
				}
			}
		}

		reader.close();

		return asset;
	}

	void xanim_parts::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = xanim_parts::parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_copy<XAnimParts>(XAssetType(this->type()), this->name().data(), mem).parts;

			auto* asset = this->asset_;

			auto* original_names = asset->names;
			asset->names = mem->allocate<scr_string_t>(asset->boneCount[9]);
			for (auto bone = 0; bone < asset->boneCount[9]; bone++)
			{
				this->add_script_string(&asset->names[bone], SL_ConvertToString(original_names[bone]));
			}

			auto* original_notifies = asset->notify;
			asset->notify = mem->allocate<XAnimNotifyInfo>(asset->notifyCount);
			if (asset->notify)
			{
				for (auto i = 0; i < asset->notifyCount; i++)
				{
					asset->notify[i].time = original_notifies[i].time;
					this->add_script_string(&asset->notify[i].name, SL_ConvertToString(original_notifies[i].name));
				}
			}
		}
	}

	void xanim_parts::prepare(zone_buffer* buf, zone_memory* mem)
	{
		// fixup scriptstrings
		auto* xanim = this->asset_;

		// name tagnames
		if (xanim->names)
		{
			for (unsigned char i = 0; i < xanim->boneCount[9]; i++)
			{
				xanim->names[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xanim->names[i])));
			}
		}

		// name notetracks
		for (unsigned char i = 0; i < xanim->notifyCount; i++)
		{
			xanim->notify[i].name = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&xanim->notify[i].name)));
		}
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

	void xanim_parts::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->names) // tagnames
		{
			buf->align(3);
			buf->write(data->names, data->boneCount[9]);
			zone_buffer::clear_pointer(&dest->names);
		}
		if (data->notify) // notetracks
		{
			buf->align(3);
			buf->write(data->notify, data->notifyCount);
			zone_buffer::clear_pointer(&dest->notify);
		}

		if (data->deltaPart) // XAnimDeltaParts
		{
			buf->align(3);
			auto* partdata = data->deltaPart;
			auto* partdest = buf->write(partdata);

			if (partdata->trans)
			{
				buf->align(3);
				buf->write_stream(partdata->trans, 8, 1); // not full struct
				if (partdata->trans->size)
				{
					buf->write_stream(&partdata->trans->u, 32, 1); // not full struct
					if (data->numframes >= 256)
					{
						buf->write_stream(&partdata->trans->u.frames.indices, sizeof(short), partdata->trans->size + 1);
					}
					else
					{
						buf->write_stream(&partdata->trans->u.frames.indices, sizeof(char), partdata->trans->size + 1);
					}

					if (partdata->trans->u.frames.frames._1)
					{
						if (partdata->trans->smallTrans)
						{
							buf->align(0);
							buf->write_stream(partdata->trans->u.frames.frames._1, sizeof(char) * 3, partdata->trans->size + 1);
						}
						else
						{
							buf->align(3);
							buf->write_stream(partdata->trans->u.frames.frames._2, sizeof(short) * 3, partdata->trans->size + 1);
						}
					}
				}
				else
				{
					buf->write_stream(partdata->trans->u.frame0, sizeof(float), 3);
				}
				zone_buffer::clear_pointer(&partdest->trans);
			}

			if (partdata->quat2)
			{
				buf->align(3);
				buf->write_stream(partdata->quat2, 8, 1); // not full struct

				if (partdata->quat2->size)
				{
					buf->write_stream(&partdata->quat2->u, 8, 1); // not full struct
					if (data->numframes >= 256)
					{
						buf->write_stream(&partdata->quat2->u.frames.indices, sizeof(short), partdata->quat2->size + 1);
					}
					else
					{
						buf->write_stream(&partdata->quat2->u.frames.indices, sizeof(char), partdata->quat2->size + 1);
					}

					if (partdata->quat2->u.frames.frames)
					{
						buf->align(3);
						buf->write_stream(partdata->quat2->u.frames.frames, sizeof(short) * 2, partdata->quat2->size + 1);
					}
				}
				else
				{
					buf->write_stream(partdata->quat2->u.frame0, sizeof(short) * 2, 1);
				}
				zone_buffer::clear_pointer(&partdest->quat2);
			}

			if (partdata->quat)
			{
				buf->align(3);
				buf->write_stream(partdata->quat, 8, 1);

				if (partdata->quat->size)
				{
					buf->write_stream(&partdata->quat->u, 8, 1); // not full struct

					if (data->numframes >= 256)
					{
						buf->write_stream(&partdata->quat->u.frames.indices, sizeof(short), partdata->quat->size + 1);
					}
					else
					{
						buf->write_stream(&partdata->quat->u.frames.indices, sizeof(char), partdata->quat->size + 1);
					}

					if (partdata->quat->u.frames.frames)
					{
						buf->align(3);
						buf->write_stream(partdata->quat->u.frames.frames, sizeof(short) * 4, partdata->quat->size + 1);
					}
				}
				else
				{
					buf->write_stream(partdata->quat->u.frame0, sizeof(short) * 4, 1);
				}
				zone_buffer::clear_pointer(&partdest->quat);
			}
		}

		if (data->dataByte) // dataByte
		{
			buf->align(0);
			buf->write_stream(data->dataByte, sizeof(char), data->dataByteCount);
		}
		if (data->dataShort) // dataShort
		{
			buf->align(1);
			buf->write_stream(data->dataShort, sizeof(short), data->dataShortCount);
		}
		if (data->dataInt) // dataInt
		{
			buf->align(3);
			buf->write_stream(data->dataInt, sizeof(int), data->dataIntCount);
		}
		if (data->randomDataShort) // randomDataShort
		{
			buf->align(1);
			buf->write_stream(data->randomDataShort, sizeof(short), data->randomDataShortCount);
		}
		if (data->randomDataByte) // randomDataByte
		{
			buf->align(0);
			buf->write_stream(data->randomDataByte, sizeof(char), data->randomDataByteCount);
		}
		if (data->randomDataInt) // randomDataInt
		{
			buf->align(3);
			buf->write_stream(data->randomDataInt, sizeof(int), data->randomDataIntCount);
		}

		// XAnim indice data
		if (data->indices.data)
		{
			if (data->numframes >= 256)
			{
				buf->align(1);
				buf->write_stream(data->indices.data, data->indexCount * 2, 1);
			}
			else
			{
				buf->align(0);
				buf->write_stream(data->indices.data, data->indexCount, 1);
			}
		}

		buf->pop_stream();
	}

	void xanim_parts::dump(XAnimParts* asset)
	{
		const auto path = "xanim\\"s + asset->name + ".xab";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		if (asset->names)
		{
			for (unsigned char bone = 0; bone < asset->boneCount[9]; bone++)
			{
				dump.dump_string(SL_ConvertToString(asset->names[bone]));
			}
		}

		if (asset->dataByte)
		{
			dump.dump_array(asset->dataByte, asset->dataByteCount);
		}
		if (asset->dataShort)
		{
			dump.dump_array(asset->dataShort, asset->dataShortCount);
		}
		if (asset->dataInt)
		{
			dump.dump_array(asset->dataInt, asset->dataIntCount);
		}
		if (asset->randomDataShort)
		{
			dump.dump_array(asset->randomDataShort, asset->randomDataShortCount);
		}
		if (asset->randomDataByte)
		{
			dump.dump_array(asset->randomDataByte, asset->randomDataByteCount);
		}
		if (asset->randomDataInt)
		{
			dump.dump_array(asset->randomDataInt, asset->randomDataIntCount);
		}

		if (asset->indices.data)
		{
			if (asset->numframes >= 256)
			{
				dump.dump_array(asset->indices._2, asset->indexCount);
			}
			else
			{
				dump.dump_array(asset->indices._1, asset->indexCount);
			}
		}

		if (asset->notify)
		{
			dump.dump_array(asset->notify, asset->notifyCount);
			for (unsigned char i = 0; i < asset->notifyCount; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->notify[i].name));
			}
		}

		if (asset->deltaPart)
		{
			dump.dump_single(asset->deltaPart);

			if (asset->deltaPart->trans)
			{
				auto extra_size = 0;

				if (asset->deltaPart->trans->size)
				{
					if (asset->numframes >= 256)
					{
						extra_size += (asset->deltaPart->trans->size * 2) + 2;
					}
					else
					{
						extra_size += asset->deltaPart->trans->size + 1;
					}
				}

				dump.dump_raw(asset->deltaPart->trans, sizeof(XAnimPartTrans) + extra_size);

				if (asset->deltaPart->trans->size)
				{
					if (asset->deltaPart->trans->u.frames.frames._1)
					{
						if (asset->deltaPart->trans->smallTrans)
						{
							dump.dump_raw(asset->deltaPart->trans->u.frames.frames._1, (3 * asset->deltaPart->trans->size) + 3);
						}
						else
						{
							dump.dump_raw(asset->deltaPart->trans->u.frames.frames._2, (6 * asset->deltaPart->trans->size) + 6);
						}
					}
				}
			}

			if (asset->deltaPart->quat2)
			{
				auto extra_size = 0;

				if (asset->deltaPart->quat2->size)
				{
					if (asset->numframes >= 256)
					{
						extra_size += (asset->deltaPart->quat2->size * 2) + 2;
					}
					else
					{
						extra_size += asset->deltaPart->quat2->size + 1;
					}
				}
				else
				{
					// no extra size required, quat2 data fits inside the frame0 buffer
					extra_size += 0;
				}

				dump.dump_raw(asset->deltaPart->quat2, sizeof(XAnimDeltaPartQuat2) + extra_size);

				if (asset->deltaPart->quat2->size && asset->deltaPart->quat2->u.frames.frames)
				{
					dump.dump_raw(asset->deltaPart->quat2->u.frames.frames, (asset->deltaPart->quat2->size * 4) + 4);
				}
			}

			if (asset->deltaPart->quat)
			{
				auto extra_size = 0;

				if (asset->deltaPart->quat->size)
				{
					if (asset->numframes >= 256)
					{
						extra_size += (asset->deltaPart->quat->size * 2) + 2;
					}
					else
					{
						extra_size += asset->deltaPart->quat->size + 1;
					}
				}
				else
				{
					// quat data contains 4 extra bytes
					extra_size += 4;
				}

				dump.dump_raw(asset->deltaPart->quat, sizeof(XAnimDeltaPartQuat) + extra_size);

				if (asset->deltaPart->quat->size && asset->deltaPart->quat->u.frames.frames)
				{
					dump.dump_raw(asset->deltaPart->quat->u.frames.frames, (asset->deltaPart->quat->size * 8) + 8);
				}
			}
		}

		dump.close();
	}
}