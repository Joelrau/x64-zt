#include <std_include.hpp>
#include "codcasterdata.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_image_entry(zone_reader& reader, CodcasterImageEntry& entry)
		{
			reader.read_string(entry.name);
			reader.read_asset(ASSET_TYPE_IMAGE, entry.image);
		}
	}

	void codcaster_data::read(zone_reader& reader, CodcasterData* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto images = reader.read_array(asset->images, 7, asset->imageCount))
		{
			for (auto i = 0u; i < asset->imageCount; i++)
			{
				read_image_entry(reader, images[i]);
			}
		}

		read_image_entry(reader, asset->unknownImage);
		if (const auto pairs = reader.read_array(asset->stringPairs, 7, asset->stringPairCount))
		{
			for (auto i = 0u; i < asset->stringPairCount; i++)
			{
				reader.read_string(pairs[i].key);
				reader.read_string(pairs[i].value);
			}
		}

		reader.pop_stream();
	}
}
