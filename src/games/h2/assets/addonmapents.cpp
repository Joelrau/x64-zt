#include <std_include.hpp>
#include "addonmapents.hpp"

namespace zonetool::h2
{
	void addon_map_ents::read(zone_reader& reader, AddonMapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		clip_map::read_triggers(reader, &asset->trigger);

		if (const auto info = reader.read_single(asset->info, 3))
		{
			clip_map::read_info(reader, info);
		}

		if (const auto models = reader.read_array(asset->cmodels, 3, asset->numSubModels))
		{
			for (auto i = 0u; i < asset->numSubModels; i++)
			{
				if (const auto info = reader.read_single(models[i].info, 3))
				{
					clip_map::read_info(reader, info);
				}
			}
		}

		reader.read_array(asset->models, 3, asset->numSubModels);
		reader.read_array(asset->physModels, 3, asset->numSubModels);

		if (const auto polytopes = reader.read_array(asset->polytope, 3, asset->polytopeCount))
		{
			for (auto i = 0u; i < asset->polytopeCount; i++)
			{
				phys_collmap::read_polytope(reader, &polytopes[i]);
			}
		}

		if (const auto meshes = reader.read_array(asset->meshData, 3, asset->meshDataCount))
		{
			for (auto i = 0u; i < asset->meshDataCount; i++)
			{
				phys_world::read_mesh(reader, &meshes[i]);
			}
		}

		reader.pop_stream();
	}

	void addon_map_ents::dump(AddonMapEnts* asset)
	{
		auto file = filesystem::file(asset->name);
		file.open("wb");
		if (file.get_fp())
		{
			file.write(asset->entityString, asset->numEntityChars, 1);
			file.close();
		}

		assetmanager::dumper dumper;
		if (dumper.open(asset->name + ".triggers"s))
		{
			dumper.dump_int(asset->trigger.count);
			dumper.dump_array(asset->trigger.models, asset->trigger.count);
			dumper.dump_int(asset->trigger.hullCount);
			dumper.dump_array(asset->trigger.hulls, asset->trigger.hullCount);
			dumper.dump_int(asset->trigger.slabCount);
			dumper.dump_array(asset->trigger.slabs, asset->trigger.slabCount);
			dumper.close();
		}
	}
}
