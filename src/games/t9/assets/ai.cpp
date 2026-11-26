#include <std_include.hpp>
#include "ai.hpp"

namespace zonetool::t9
{
	void aim_table::read(auto&, AimTable*)
	{
	}

	void shoot_table::read(auto&, ShootTable*)
	{
	}

	void anim_selector_table::read(auto& reader, AnimSelectorTable* asset)
	{
		const auto selectors = reader.read_array(asset->selectors, 7, asset->selectorCount);
		if (!selectors)
		{
			return;
		}

		for (auto i = 0; i < asset->selectorCount; i++)
		{
			auto& selector = selectors[i];
			reader.read_array(selector.columns, 7, selector.columnCount);

			if (const auto rows = reader.read_array(selector.rows, 7, selector.rowCount))
			{
				for (auto j = 0; j < selector.rowCount; j++)
				{
					reader.read_array(rows[j].cells, 7, rows[j].cellCount);
					reader.read_single(rows[j].data0, 7);
					reader.read_single(rows[j].data1, 7);
				}
			}

			reader.read_single(selector.defaultRow, 7);
		}
	}

	void anim_mapping_table::read(auto& reader, AnimMappingTable* asset)
	{
		reader.read_array(asset->hashes, 7, asset->entryCount);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_asset_array(ASSET_TYPE_XANIM, entries[i].anims, entries[i].animCount);
			}
		}
	}

	void anim_state_machine::read(auto& reader, AnimStateMachine* asset)
	{
		if (const auto states = reader.read_array(asset->states, 7, asset->stateCount))
		{
			for (auto i = 0; i < asset->stateCount; i++)
			{
				reader.read_array(states[i].indices, 3, states[i].indexCount);
			}
		}

		if (const auto transitions = reader.read_array(asset->transitions, 7, asset->transitionCount))
		{
			for (auto i = 0; i < asset->transitionCount; i++)
			{
				reader.read_array(transitions[i].indices, 3, transitions[i].indexCount);
			}
		}

		reader.read_array(asset->parameters, 7, asset->parameterCount);
	}

	void behavior_tree::read(auto& reader, BehaviorTree* asset)
	{
		if (const auto nodes = reader.read_array(asset->nodes, 7, asset->nodeCount))
		{
			for (auto i = 0; i < asset->nodeCount; i++)
			{
				reader.read_array(nodes[i].children, 3, nodes[i].childCount);
				reader.read_asset(ASSET_TYPE_BEHAVIORSTATEMACHINE, nodes[i].stateMachine);
			}
		}

		reader.read_array(asset->roots, 3, asset->rootCount);
	}

	void behavior_state_machine::read(auto& reader, BehaviorStateMachine* asset)
	{
		if (const auto states = reader.read_array(asset->states, 7, asset->stateCount))
		{
			for (auto i = 0u; i < asset->stateCount; i++)
			{
				reader.read_array(states[i].transitions, 7, states[i].transitionCount);
			}
		}
	}

	void xmodel_alias::read(auto& reader, XModelAlias* asset)
	{
		for (auto& model : asset->models)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}
	}

	void character::read(auto& reader, Character* asset)
	{
		for (auto& bundle : asset->scriptBundles0)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLELIST, asset->scriptBundleList);

		for (auto& bundle : asset->scriptBundles1)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		reader.read_asset(ASSET_TYPE_XMODEL, asset->model0);
		reader.read_asset(ASSET_TYPE_XMODELALIAS, asset->alias0);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model1);

		for (auto& alias : asset->aliases0)
		{
			reader.read_asset(ASSET_TYPE_XMODELALIAS, alias);
		}

		reader.read_asset(ASSET_TYPE_XMODEL, asset->model2);
		reader.read_asset(ASSET_TYPE_XMODELALIAS, asset->alias1);
		reader.read_asset(ASSET_TYPE_XMODEL, asset->model3);
		reader.read_asset(ASSET_TYPE_XMODELALIAS, asset->alias2);

		for (auto& model : asset->models0)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, model);
		}

		for (auto& pair : asset->modelPairs)
		{
			reader.read_asset(ASSET_TYPE_XMODELALIAS, pair.alias);
			reader.read_asset(ASSET_TYPE_XMODEL, pair.model);
		}

		reader.read_asset(ASSET_TYPE_XMODELALIAS, asset->alias3);

		for (auto& entry : asset->modelEntries)
		{
			reader.read_asset(ASSET_TYPE_XMODEL, entry.model);
		}

		reader.read_asset(ASSET_TYPE_RAGDOLL, asset->ragdoll);
		reader.read_string(asset->string0);
		reader.read_string(asset->string1);

		for (auto& effect : asset->effects)
		{
			reader.read_asset(ASSET_TYPE_FX, effect);
		}
	}

	void ai_type::read(auto& reader, AIType* asset)
	{
		reader.read_string(asset->string);

		for (auto& weapon : asset->weapons0)
		{
			reader.read_asset(ASSET_TYPE_WEAPON, weapon);
		}

		for (auto& weapon : asset->weapons1)
		{
			reader.read_asset(ASSET_TYPE_WEAPON, weapon);
		}

		reader.read_asset(ASSET_TYPE_WEAPON, asset->weapon);
		reader.read_asset(ASSET_TYPE_FOOTSTEPTABLE, asset->footstepTable);

		for (auto& table : asset->surfaceFxTables)
		{
			reader.read_asset(ASSET_TYPE_SURFACEFXTABLE, table);
		}

		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle0);

		for (auto& character : asset->characters)
		{
			reader.read_asset(ASSET_TYPE_CHARACTER, character);
		}

		reader.read_asset(ASSET_TYPE_BEHAVIORTREE, asset->behaviorTree);
		reader.read_asset(ASSET_TYPE_ANIMSTATEMACHINE, asset->animStateMachine);
		reader.read_asset(ASSET_TYPE_ANIMSELECTORTABLE, asset->animSelectorTable);

		for (auto& table : asset->animMappingTables)
		{
			reader.read_asset(ASSET_TYPE_ANIMMAPPINGTABLE, table);
		}

		reader.read_asset(ASSET_TYPE_AIMTABLE, asset->aimTable);
		reader.read_asset(ASSET_TYPE_SHOOTTABLE, asset->shootTable);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle1);

		for (auto& bundle : asset->scriptBundles0)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		for (auto& bundle : asset->scriptBundles1)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		for (auto& bundle : asset->scriptBundles2)
		{
			reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, bundle);
		}

		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_SCRIPTBUNDLE, asset->scriptBundle2);
	}

	void tactical_query::read(auto& reader, TacticalQuery* asset)
	{
		for (auto& string : asset->strings)
		{
			reader.read_string(string);
		}
	}
}

namespace zonetool::t9
{
	template void aim_table::read(zone_reader&, AimTable*);
	template void shoot_table::read(zone_reader&, ShootTable*);
	template void anim_selector_table::read(zone_reader&, AnimSelectorTable*);
	template void anim_mapping_table::read(zone_reader&, AnimMappingTable*);
	template void anim_state_machine::read(zone_reader&, AnimStateMachine*);
	template void behavior_tree::read(zone_reader&, BehaviorTree*);
	template void behavior_state_machine::read(zone_reader&, BehaviorStateMachine*);
	template void xmodel_alias::read(zone_reader&, XModelAlias*);
	template void character::read(zone_reader&, Character*);
	template void ai_type::read(zone_reader&, AIType*);
	template void tactical_query::read(zone_reader&, TacticalQuery*);
	template void aim_table::read(zone_writer&, AimTable*);
	template void shoot_table::read(zone_writer&, ShootTable*);
	template void anim_selector_table::read(zone_writer&, AnimSelectorTable*);
	template void anim_mapping_table::read(zone_writer&, AnimMappingTable*);
	template void anim_state_machine::read(zone_writer&, AnimStateMachine*);
	template void behavior_tree::read(zone_writer&, BehaviorTree*);
	template void behavior_state_machine::read(zone_writer&, BehaviorStateMachine*);
	template void xmodel_alias::read(zone_writer&, XModelAlias*);
	template void character::read(zone_writer&, Character*);
	template void ai_type::read(zone_writer&, AIType*);
	template void tactical_query::read(zone_writer&, TacticalQuery*);
}
