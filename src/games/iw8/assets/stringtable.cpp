#include <std_include.hpp>
#include "stringtable.hpp"

namespace zonetool::iw8
{
	void string_table::read(zone_reader& reader, StringTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->cellIndices, 1, asset->columnCount * asset->rowCount);
		reader.read_array(asset->hashes, 3, asset->uniqueCellCount);

		if (const auto strings = reader.read_array(asset->strings, 7, asset->uniqueCellCount))
		{
			for (auto i = 0; i < asset->uniqueCellCount; i++)
			{
				reader.read_string(strings[i]);
			}
		}
		reader.pop_stream();
	}
}
