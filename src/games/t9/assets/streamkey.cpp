#include <std_include.hpp>
#include "streamkey.hpp"

namespace zonetool::t9
{
	void stream_key::read(auto& reader, StreamKey* asset)
	{
		if (asset->flags & STREAM_KEY_FLAG_RESIDENT)
		{
			reader.read_array(asset->data, 0xFF, asset->size);
			return;
		}

		const auto cpu = (asset->flags & STREAM_KEY_FLAG_CPU) != 0;
		const auto page_aligned = cpu && (asset->flags & STREAM_KEY_FLAG_PAGE_ALIGNED);

		reader.push_stream(cpu ? XFILE_BLOCK_STREAMER_CPU : XFILE_BLOCK_STREAMER);
		reader.read_array(asset->data, page_aligned ? 0xFFFF : 0xFF, asset->size);
		reader.pop_stream();
	}
}

namespace zonetool::t9
{
	template void stream_key::read(zone_reader&, StreamKey*);
	template void stream_key::read(zone_writer&, StreamKey*);
}
