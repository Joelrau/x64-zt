#include <std_include.hpp>
#include "dynentitylist.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_dyn_entity_defs(zone_reader& reader, DynEntityDef*& field, const unsigned int count)
		{
			if (const auto defs = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, defs[i].baseModel);
					reader.read_single(defs[i].linkTo, 3);
				}
			}
		}

		void read_transient_population(zone_reader& reader, SpatialPartition_TransientPopulation*& field)
		{
			if (const auto population = reader.read_single(field, 7))
			{
				reader.read_array(population->buckets, 3, population->partitionCount);
				reader.read_array(population->bucketTails, 3, population->partitionCount);
				reader.read_array(population->partitionIds, 3, population->partitionCount);
			}
		}
	}

	void dyn_entity_list::read(zone_reader& reader, DynEntityList* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_dyn_entity_defs(reader, asset->dynEntDefList[0], asset->dynEntCount[0]);
		read_dyn_entity_defs(reader, asset->dynEntDefList[1], asset->dynEntCount[1]);

		for (auto& population : asset->dynEntSpatialPopulation)
		{
			read_transient_population(reader, population);
		}
		reader.pop_stream();
	}
}
