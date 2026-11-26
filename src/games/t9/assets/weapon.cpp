#include <std_include.hpp>
#include "weapon.hpp"

namespace zonetool::t9
{
	namespace
	{
		template <typename T, std::size_t N>
		void read_assets(auto& reader, const std::int32_t type, T* (&assets)[N])
		{
			for (auto& asset : assets)
			{
				reader.read_asset(type, asset);
			}
		}

		void read_weapon_value_tables(auto& reader, WeaponValueTables& tables)
		{
			read_value_array(reader, tables.values0);
			read_value_array(reader, tables.values1);
		}

		template <typename T>
		void read_pointer_array(auto& reader, T**& field, const std::int32_t count)
		{
			if (const auto items = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					reader.read_single(items[i], 7);
				}
			}
		}
	}

	void gesture::read(auto& reader, Gesture* asset)
	{
		read_assets(reader, ASSET_TYPE_XANIM, asset->anims);
	}

	void gesture_table::read(auto& reader, GestureTable* asset)
	{
		read_assets(reader, ASSET_TYPE_GESTURE, asset->gestures);
	}

	void weapon_style_settings::read(auto& reader, WeaponStyleSettings* asset)
	{
		reader.read_single(asset->data, 3);
	}

	void weapon_tunables::read(auto& reader, WeaponTunables* asset)
	{
		reader.read_array(asset->entries, 3, asset->entryCount);
		reader.read_string(asset->string1);
		reader.read_single(asset->data0, 3);
		reader.read_single(asset->data1, 3);
		reader.read_string(asset->string0);
		read_weapon_value_tables(reader, asset->valueTables);
	}

	void weapon_secondary_movement::read(auto& reader, WeaponSecondaryMovement* asset)
	{
		read_pointer_array(reader, asset->states0, asset->stateCount0);
		read_pointer_array(reader, asset->states1, asset->stateCount1);
		reader.read_array(asset->entries0, 7, asset->entryCount0);
		reader.read_array(asset->entries1, 7, asset->entryCount1);
	}

	void cinematic_motion::read(auto& reader, CinematicMotion* asset)
	{
		read_value_array(reader, asset->values0);
		read_value_array(reader, asset->values1);
		read_value_array(reader, asset->values2);
		read_value_array(reader, asset->values3);
	}

	void weapon_camo::read(auto& reader, WeaponCamo* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		if (!entries)
		{
			return;
		}

		for (auto i = 0ull; i < asset->entryCount; i++)
		{
			auto& entry = entries[i];
			reader.read_asset_array(ASSET_TYPE_MATERIAL, entry.materials, entry.count);
			reader.read_asset_array(ASSET_TYPE_IMAGE, entry.images, entry.count);

			if (const auto layers = reader.read_array(entry.layers, 7, entry.count))
			{
				for (auto j = 0u; j < entry.count; j++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, layers[j].material);
					reader.read_asset(ASSET_TYPE_IMAGE, layers[j].image);
				}
			}
		}
	}

	void weapon_camo_binding::read(auto& reader, WeaponCamoBinding* asset)
	{
		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0ull; i < asset->entryCount; i++)
			{
				reader.read_asset_array(ASSET_TYPE_MATERIAL, entries[i].materials, entries[i].materialCount);
				reader.read_array(entries[i].indices, 1, entries[i].indexCount);
			}
		}

		reader.read_asset(ASSET_TYPE_WEAPONCAMO, asset->camo);
	}

	void shared_weapon_sounds::read(auto& reader, SharedWeaponSounds* asset)
	{
		reader.read_asset(ASSET_TYPE_SURFACESOUNDDEF, asset->surfaceSoundDef0);
		reader.read_asset(ASSET_TYPE_SURFACESOUNDDEF, asset->surfaceSoundDef1);
		read_assets(reader, ASSET_TYPE_ENTITYSOUNDIMPACTS, asset->entitySoundImpacts);
	}

	void weapon_blueprint::read(auto& reader, WeaponBlueprint* asset)
	{
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, entries[i].image);
				read_assets(reader, ASSET_TYPE_XMODEL, entries[i].models);
				reader.read_asset(ASSET_TYPE_WEAPONCAMOBINDING, entries[i].camoBinding);
			}
		}

		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims0, 1);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims1, 12);
		read_assets(reader, ASSET_TYPE_FX, asset->effects0);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx);
		read_assets(reader, ASSET_TYPE_IMPACTSFXTABLE, asset->impactTables);
		read_assets(reader, ASSET_TYPE_FX, asset->effects1);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers0, 2);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers1, 2);
		reader.read_asset(ASSET_TYPE_WEAPONCAMOBINDING, asset->camoBinding);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle);
		read_script_bundle_object(reader, asset->bundle);
	}

	void attachment::read(auto& reader, Attachment* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_RUMBLE, asset->rumble);
		read_assets(reader, ASSET_TYPE_STATUSEFFECT, asset->statusEffects);
		reader.read_array(asset->entries0, 7, asset->entryCount0);
		reader.read_array(asset->entries1, 7, asset->entryCount1);
		reader.read_array(asset->entries2, 7, asset->entryCount2);
		read_value_array(reader, asset->values0);
		read_value_array(reader, asset->values1);
	}

	void attachment_unique::read(auto& reader, AttachmentUnique* asset)
	{
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images0);
		reader.read_array(asset->strings0, 3, 0x40);
		reader.read_array(asset->strings1, 3, 2);

		for (auto& view : asset->views)
		{
			for (auto& set : view.modelSets0)
			{
				read_assets(reader, ASSET_TYPE_XMODEL, set.models);
			}

			for (auto& set : view.modelSets1)
			{
				read_assets(reader, ASSET_TYPE_XMODEL, set.models);
			}

			reader.read_asset(ASSET_TYPE_IMAGE, view.image);
		}

		for (auto& pair : asset->modelPairs0)
		{
			read_assets(reader, ASSET_TYPE_XMODEL, pair.models);
		}

		for (auto& pair : asset->modelPairs1)
		{
			read_assets(reader, ASSET_TYPE_XMODEL, pair.models);
		}

		read_assets(reader, ASSET_TYPE_XMODEL, asset->models);
		reader.read_asset(ASSET_TYPE_FX, asset->effect);
		reader.read_asset(ASSET_TYPE_WEAPONCAMO, asset->camo);
		reader.read_asset(ASSET_TYPE_WEAPONCAMOBINDING, asset->camoBinding);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images1);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims0, 0x194);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims1, 0x168);
		reader.read_asset_array(ASSET_TYPE_GESTURE, asset->gestures, 8);
		read_assets(reader, ASSET_TYPE_SHAREDWEAPONSOUNDS, asset->sounds);
		read_assets(reader, ASSET_TYPE_FX, asset->effects0);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx0);
		reader.read_asset(ASSET_TYPE_FX, asset->effect0);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx1);
		read_assets(reader, ASSET_TYPE_IMPACTSFXTABLE, asset->impactTables);
		read_assets(reader, ASSET_TYPE_FX, asset->effects1);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers0, 2);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers1, 2);
		read_assets(reader, ASSET_TYPE_LASER, asset->lasers);
		read_weapon_value_tables(reader, asset->valueTables);
	}

	void weapon_frontend::read(auto& reader, WeaponFrontend* asset)
	{
		read_script_bundle_object(reader, asset->bundle);
	}

	void weapon::read(auto& reader, Weapon* asset)
	{
		reader.read_asset(ASSET_TYPE_WEAPONTUNABLES, asset->tunables);
		for (auto& style : asset->styles)
		{
			reader.read_asset(ASSET_TYPE_WEAPONSTYLESETTINGS, style.settings);
		}

		reader.read_asset_array(ASSET_TYPE_WEAPONBLUEPRINT, asset->blueprints, asset->blueprintCount);
		reader.read_asset(ASSET_TYPE_WEAPONSECONDARYMOVEMENT, asset->secondaryMovement);
		reader.read_asset(ASSET_TYPE_CINEMATICMOTION, asset->cinematicMotion);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims0, 0x194);
		reader.read_asset_array(ASSET_TYPE_XANIM, asset->anims1, 0x168);
		read_assets(reader, ASSET_TYPE_XANIM, asset->anims2);
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models0);
		reader.read_array(asset->strings0, 3, 0x40);
		reader.read_array(asset->strings1, 3, 0x40);
		reader.read_array(asset->strings2, 3, 2);
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models1);
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models2);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model0);
		read_assets(reader, ASSET_TYPE_XCAM, asset->cams);
		read_assets(reader, ASSET_TYPE_FX, asset->effects0);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx1[0]);
		reader.read_asset(ASSET_TYPE_FX, asset->effect0);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx1[1]);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx1[2]);
		reader.read_asset(ASSET_TYPE_FX, asset->effect1);
		read_assets(reader, ASSET_TYPE_SHAREDWEAPONSOUNDS, asset->sounds);
		read_assets(reader, ASSET_TYPE_TAGFX, asset->tagFx2);
		read_assets(reader, ASSET_TYPE_TAGFX, asset->tagFx3);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images0);
		reader.read_string(asset->string);
		read_assets(reader, ASSET_TYPE_FX, asset->effects2);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images1);
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models3);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images2);

		for (auto& set : asset->modelSets)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, set.model0);
			reader.read_asset(ASSET_TYPE_XMODEL, set.model1);
			reader.read_asset(ASSET_TYPE_FX, set.effect);
		}

		read_assets(reader, ASSET_TYPE_IMAGE, asset->images3);
		read_assets(reader, ASSET_TYPE_MATERIAL, asset->materials);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images4);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx0);
		reader.read_asset(ASSET_TYPE_BALLISTICDESC, asset->ballisticDesc);
		read_assets(reader, ASSET_TYPE_TRIGGEREFFECTDESC, asset->triggerEffects);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images5);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image0);
		read_assets(reader, ASSET_TYPE_OBJECTIVE, asset->objectives);
		read_assets(reader, ASSET_TYPE_XMODEL, asset->models4);
		read_assets(reader, ASSET_TYPE_FX, asset->effects3);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model5);
		read_assets(reader, ASSET_TYPE_FX, asset->effects4);
		reader.read_array(asset->values0, 3, asset->valueCount0);
		reader.read_array(asset->values3, 3, asset->valueCount0);
		reader.read_array(asset->values1, 3, asset->valueCount1);
		reader.read_array(asset->values4, 3, asset->valueCount1);
		reader.read_array(asset->values2, 3, asset->valueCount2);
		reader.read_array(asset->values5, 3, asset->valueCount2);
		reader.read_asset_array(ASSET_TYPE_GESTURE, asset->gestures, 8);
		reader.read_asset_array(ASSET_TYPE_GESTURETABLE, asset->gestureTables, 8);
		read_assets(reader, ASSET_TYPE_RUMBLE, asset->rumbles);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers0, 2);
		reader.read_asset_array(ASSET_TYPE_TRACER, asset->tracers1, 2);
		read_assets(reader, ASSET_TYPE_LASER, asset->lasers);
		read_assets(reader, ASSET_TYPE_FLAMETABLE, asset->flameTables);
		read_assets(reader, ASSET_TYPE_BEAM, asset->beams);
		read_assets(reader, ASSET_TYPE_FX, asset->effects5);
		read_assets(reader, ASSET_TYPE_TAGFX, asset->tagFx4);
		reader.read_asset(ASSET_TYPE_WEAPONCAMO, asset->camo);
		reader.read_asset(ASSET_TYPE_WEAPONCAMOBINDING, asset->camoBinding);
		reader.read_asset(ASSET_TYPE_FX, asset->effect2);

		for (auto& effect : asset->statusEffects)
		{
			reader.read_asset(ASSET_TYPE_STATUSEFFECT, effect.statusEffect);
		}

		read_assets(reader, ASSET_TYPE_FX, asset->effects6);
		read_assets(reader, ASSET_TYPE_IMPACTSFXTABLE, asset->impactTables);
		read_assets(reader, ASSET_TYPE_IMPACTSOUNDSTABLE, asset->impactSoundsTables);
		read_assets(reader, ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundles0);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENT, asset->attachments, 64);
		reader.read_asset_array(ASSET_TYPE_ATTACHMENTUNIQUE, asset->attachmentUniques, 128);
		read_assets(reader, ASSET_TYPE_IMAGE, asset->images6);
		reader.read_single(asset->data, 3);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle1);
	}
}

namespace zonetool::t9
{
	template void gesture::read(zone_reader&, Gesture*);
	template void gesture_table::read(zone_reader&, GestureTable*);
	template void weapon_style_settings::read(zone_reader&, WeaponStyleSettings*);
	template void weapon_tunables::read(zone_reader&, WeaponTunables*);
	template void weapon_secondary_movement::read(zone_reader&, WeaponSecondaryMovement*);
	template void cinematic_motion::read(zone_reader&, CinematicMotion*);
	template void weapon_camo::read(zone_reader&, WeaponCamo*);
	template void weapon_camo_binding::read(zone_reader&, WeaponCamoBinding*);
	template void shared_weapon_sounds::read(zone_reader&, SharedWeaponSounds*);
	template void weapon_blueprint::read(zone_reader&, WeaponBlueprint*);
	template void attachment::read(zone_reader&, Attachment*);
	template void attachment_unique::read(zone_reader&, AttachmentUnique*);
	template void weapon_frontend::read(zone_reader&, WeaponFrontend*);
	template void weapon::read(zone_reader&, Weapon*);
	template void gesture::read(zone_writer&, Gesture*);
	template void gesture_table::read(zone_writer&, GestureTable*);
	template void weapon_style_settings::read(zone_writer&, WeaponStyleSettings*);
	template void weapon_tunables::read(zone_writer&, WeaponTunables*);
	template void weapon_secondary_movement::read(zone_writer&, WeaponSecondaryMovement*);
	template void cinematic_motion::read(zone_writer&, CinematicMotion*);
	template void weapon_camo::read(zone_writer&, WeaponCamo*);
	template void weapon_camo_binding::read(zone_writer&, WeaponCamoBinding*);
	template void shared_weapon_sounds::read(zone_writer&, SharedWeaponSounds*);
	template void weapon_blueprint::read(zone_writer&, WeaponBlueprint*);
	template void attachment::read(zone_writer&, Attachment*);
	template void attachment_unique::read(zone_writer&, AttachmentUnique*);
	template void weapon_frontend::read(zone_writer&, WeaponFrontend*);
	template void weapon::read(zone_writer&, Weapon*);
}
