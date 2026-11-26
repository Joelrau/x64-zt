#include <std_include.hpp>
#include "gamemap.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_data(auto& reader, GameMapData& data)
		{
			const auto entity_count = static_cast<std::size_t>(data.entityCount + 0x100u);
			if (const auto entities = reader.read_array(data.entities, 7, entity_count))
			{
				for (auto i = 0u; i < entity_count; i++)
				{
					reader.read_array(entities[i].values, 3, entities[i].valueCount);
				}
			}

			reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
			reader.read_array(data.runtimeData, 15, entity_count);
			reader.pop_stream();

			reader.read_array(data.data, 0, data.dataSize);

			const auto link_count = static_cast<std::size_t>(data.linkCount);
			if (const auto links = reader.read_array(data.links, 7, link_count))
			{
				for (auto i = 0u; i < link_count; i++)
				{
					if (links[i].type < 0)
					{
						auto& values = links[i].data.values;
						reader.read_array(values.values, 1, static_cast<std::size_t>(values.count));
					}
				}
			}
		}
	}

	void game_map::read(auto& reader, GameMap* asset)
	{
		read_data(reader, asset->data);
		reader.read_array(asset->grid1, 7);
		reader.read_array(asset->grid2, 7);
	}
}

namespace zonetool::t9
{
	template void game_map::read(zone_reader&, GameMap*);
	template void game_map::read(zone_writer&, GameMap*);
}
