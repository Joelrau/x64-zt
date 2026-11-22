#include <std_include.hpp>
#include "loadedsound.hpp"

namespace zonetool::iw3
{
	void loaded_sound::read(zone_reader32& reader, LoadedSound* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		reader.push_stream(XFILE_BLOCK_TEMP);
		reader.read_array(asset->sound.data, 0, asset->sound.info.data_len);
		reader.pop_stream();

		reader.pop_stream();
	}
}
