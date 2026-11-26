#include <std_include.hpp>
#include "vehicle.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_parts(auto& reader, VehicleAssemblyPart*& field, const std::size_t count)
		{
			if (const auto parts = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_asset(ASSET_TYPE_XMODEL, parts[i].model);
					reader.read_asset(ASSET_TYPE_DESTRUCTIBLEDEF, parts[i].destructibleDef);
					reader.read_array(parts[i].values, 3, parts[i].valueCount);
				}
			}
		}

		void read_collision(auto& reader, VehicleCollision*& field)
		{
			if (const auto collision = reader.read_single(field, 7))
			{
				if (const auto mesh = reader.read_single(collision->mesh, 7))
				{
					read_clip_map_mesh(reader, *mesh);
				}

				if (const auto table = reader.read_single(collision->table, 7))
				{
					read_clip_map_table(reader, *table);
				}
			}
		}
	}

	void vehicle_sound_def::read(auto& reader, VehicleSoundDef* asset)
	{
		reader.read_string(asset->string);

		for (auto& alias : asset->aliases)
		{
			reader.read_string(alias.name);
		}

		for (auto& event : asset->events)
		{
			reader.read_string(event.name);

			if (const auto params = reader.read_single(event.params, 7))
			{
				reader.read_string(params->name);
			}
		}
	}

	void vehicle_assembly::read(auto& reader, VehicleAssembly* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle);

		if (const auto pairs = reader.read_array(asset->imagePairs, 7, asset->imagePairCount))
		{
			for (auto i = 0u; i < asset->imagePairCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, pairs[i].image0);
				reader.read_asset(ASSET_TYPE_IMAGE, pairs[i].image1);
			}
		}

		read_parts(reader, asset->parts0, asset->partCount0);
		reader.read_array(asset->indices0, 1, asset->indexCount0);
		read_parts(reader, asset->parts1, asset->partCount1);
		reader.read_array(asset->indices1, 1, asset->indexCount1);
		read_parts(reader, asset->parts2, asset->partCount2);

		if (const auto sets = reader.read_array(asset->materialSets, 7, asset->materialSetCount))
		{
			for (auto i = 0u; i < asset->materialSetCount; i++)
			{
				reader.read_array(sets[i].indices, 1, sets[i].indexCount);
				reader.read_asset_array(ASSET_TYPE_MATERIAL, sets[i].materials, sets[i].materialCount);
			}
		}

		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materials, asset->materialCount);
		reader.read_array(asset->tags, 3, asset->tagCount);

		if (const auto lists = reader.read_array(asset->modelLists, 7, asset->modelListCount))
		{
			for (auto i = 0u; i < asset->modelListCount; i++)
			{
				reader.read_asset_array(ASSET_TYPE_XMODEL, lists[i].models, lists[i].modelCount);
			}
		}
	}

	void vehicle::read(auto& reader, Vehicle* asset)
	{
		for (auto& values : asset->values0)
		{
			read_value_array(reader, values);
		}

		read_value_array(reader, asset->values1);

		for (auto& tire : asset->tireFx)
		{
			reader.read_asset(ASSET_TYPE_FX, tire.effect);
		}

		for (auto& surface : asset->surfaceFx)
		{
			reader.read_asset(ASSET_TYPE_FX, surface.effect);
			reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, surface.surfaceFxTable);
		}

		for (auto& entry : asset->bundleEntries)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, entry.scriptBundle);
		}

		reader.read_asset(ASSET_TYPE_CINEMATICMOTION, asset->cinematicMotion);

		for (auto& sound_def : asset->soundDefs)
		{
			reader.read_asset(ASSET_TYPE_VEHICLESOUNDDEF, sound_def);
		}

		reader.read_string(asset->string0);
		reader.read_array(asset->seats, 3, asset->seatCount);

		for (auto& model : asset->models)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}

		for (auto& collision : asset->collisions)
		{
			read_collision(reader, collision);
		}

		reader.read_asset(ASSET_TYPE_FX, asset->effect0);
		reader.read_asset(ASSET_TYPE_FX, asset->effect1);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->rumble0);
		reader.read_asset(ASSET_TYPE_FX, asset->effect2);
		reader.read_asset(ASSET_TYPE_VEHICLEFXDEF, asset->fxDef);

		for (auto& fx_def : asset->fxDefs)
		{
			reader.read_asset(ASSET_TYPE_VEHICLEFXDEF, fx_def);
		}

		reader.read_asset(ASSET_TYPE_FX, asset->effect3);
		reader.read_asset(ASSET_TYPE_FX, asset->effect4);
		reader.read_asset(ASSET_TYPE_FX, asset->effect5);

		for (const auto effects : {asset->effects0, asset->effects1, asset->effects2, asset->effects3})
		{
			for (auto i = 0; i < 4; i++)
			{
				reader.read_asset(ASSET_TYPE_FX, effects[i]);
			}
		}

		reader.read_string(asset->string1);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->rumble1);
		reader.read_string(asset->string2);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image0);

		for (auto& string : asset->strings)
		{
			reader.read_string(string.name);
		}

		reader.read_string(asset->string3);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->rumble2);

		for (auto& rumble : asset->rumbles)
		{
			reader.read_asset(ASSET_TYPE_RUMBLE, rumble);
		}

		read_value_array(reader, asset->values2);

		for (auto& bundle : asset->scriptBundles)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		reader.read_asset(ASSET_TYPE_ANIMSTATEMACHINE, asset->animStateMachine);
		reader.read_asset(ASSET_TYPE_ANIMSELECTORTABLE, asset->animSelectorTable);
		reader.read_asset(ASSET_TYPE_ANIMMAPPINGTABLE, asset->animMappingTable);
		reader.read_asset(ASSET_TYPE_SURFACESOUNDDEF, asset->surfaceSoundDef);
		reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, asset->surfaceFxTable);
		reader.read_asset(ASSET_TYPE_DESTRUCTIBLEDEF, asset->destructibleDef);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image1);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image2);

		for (auto& values : asset->values3)
		{
			read_value_array(reader, values);
		}

		for (auto& values : asset->values4)
		{
			read_value_array(reader, values);
		}

		read_script_bundle_object(reader, asset->bundle);
		reader.read_asset(ASSET_TYPE_VEHICLEASSEMBLY, asset->assembly);
	}
}

namespace zonetool::t9
{
	template void vehicle_sound_def::read(zone_reader&, VehicleSoundDef*);
	template void vehicle_assembly::read(zone_reader&, VehicleAssembly*);
	template void vehicle::read(zone_reader&, Vehicle*);
	template void vehicle_sound_def::read(zone_writer&, VehicleSoundDef*);
	template void vehicle_assembly::read(zone_writer&, VehicleAssembly*);
	template void vehicle::read(zone_writer&, Vehicle*);
}
