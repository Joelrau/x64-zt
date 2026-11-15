#include <std_include.hpp>
#include "streaminginfo.hpp"

namespace zonetool::iw8
{
	namespace
	{
		unsigned int image_flag_words(const unsigned int image_count)
		{
			return (4 * image_count + 31) >> 5;
		}

		void read_flags(zone_reader& reader, AlwaysloadedFlags& flags)
		{
			reader.read_asset_array(ASSET_TYPE_IMAGE, flags.images, flags.imageCount);
			reader.read_array(flags.imageFlags, 3, image_flag_words(flags.imageCount));
		}

		void read_flag_set(zone_reader& reader, AlwaysloadedFlagSet& set)
		{
			read_flags(reader, set.globalFlags);

			if (const auto models = reader.read_array(set.perModelFlags, 7, set.modelCount))
			{
				for (auto i = 0u; i < set.modelCount; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, models[i].model);
					read_flags(reader, models[i].flags);
				}
			}

			reader.read_asset_array(ASSET_TYPE_XMODEL_SURFS, set.modelSurfs, set.modelSurfCount);
		}

		void read_transient_sets(zone_reader& reader, TransientSets& sets)
		{
			if (const auto names = reader.read_array(sets.names, 7, sets.count))
			{
				for (auto i = 0u; i < sets.count; i++)
				{
					reader.read_string(names[i]);
				}
			}

			reader.read_array(sets.masks, 3, sets.count);
		}

		void read_transient_costs(zone_reader& reader, TransientCosts& costs)
		{
			const auto count = costs.transientCostCount;
			reader.read_array(costs.streamingCosts, 3, costs.streamingItemCount);

			auto& set = costs.streamContents;
			reader.read_array(set.contentCounts, 1, count);

			if (const auto contents = reader.read_array(set.contents, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_array(contents[i].contents, 1, set.contentCounts[i]);
				}
			}

			reader.read_array(costs.residentPageSizes, 1, count);
			reader.read_array(costs.tempPageSizes, 1, count);
		}

		template <typename T>
		void read_named_references(zone_reader& reader, T*& field, const unsigned int count)
		{
			if (const auto references = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_string(references[i].name);
				}
			}
		}

		void read_transient_info(zone_reader& reader, TransientInfo& info)
		{
			read_transient_sets(reader, info.transientSets);
			read_transient_costs(reader, info.transientCosts);
			read_named_references(reader, info.forcedStubAssets, info.forcedStubAssetCount);
			read_named_references(reader, info.transientFiles, info.transientFileCount);
			read_named_references(reader, info.transientAssets, info.transientAssetCount);

			if (const auto maps = reader.read_array(info.transientWorldGridTileMaps, 7, info.transientWorldGridTileMapCount))
			{
				for (auto i = 0u; i < info.transientWorldGridTileMapCount; i++)
				{
					reader.read_array(maps[i].spaceToTransientIndex, 1, maps[i].indexCount);
				}
			}

			read_spatial_tree(reader, info.transientWorldGridSpatial);
			reader.read_array(info.transientWorldGridBounds, 3, info.transientWorldGridBoundsCount);
			reader.read_array(info.transientWorldGridHighLodLookup, 1, info.transientWorldGridHighLodLookupCount);
			reader.read_array(info.transientSkipLoading.array, 3, info.transientSkipLoading.wordCount);
		}
	}

	void streaming_info::read(zone_reader& reader, StreamingInfo* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_flag_set(reader, asset->alwaysloadedFlagSet);

		if (const auto info = reader.read_single(asset->transientInfo, 7))
		{
			read_transient_info(reader, *info);
		}
		reader.pop_stream();
	}
}
