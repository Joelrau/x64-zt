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
		}

		LoadedSound* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_LOADED_SOUND, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing loadedsound \"%s\"...", name.data());

			auto* asset = read.read_single<LoadedSound>();
			asset->name = read.read_string();
			asset->info.name = read.read_string();
			asset->info.fileName = read.read_string();
			asset->data = read.read_array<char>();
			asset->seekTable = read.read_array<unsigned int>();
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

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
		}

		static void dump(LoadedSound* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_LOADED_SOUND, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->info.name);
			write.dump_string(asset->info.fileName);
			write.dump_array(asset->data, asset->dataSize);
			write.dump_array(asset->seekTable, asset->seekTableCount);
			write.close();
		}
	};
}
