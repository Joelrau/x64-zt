#pragma once

namespace zonetool::s2
{
	class loaded_sound : public asset_base<LoadedSound, ASSET_TYPE_LOADED_SOUND>
	{
	public:
		static void read_stream_info(zone_reader& reader, LoadedSoundStreamInfo& info)
		{
			reader.read_string(info.name);
			reader.read_string(info.fileName);
		}

		static void read(zone_reader& reader, LoadedSound* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			read_stream_info(reader, asset->info);
			reader.read_array(asset->data, 0, asset->dataSize);
			reader.read_array(asset->seekTable, 3, asset->seekTableCount);
			reader.pop_stream();
			asset->info.streamKey = reader.next_shared_stream_key();
		}

		LoadedSound* parse(const std::string& name, zone_memory* mem) override;

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);
			dest->info.streamKey = {};

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (data->info.name)
			{
				dest->info.name = buf->write_str(data->info.name);
			}
			if (data->info.fileName)
			{
				dest->info.fileName = buf->write_str(data->info.fileName);
			}
			if (data->data)
			{
				buf->align(0);
				buf->write(data->data, data->dataSize);
				buf->clear_pointer(&dest->data);
			}
			if (data->seekTable)
			{
				buf->align(3);
				buf->write(data->seekTable, data->seekTableCount);
				buf->clear_pointer(&dest->seekTable);
			}

			buf->pop_stream();
			buf->write_shared_stream_key(data->info.streamKey);
		}

		static void dump(LoadedSound* asset);

		template <typename J>
		static void visit_stream_key_json(const J& j, XStreamFileKey& value)
		{
			j.value("hash", value.hash);
			j.value("index", value.index);
			j.value("unknown", value.unknown);
			j.value("slot", value.slot);
		}

		template <typename J>
		static void visit_stream_info_json(const J& j, LoadedSoundStreamInfo& value)
		{
			j.value("unknown0", value.unknown0);
			j.string("name", value.name);
			j.string("fileName", value.fileName);
			j.object("streamKey", value.streamKey, visit_stream_key_json<J>);
		}

		template <typename J>
		static void visit_audio_format_json(const J& j, SndAudioFormat& value)
		{
			j.value("dataLength", value.dataLength);
			j.value("sampleCount", value.sampleCount);
			j.value("sampleRate", value.sampleRate);
			j.value("channelCount", value.channelCount);
			j.value("bitsPerSample", value.bitsPerSample);
			j.value("blockSize", value.blockSize);
			j.value("format", value.format);
			j.value("unknown15", value.unknown15);
		}
	};
}
