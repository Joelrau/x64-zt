#pragma once

namespace zonetool::s2
{
	class loaded_sound
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
	};
}
