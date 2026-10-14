#pragma once

namespace zonetool::s2
{
	std::optional<std::string> read_shared_stream(const XSurfaceShared* shared);

	class xsurface_shared
	{
	public:
		static void read(zone_reader& reader, XSurfaceShared* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);

			if (asset->flags & XSURFACE_SHARED_STREAMED)
			{
				reader.push_stream(XFILE_BLOCK_RUNTIME_UNREAD);
				reader.read_array(asset->data, 255, asset->dataSize);
				reader.pop_stream();
			}
			else
			{
				reader.read_array(asset->data, 255, asset->dataSize);
			}

			reader.read_stream(asset->subdivData, 15, 48, asset->subdivCount);
			reader.pop_stream();
		}
	};
}
