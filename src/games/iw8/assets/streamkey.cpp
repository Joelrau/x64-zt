#include <std_include.hpp>
#include "streamkey.hpp"

namespace zonetool::iw8
{
	void stream_key::read(zone_reader& reader, StreamKey* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (asset->flags & STREAM_KEY_RESIDENT)
		{
			reader.read_array(asset->data.residentData, 15, asset->dataSize);
		}
		reader.pop_stream();
	}
}
