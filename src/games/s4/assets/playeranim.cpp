#include <std_include.hpp>
#include "playeranim.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_index_sets(zone_reader& reader, PlayerAnimIndexSet*& field, const int count)
		{
			if (const auto sets = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					reader.read_array(sets[i].indices0, 3, sets[i].count0);
					reader.read_array(sets[i].indices1, 3, sets[i].count1);
				}
			}
		}
	}

	void player_anim::read(zone_reader& reader, PlayerAnim* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_script_strings(entries[i].names, 3);
			}
		}

		read_index_sets(reader, asset->indexSets0, 0x80);
		read_index_sets(reader, asset->indexSets1, 0x5C);

		if (const auto entries = reader.read_array(asset->unknown1, 7, asset->unknown1Count))
		{
			for (auto i = 0; i < asset->unknown1Count; i++)
			{
				reader.read_array(entries[i].data, 3, entries[i].dataCount);
				reader.read_array(entries[i].pairs, 1, entries[i].pairCount);
			}
		}

		reader.read_array(asset->vectors, 3, asset->vectorCount);
		reader.read_array(asset->unknown3, 1, asset->unknown3Count);

		if (const auto entries = reader.read_array(asset->unknown4, 7, asset->unknown4Count))
		{
			for (auto i = 0; i < asset->unknown4Count; i++)
			{
				reader.read_array(entries[i].values, 1, entries[i].count);
			}
		}

		reader.read_array(asset->unknown5, 1, asset->unknown5Count);
		reader.read_array(asset->unknown6, 1, asset->unknown6Count);
		reader.read_array(asset->entryValues0, 1, asset->entryCount);
		reader.read_array(asset->unknown7, 1, asset->unknown7Count);
		reader.read_array(asset->entryValues1, 1, asset->entryCount);
		reader.read_array(asset->unknown8, 1, asset->unknown8Count);
		reader.read_asset_array(ASSET_TYPE_XANIMPARTS, asset->anims, asset->animCount);
		reader.pop_stream();
	}
}
