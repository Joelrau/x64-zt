#include <std_include.hpp>
#include "streamerworld.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_bit_grid(auto& reader, StreamerBitGrid& grid)
		{
			reader.read_array(grid.cells, 3, grid.cellCount);

			const auto bit_size = grid.bitRows ? ((grid.bitRows - 1) * grid.bitStride + 0x40) >> 3 : 0u;
			reader.read_array(grid.bits, 0, bit_size);

			reader.read_array(grid.pairs, 3, grid.pairCount);
			reader.read_array(grid.triples, 3, grid.tripleCount);
		}

		void read_section(auto& reader, StreamerSection& section)
		{
			if (const auto keys = reader.read_array(section.keys, 7, section.keyCount))
			{
				for (auto i = 0ull; i < section.keyCount; i++)
				{
					reader.read_asset(ASSET_TYPE_STREAMKEY, keys[i].streamKey);
				}
			}

			read_bit_grid(reader, section.bitGrid);

			if (const auto value_lists = reader.read_array(section.valueLists, 7))
			{
				if (const auto lists = reader.read_array(value_lists->lists, 7, value_lists->listCount))
				{
					for (auto i = 0ull; i < value_lists->listCount; i++)
					{
						reader.read_array(lists[i].values, 3, lists[i].valueCount);
					}
				}
			}

			reader.read_asset_array(ASSET_TYPE_MATERIAL, section.materials, section.materialCount);
			reader.read_asset_array(ASSET_TYPE_IMAGE, section.images, section.imageCount);
		}

		void read_model_set(auto& reader, StreamerModelSet& set)
		{
			if (const auto models = reader.read_array(set.models, 7, set.modelCount))
			{
				for (auto i = 0u; i < set.modelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
				}
			}

			if (const auto lods = reader.read_array(set.lods, 7, 8))
			{
				for (auto i = 0; i < 8; i++)
				{
					if (const auto groups = reader.read_array(lods[i].groups, 7, lods[i].groupCount))
					{
						for (auto j = 0u; j < lods[i].groupCount; j++)
						{
							reader.read_array(groups[j].values, 3, groups[j].valueCount);
						}
					}
				}
			}

			reader.read_array(set.modelData, 3, set.modelCount);
			reader.read_asset(ASSET_TYPE_STREAMKEY, set.streamKey);
		}
	}

	void streamer_world::read(auto& reader, StreamerWorld* asset)
	{
		if (const auto groups = reader.read_array(asset->groups, 7, asset->groupCount))
		{
			for (auto i = 0ull; i < asset->groupCount; i++)
			{
				if (const auto lists = reader.read_array(groups[i].lists, 7, groups[i].listCount))
				{
					for (auto j = 0ull; j < groups[i].listCount; j++)
					{
						reader.read_array(lists[j].ranges, 3, lists[j].rangeCount);
					}
				}
			}
		}

		if (const auto pair_lists = reader.read_array(asset->pairLists, 7, asset->pairListCount))
		{
			for (auto i = 0ull; i < asset->pairListCount; i++)
			{
				reader.read_array(pair_lists[i].pairs, 3, pair_lists[i].pairCount);
			}
		}

		reader.read_array(asset->values1, 7, asset->values1Count);
		reader.read_array(asset->values2, 7, asset->values2Count);

		if (const auto cells = reader.read_array(asset->cells, 7, asset->cellCount))
		{
			for (auto i = 0ull; i < asset->cellCount; i++)
			{
				read_section(reader, cells[i].section);
				read_model_set(reader, cells[i].modelSet);
			}
		}

		reader.read_array(asset->values3, 7, asset->values3Count);
		reader.read_array(asset->values4, 7, asset->values4Count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->models, asset->modelCount);
		reader.read_array(asset->data, 0, asset->dataSize);
		reader.read_asset(ASSET_TYPE_TERRAINGFX, asset->terrainGfx);
		reader.read_asset(ASSET_TYPE_LIGHTING, asset->lighting);
	}

	void streamer_hint::read(auto& reader, StreamerHint* asset)
	{
		reader.read_array(asset->records, 7, asset->recordCount);
		if (const auto groups = reader.read_array(asset->groups, 7, asset->groupCount))
		{
			for (auto i = 0; i < asset->groupCount; i++)
			{
				reader.read_array(groups[i].entries, 3, groups[i].entryCount);
				reader.read_array(groups[i].indices, 1, groups[i].indexCount);
				reader.read_array(groups[i].values, 3, groups[i].valueCount);
			}
		}
	}
}

namespace zonetool::t9
{
	template void streamer_world::read(zone_reader&, StreamerWorld*);
	template void streamer_world::read(zone_writer&, StreamerWorld*);
	template void streamer_hint::read(zone_reader&, StreamerHint*);
	template void streamer_hint::read(zone_writer&, StreamerHint*);
}
