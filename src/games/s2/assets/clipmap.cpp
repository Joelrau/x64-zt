#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_brushes(zone_reader& reader, ClipBrushInfo* brushes)
		{
			if (const auto sides = reader.read_array(brushes->sides, 7, brushes->sideCount))
			{
				for (auto i = 0u; i < brushes->sideCount; i++)
				{
					if (sides[i].indexCount > 0)
					{
						reader.read_array(sides[i].indices, 3, sides[i].indexCount);
					}
				}
			}

			reader.read_array(brushes->indices, 3, brushes->indexCount);
		}

		void read_shared_info(zone_reader& reader, ClipInfo*& field)
		{
			reader.push_stream(XFILE_BLOCK_TEMP);
			if (const auto info = reader.read_single(field, 7))
			{
				clip_map::read_info(reader, info);
			}
			reader.pop_stream();
		}

		void read_dyn_entity_defs(zone_reader& reader, DynEntityDef*& field, const std::uint16_t count)
		{
			if (const auto defs = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, defs[i].xModel);
					reader.read_asset(ASSET_TYPE_FX, defs[i].destroyFx);
					reader.read_asset(ASSET_TYPE_SOUND, defs[i].sound);
					reader.read_asset(ASSET_TYPE_PHYSPRESET, defs[i].physPreset);
					reader.read_array(defs[i].hinge, 3);
					reader.read_array(defs[i].linkTo, 3);
				}
			}
		}

		template <typename T>
		void read_runtime_lists(zone_reader& reader, T* (&lists)[2], const std::size_t alignment, const std::uint16_t* counts)
		{
			for (auto i = 0; i < 2; i++)
			{
				reader.push_stream(XFILE_BLOCK_RUNTIME);
				reader.read_array(lists[i], alignment, counts[i]);
				reader.pop_stream();
			}
		}
	}

	void clip_map::read_info(zone_reader& reader, ClipInfo* info)
	{
		reader.read_array(info->planes, 3, info->planeCount);

		if (const auto materials = reader.read_array(info->materials, 7, info->numMaterials))
		{
			for (auto i = 0u; i < info->numMaterials; i++)
			{
				reader.read_string(materials[i].name);
			}
		}

		read_brushes(reader, &info->brushes);
		reader.read_array(info->brushBounds.data, 15, info->brushBounds.count);
		reader.read_array(info->leafBrushes.data, 3, info->leafBrushes.count);

		for (auto& tr_zone : info->trZones)
		{
			reader.read_asset(ASSET_TYPE_CLIPMAP_TRANSIENT_ZONE, tr_zone);
		}
	}

	void clip_map::read(zone_reader& reader, clipMap_t* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		read_info(reader, &asset->info);
		read_shared_info(reader, asset->pInfo);

		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->numNodes))
		{
			for (auto i = 0u; i < asset->numNodes; i++)
			{
				reader.read_single(nodes[i].plane, 3);
			}
		}

		reader.read_array(asset->leafs, 7, asset->numLeafs);

		if (const auto cmodels = reader.read_array(asset->cmodels, 7, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				read_shared_info(reader, cmodels[i].info);
			}
		}

		reader.read_asset(ASSET_TYPE_MAP_ENTS, asset->mapEnts);

		if (const auto names = reader.read_array(asset->staticModelNames, 7, asset->staticModelNameCount))
		{
			for (auto i = 0; i < asset->staticModelNameCount; i++)
			{
				reader.read_string(names[i].name);
			}
		}

		auto& static_models = asset->staticModelData;
		reader.read_array(static_models.nodes, 7, static_models.nodeCount);
		reader.read_array(static_models.leafs, 7, static_models.leafCount);
		reader.read_array(static_models.bounds, 3, static_models.boundsCount);

		read_dyn_entity_defs(reader, asset->dynEntDefList[0], asset->dynEntCount[0]);
		read_dyn_entity_defs(reader, asset->dynEntDefList[1], asset->dynEntCount[1]);
		read_runtime_lists(reader, asset->dynEntPoseList, 3, asset->dynEntCount);
		read_runtime_lists(reader, asset->dynEntClientList, 7, asset->dynEntCount);
		read_runtime_lists(reader, asset->dynEntCollList, 3, asset->dynEntCount);

		reader.read_script_string_array(asset->scrStrings, asset->scrStringCount);

		if (const auto entries = reader.read_array(asset->scrEntries, 3, asset->scrEntryCount))
		{
			for (auto i = 0u; i < asset->scrEntryCount; i++)
			{
				reader.read_script_string(entries[i].name);
			}
		}

		if (const auto mayhems = reader.read_array(asset->mayhems, 7, asset->mayhemCount))
		{
			for (auto i = 0u; i < asset->mayhemCount; i++)
			{
				reader.read_asset(ASSET_TYPE_MAYHEM, mayhems[i].mayhem);
				reader.read_array(mayhems[i].transform, 3);
				reader.read_script_string(mayhems[i].name);
			}
		}

		reader.pop_stream();
	}
}
