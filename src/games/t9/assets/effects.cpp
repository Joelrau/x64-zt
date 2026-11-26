#include <std_include.hpp>
#include "effects.hpp"

namespace zonetool::t9
{
	void read_value_array(auto& reader, ValueArray& array)
	{
		reader.read_array(array.values, 3, array.count);
	}

	void tag_fx::read(auto& reader, TagFx* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		if (!entries)
		{
			return;
		}

		for (auto i = 0; i < asset->entryCount; i++)
		{
			reader.read_asset(ASSET_TYPE_FX, entries[i].effect);
			for (auto& effect : entries[i].tagEffects)
			{
				reader.read_asset(ASSET_TYPE_FX, effect);
			}
		}
	}

	void beam::read(auto& reader, Beam* asset)
	{
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material0);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material1);

		for (auto* values : {&asset->values0, &asset->values1, &asset->values2, &asset->values3, &asset->values4,
			&asset->values5, &asset->values6, &asset->values7, &asset->values8, &asset->values9, &asset->values10,
			&asset->values11, &asset->values12})
		{
			read_value_array(reader, *values);
		}

		for (auto& effect : asset->effects)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}

		for (auto* values : {&asset->values13, &asset->values14, &asset->values15, &asset->values16})
		{
			read_value_array(reader, *values);
		}
	}

	void tracer::read(auto& reader, Tracer* asset)
	{
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.read_asset(ASSET_TYPE_BEAM, asset->beam);
		read_value_array(reader, asset->values);
	}

	void laser::read(auto& reader, Laser* asset)
	{
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
		reader.read_asset(ASSET_TYPE_FX, asset->effect);
	}

	void flame_table::read(auto& reader, FlameTable* asset)
	{
		for (auto& material : asset->materials)
		{
			reader.read_asset(ASSET_TYPE_MATERIAL, material);
		}
	}

	void ballistic_desc::read(auto&, BallisticDesc*)
	{
	}

	void trigger_effect_desc::read(auto&, TriggerEffectDesc*)
	{
	}

	void shell_shock::read(auto& reader, ShellShock* asset)
	{
		reader.read_string(asset->string);
	}

	void status_effect::read(auto& reader, StatusEffect* asset)
	{
		reader.read_asset(ASSET_TYPE_SHELLSHOCK, asset->shellShock);

		auto& data = asset->data;
		reader.read_asset(ASSET_TYPE_POSTFXBUNDLE, data.postFxBundle0);
		reader.read_asset(ASSET_TYPE_POSTFXBUNDLE, data.postFxBundle1);
		reader.read_asset(ASSET_TYPE_TAGFX, data.tagFx0);
		reader.read_asset(ASSET_TYPE_TAGFX, data.tagFx1);
		reader.read_asset(ASSET_TYPE_TAGFX, data.tagFx2);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, data.scriptBundle);
		reader.read_asset(ASSET_TYPE_RUMBLE, data.rumble);
		reader.read_asset(ASSET_TYPE_GESTURE, data.gesture);
		reader.read_asset(ASSET_TYPE_GESTURETABLE, data.gestureTable);

		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_FX, asset->fx.effect0);
		reader.read_asset(ASSET_TYPE_FX, asset->fx.effect1);
		read_value_array(reader, asset->values);
	}

	void vehicle_fx_def::read(auto& reader, VehicleFxDef* asset)
	{
		for (auto& effect : asset->effects0)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}

		for (auto& effect : asset->effects1)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}

		for (auto& effect : asset->effects2)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}
	}
}

namespace zonetool::t9
{
	template void read_value_array(zone_reader&, ValueArray&);
	template void tag_fx::read(zone_reader&, TagFx*);
	template void beam::read(zone_reader&, Beam*);
	template void tracer::read(zone_reader&, Tracer*);
	template void laser::read(zone_reader&, Laser*);
	template void flame_table::read(zone_reader&, FlameTable*);
	template void ballistic_desc::read(zone_reader&, BallisticDesc*);
	template void trigger_effect_desc::read(zone_reader&, TriggerEffectDesc*);
	template void shell_shock::read(zone_reader&, ShellShock*);
	template void status_effect::read(zone_reader&, StatusEffect*);
	template void vehicle_fx_def::read(zone_reader&, VehicleFxDef*);
	template void read_value_array(zone_writer&, ValueArray&);
	template void tag_fx::read(zone_writer&, TagFx*);
	template void beam::read(zone_writer&, Beam*);
	template void tracer::read(zone_writer&, Tracer*);
	template void laser::read(zone_writer&, Laser*);
	template void flame_table::read(zone_writer&, FlameTable*);
	template void ballistic_desc::read(zone_writer&, BallisticDesc*);
	template void trigger_effect_desc::read(zone_writer&, TriggerEffectDesc*);
	template void shell_shock::read(zone_writer&, ShellShock*);
	template void status_effect::read(zone_writer&, StatusEffect*);
	template void vehicle_fx_def::read(zone_writer&, VehicleFxDef*);
}
