#include <std_include.hpp>
#include "player.hpp"

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

		void read_outfit_parts(auto& reader, PlayerOutfit* asset)
		{
			const auto parts = reader.read_array(asset->parts, 7, asset->partCount);
			for (std::uint32_t i = 0; parts && i < asset->partCount; i++)
			{
				auto& part = parts[i];
				reader.read_asset(ASSET_TYPE_SCRIPTBUNDLELIST, part.bundleList);
				read_assets(reader, ASSET_TYPE_SCRIPTBUNDLE, part.bundles);
				read_assets(reader, ASSET_TYPE_XMODEL, part.models);
				reader.read_asset(ASSET_TYPE_XMODEL, part.model);
				reader.read_asset_array(ASSET_TYPE_MATERIAL, part.materials, part.materialCount);
			}
		}

		void read_outfit_models(auto& reader, PlayerOutfit* asset)
		{
			const auto models = reader.read_array(asset->models, 7, asset->modelCount);
			for (std::uint32_t i = 0; models && i < asset->modelCount; i++)
			{
				auto& model = models[i];
				reader.read_asset(ASSET_TYPE_XMODEL, model.model);
				reader.read_asset_array(ASSET_TYPE_MATERIAL, model.materials0, model.materialCount0);
				reader.read_asset_array(ASSET_TYPE_MATERIAL, model.materials1, model.materialCount1);
			}
		}

		template <typename T>
		void read_outfit_bundle_sets(auto& reader, T*& field, const std::uint32_t count)
		{
			const auto sets = reader.read_array(field, 7, count);
			for (std::uint32_t i = 0; sets && i < count; i++)
			{
				auto& set = sets[i];
				reader.read_asset(ASSET_TYPE_SCRIPTBUNDLELIST, set.bundleList);
				read_assets(reader, ASSET_TYPE_SCRIPTBUNDLE, set.bundles);
				read_assets(reader, ASSET_TYPE_XMODEL, set.models0);
				read_assets(reader, ASSET_TYPE_XMODEL, set.models1);
				read_assets(reader, ASSET_TYPE_XMODEL, set.models2);
			}
		}

		void read_outfit_model_lists(auto& reader, PlayerOutfit* asset)
		{
			const auto lists = reader.read_array(asset->modelLists, 7, asset->modelListCount);
			for (std::uint32_t i = 0; lists && i < asset->modelListCount; i++)
			{
				read_assets(reader, ASSET_TYPE_XMODEL, lists[i].models);
			}
		}

		void read_material_groups(auto& reader, PlayerOutfitMaterialGroup*& field, const std::uint32_t count)
		{
			const auto groups = reader.read_array(field, 7, count);
			for (std::uint32_t i = 0; groups && i < count; i++)
			{
				auto& group = groups[i];
				const auto slots = reader.read_array(group.materialSlots, 7, group.materialSlotCount);
				for (std::uint16_t j = 0; slots && j < group.materialSlotCount; j++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, slots[j].material0);
					reader.read_asset(ASSET_TYPE_MATERIAL, slots[j].material1);
					reader.read_asset(ASSET_TYPE_IMAGE, slots[j].image);
				}

				reader.read_array(group.materialIndices, 1, group.materialSlotCount);

				const auto image_slots = reader.read_array(group.materialImageSlots, 7, group.materialImageSlotCount);
				for (std::uint16_t j = 0; image_slots && j < group.materialImageSlotCount; j++)
				{
					auto& slot = image_slots[j];
					if (const auto material_image = reader.read_single(slot.materialImage, 7))
					{
						reader.read_asset(ASSET_TYPE_MATERIAL, material_image->material);
						reader.read_asset(ASSET_TYPE_IMAGE, material_image->image);
					}

					reader.read_array(slot.values, 3, slot.valueCount);
				}
			}
		}

		void read_material_sets(auto& reader, PlayerOutfitMaterialSet*& field, const std::uint32_t count)
		{
			const auto sets = reader.read_array(field, 7, count);
			for (std::uint32_t i = 0; sets && i < count; i++)
			{
				auto& set = sets[i];
				reader.read_asset(ASSET_TYPE_IMAGE, set.image);
				reader.read_array(set.indices, 1, set.indexCount);
				read_material_groups(reader, set.groups, set.groupCount);
			}
		}

		void read_outfit_index_sets(auto& reader, PlayerOutfit* asset)
		{
			const auto sets = reader.read_array(asset->indexSets, 7, asset->indexSetCount);
			for (std::uint32_t i = 0; sets && i < asset->indexSetCount; i++)
			{
				reader.read_array(sets[i].values, 1, sets[i].valueCount);
				reader.read_array(sets[i].indices, 1, sets[i].indexCount);
			}
		}

		void read_outfit_image_sets(auto& reader, PlayerOutfit* asset)
		{
			const auto sets = reader.read_array(asset->imageSets, 7, asset->imageSetCount);
			for (std::uint64_t i = 0; sets && i < asset->imageSetCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, sets[i].image0);
				reader.read_asset(ASSET_TYPE_IMAGE, sets[i].image1);
			}
		}

		void read_animation_sets(auto& reader, PlayerAnimationSet*& field, const std::uint32_t count)
		{
			const auto sets = reader.read_array(field, 7, count);
			for (std::uint32_t i = 0; sets && i < count; i++)
			{
				auto& set = sets[i];
				reader.read_string(set.name);

				const auto keys = reader.read_array(set.keys, 7, set.keyCount);
				for (std::uint32_t j = 0; keys && j < set.keyCount; j++)
				{
					reader.read_array(keys[j].values, 3, keys[j].valueCount);
					reader.read_array(keys[j].weights0, 3, keys[j].weightCount);
					reader.read_array(keys[j].weights1, 3, keys[j].weightCount);
				}
			}
		}
	}

	void player_sounds_table::read(auto& reader, PlayerSoundsTable* asset)
	{
		read_assets(reader, ASSET_TYPE_SURFACESOUNDDEF, asset->soundDefs0);
		read_assets(reader, ASSET_TYPE_SURFACESOUNDDEF, asset->soundDefs1);
		read_assets(reader, ASSET_TYPE_SURFACESOUNDDEF, asset->soundDefs2);
		read_assets(reader, ASSET_TYPE_SURFACESOUNDDEF, asset->soundDefs3);
	}

	void player_fx_table::read(auto& reader, PlayerFxTable* asset)
	{
		read_assets(reader, ASSET_TYPE_SURFACEFXTABLE, asset->surfaceFxTables);
		read_assets(reader, ASSET_TYPE_FX, asset->effects);
	}

	void player_role_category::read(auto& reader, PlayerRoleCategory* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		read_script_bundle_object(reader, asset->bundle);
	}

	void player_role_category_table::read(auto& reader, PlayerRoleCategoryTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_PLAYERROLECATEGORY, asset->categories, asset->categoryCount);
	}

	void player_outfit::read(auto& reader, PlayerOutfit* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_PLAYERFXTABLE, asset->fxTable);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx);
		reader.read_asset(ASSET_TYPE_RAGDOLL, asset->ragdoll);
		reader.read_asset(ASSET_TYPE_FX, asset->effect);
		read_outfit_parts(reader, asset);
		read_outfit_models(reader, asset);
		read_outfit_bundle_sets(reader, asset->bundleSets0, asset->bundleSetCount0);
		read_outfit_model_lists(reader, asset);
		read_outfit_bundle_sets(reader, asset->bundleSets1, asset->bundleSetCount1);
		read_material_groups(reader, asset->materialGroups, asset->materialGroupCount);
		read_material_sets(reader, asset->materialSets0, asset->materialSetCount0);
		read_material_sets(reader, asset->materialSets1, asset->materialSetCount1);
		read_outfit_index_sets(reader, asset);
		read_outfit_image_sets(reader, asset);
	}

	void character_body_type::read(auto& reader, CharacterBodyType* asset)
	{
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle);
		reader.read_string(asset->string);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset_array(ASSET_TYPE_PLAYEROUTFIT, asset->outfits, asset->outfitCount);
		read_script_bundle_object(reader, asset->bundle);
		reader.read_asset(ASSET_TYPE_PLAYERSOUNDSTABLE, asset->soundsTable);
		reader.read_asset(ASSET_TYPE_PLAYERFXTABLE, asset->fxTable);
		reader.read_asset(ASSET_TYPE_TAGFX, asset->tagFx);
		read_assets(reader, ASSET_TYPE_FOOTSTEPTABLE, asset->footstepTables);
		reader.read_array(asset->values, 1, asset->valueCount);
		reader.read_array(asset->indices, 1, asset->indexCount);

		const auto index_sets = reader.read_array(asset->indexSets, 7, asset->outfitCount);
		for (std::uint32_t i = 0; index_sets && i < asset->outfitCount; i++)
		{
			reader.read_array(index_sets[i].values, 1, index_sets[i].valueCount);
		}
	}

	void player_movement_tunables::read(auto& reader, PlayerMovementTunables* asset)
	{
		reader.read_asset(ASSET_TYPE_CINEMATICMOTION, asset->cinematicMotion0);
		for (auto& value : asset->values0)
		{
			read_value_array(reader, value.values);
		}

		for (auto& value : asset->values1)
		{
			read_value_array(reader, value.values);
		}

		reader.read_asset(ASSET_TYPE_CINEMATICMOTION, asset->cinematicMotion1);
		for (auto& value : asset->values2)
		{
			read_value_array(reader, value.values);
		}

		for (auto& value : asset->values3)
		{
			read_value_array(reader, value.values);
		}

		for (auto& values : asset->values4)
		{
			read_value_array(reader, values);
		}

		for (auto& values : asset->values5)
		{
			read_value_array(reader, values);
		}

		for (auto& values : asset->values6)
		{
			read_value_array(reader, values);
		}

		read_assets(reader, ASSET_TYPE_RUMBLE, asset->rumbles);
	}

	void talent::read(auto& reader, Talent* asset)
	{
		read_script_bundle_object(reader, asset->bundle);
	}

	void player_talent_template::read(auto& reader, PlayerTalentTemplate* asset)
	{
		reader.read_asset_array(ASSET_TYPE_TALENT, asset->talents0, asset->talentCount0);
		reader.read_asset_array(ASSET_TYPE_TALENT, asset->talents1, asset->talentCount1);
	}

	void player_role_template::read(auto& reader, PlayerRoleTemplate* asset)
	{
		reader.read_asset(ASSET_TYPE_CHARACTERBODYTYPE, asset->bodyType);
		reader.read_asset(ASSET_TYPE_PLAYERROLECATEGORY, asset->category);
		reader.read_asset(ASSET_TYPE_PLAYERMOVEMENTTUNABLES, asset->movementTunables);
		reader.read_asset(ASSET_TYPE_PLAYERTALENTTEMPLATE, asset->talentTemplate);
		read_script_bundle_object(reader, asset->bundle);
		reader.read_asset_array(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundles, 4);
	}

	void customization_table::read(auto& reader, CustomizationTable* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		for (std::uint32_t i = 0; entries && i < asset->entryCount; i++)
		{
			if (const auto entry = reader.read_single(entries[i], 7))
			{
				reader.read_asset(ASSET_TYPE_PLAYERROLETEMPLATE, entry->roleTemplate);
			}
		}
	}

	void customization_table_frontend::read(auto& reader, CustomizationTableFrontend* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		for (std::uint32_t i = 0; entries && i < asset->entryCount; i++)
		{
			if (const auto entry = reader.read_single(entries[i], 7))
			{
				reader.read_asset(ASSET_TYPE_PLAYERROLETEMPLATEFRONTEND, entry->roleTemplate);
			}
		}
	}

	void player_role_template_frontend::read(auto& reader, PlayerRoleTemplateFrontend* asset)
	{
		reader.read_asset(ASSET_TYPE_CHARACTERBODYTYPEFRONTEND, asset->bodyType);
		read_script_bundle_object(reader, asset->bundle);
	}

	void character_body_type_frontend::read(auto& reader, CharacterBodyTypeFrontend* asset)
	{
		read_script_bundle_object(reader, asset->bundle);
	}

	void char_mixer::read(auto& reader, CharMixer* asset)
	{
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model);
	}

	void execution::read(auto& reader, Execution* asset)
	{
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcam0);
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcam1);
		reader.read_asset(ASSET_TYPE_XCAM, asset->xcam2);
		read_assets(reader, ASSET_TYPE_XANIM, asset->anims);
		read_script_bundle_object(reader, asset->bundle);
	}

	void player_animation::read(auto& reader, PlayerAnimation* asset)
	{
		reader.read_array(asset->values0, 3, asset->entryCount);
		reader.read_array(asset->values1, 7, asset->entryCount);
		read_animation_sets(reader, asset->sets0, asset->setCount0);
		read_animation_sets(reader, asset->sets1, asset->setCount1);
	}

	void player_settings::read(auto& reader, PlayerSettings* asset)
	{
		reader.read_array(asset->data, 0, asset->dataSize);
	}

	void motion_matching_input::read(auto&, MotionMatchingInput*)
	{
	}

	void spray_or_gesture_item::read(auto& reader, SprayOrGestureItem* asset)
	{
		switch (asset->type.value & 0x7FFFFFFFFFFFFFFF)
		{
		case SPRAY_OR_GESTURE_TYPE_GESTURE:
		{
			reader.read_asset(ASSET_TYPE_GESTURE, asset->gestures.gesture0);
			reader.read_asset(ASSET_TYPE_GESTURE, asset->gestures.gesture1);

			const auto entries = reader.read_array(asset->gestures.entries, 7, asset->gestures.entryCount);
			for (std::uint32_t i = 0; entries && i < asset->gestures.entryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_GESTURE, entries[i].gesture0);
				reader.read_asset(ASSET_TYPE_GESTURE, entries[i].gesture1);
				reader.read_array(entries[i].values, 7, entries[i].valueCount);
			}
			break;
		}
		case SPRAY_OR_GESTURE_TYPE_MATERIAL:
			reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
			break;
		case SPRAY_OR_GESTURE_TYPE_GESTURE_TABLE:
			reader.read_asset(ASSET_TYPE_GESTURETABLE, asset->gestureTable);
			break;
		}

		read_script_bundle_object(reader, asset->bundle);
	}

	void spray_or_gesture_list::read(auto& reader, SprayOrGestureList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_SPRAYORGESTUREITEM, asset->items, asset->itemCount);
	}

	void ui_model_data_struct::read(auto& reader, UIModelDataStruct* asset)
	{
		reader.read_asset(ASSET_TYPE_UIMODELDATASTRUCT, asset->parent0);
		reader.read_asset(ASSET_TYPE_UIMODELDATASTRUCT, asset->parent1);

		const auto fields = reader.read_array(asset->fields, 7, asset->fieldCount);
		for (std::uint32_t i = 0; fields && i < asset->fieldCount; i++)
		{
			reader.read_asset(ASSET_TYPE_UIMODELDATASTRUCT, fields[i].dataStruct);
		}
	}
}

namespace zonetool::t9
{
	template void player_sounds_table::read(zone_reader&, PlayerSoundsTable*);
	template void player_fx_table::read(zone_reader&, PlayerFxTable*);
	template void player_role_category::read(zone_reader&, PlayerRoleCategory*);
	template void player_role_category_table::read(zone_reader&, PlayerRoleCategoryTable*);
	template void player_outfit::read(zone_reader&, PlayerOutfit*);
	template void character_body_type::read(zone_reader&, CharacterBodyType*);
	template void player_movement_tunables::read(zone_reader&, PlayerMovementTunables*);
	template void talent::read(zone_reader&, Talent*);
	template void player_talent_template::read(zone_reader&, PlayerTalentTemplate*);
	template void player_role_template::read(zone_reader&, PlayerRoleTemplate*);
	template void customization_table::read(zone_reader&, CustomizationTable*);
	template void customization_table_frontend::read(zone_reader&, CustomizationTableFrontend*);
	template void player_role_template_frontend::read(zone_reader&, PlayerRoleTemplateFrontend*);
	template void character_body_type_frontend::read(zone_reader&, CharacterBodyTypeFrontend*);
	template void char_mixer::read(zone_reader&, CharMixer*);
	template void execution::read(zone_reader&, Execution*);
	template void player_animation::read(zone_reader&, PlayerAnimation*);
	template void player_settings::read(zone_reader&, PlayerSettings*);
	template void motion_matching_input::read(zone_reader&, MotionMatchingInput*);
	template void spray_or_gesture_item::read(zone_reader&, SprayOrGestureItem*);
	template void spray_or_gesture_list::read(zone_reader&, SprayOrGestureList*);
	template void ui_model_data_struct::read(zone_reader&, UIModelDataStruct*);
	template void player_sounds_table::read(zone_writer&, PlayerSoundsTable*);
	template void player_fx_table::read(zone_writer&, PlayerFxTable*);
	template void player_role_category::read(zone_writer&, PlayerRoleCategory*);
	template void player_role_category_table::read(zone_writer&, PlayerRoleCategoryTable*);
	template void player_outfit::read(zone_writer&, PlayerOutfit*);
	template void character_body_type::read(zone_writer&, CharacterBodyType*);
	template void player_movement_tunables::read(zone_writer&, PlayerMovementTunables*);
	template void talent::read(zone_writer&, Talent*);
	template void player_talent_template::read(zone_writer&, PlayerTalentTemplate*);
	template void player_role_template::read(zone_writer&, PlayerRoleTemplate*);
	template void customization_table::read(zone_writer&, CustomizationTable*);
	template void customization_table_frontend::read(zone_writer&, CustomizationTableFrontend*);
	template void player_role_template_frontend::read(zone_writer&, PlayerRoleTemplateFrontend*);
	template void character_body_type_frontend::read(zone_writer&, CharacterBodyTypeFrontend*);
	template void char_mixer::read(zone_writer&, CharMixer*);
	template void execution::read(zone_writer&, Execution*);
	template void player_animation::read(zone_writer&, PlayerAnimation*);
	template void player_settings::read(zone_writer&, PlayerSettings*);
	template void motion_matching_input::read(zone_writer&, MotionMatchingInput*);
	template void spray_or_gesture_item::read(zone_writer&, SprayOrGestureItem*);
	template void spray_or_gesture_list::read(zone_writer&, SprayOrGestureList*);
	template void ui_model_data_struct::read(zone_writer&, UIModelDataStruct*);
}
