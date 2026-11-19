#include <std_include.hpp>
#include "animset.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_anims(zone_reader& reader, AnimsetAnim*& field, const unsigned int count)
		{
			if (const auto anims = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_script_string(anims[i].name);
					reader.read_script_string(anims[i].aimsetName);
				}
			}
		}

		void read_params(zone_reader& reader, ASM_Function_Param*& field, const int count)
		{
			if (const auto params = reader.read_array(field, 3, count))
			{
				for (auto i = 0; i < count; i++)
				{
					if (params[i].m_Type == ParamType_String)
					{
						reader.read_script_string(params[i].u.m_String);
					}
				}
			}
		}

		void read_function(zone_reader& reader, ASM_Function& func)
		{
			read_params(reader, func.m_Params, func.m_NumParams);
		}

		void read_functions(zone_reader& reader, ASM_Function*& field, const unsigned int count)
		{
			if (const auto funcs = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_function(reader, funcs[i]);
				}
			}
		}

		void read_asm_state(zone_reader& reader, ASM_State& state, const ASM_Mode mode)
		{
			if (const auto transitions = reader.read_array(state.m_Transitions, 7, state.m_NumTransitions))
			{
				for (auto i = 0; i < state.m_NumTransitions; i++)
				{
					read_function(reader, transitions[i].m_FuncName);
				}
			}

			reader.read_script_string(state.m_Name);
			reader.read_script_string_array(state.m_Subtrees, state.m_NumSubtrees);
			read_function(reader, state.m_PlayAnim);
			read_function(reader, state.m_PlayAnimNoteHandler);
			read_function(reader, state.m_ChooseAnimFunc);
			read_function(reader, state.m_TerminateFunc);
			reader.read_script_string(state.m_ExitNotify);
			reader.read_script_string(state.m_FacialState);
			read_params(reader, state.m_DeathParams, state.m_DeathNumParams);
			read_params(reader, state.m_PainParams, state.m_PainNumParams);
			read_params(reader, state.m_ExecutionParams, state.m_ExecutionNumParams);

			if (mode == ASM_MODE_PLAYER)
			{
				if (const auto player_state = reader.read_single(state.u.m_PlayerState, 7))
				{
					read_function(reader, player_state->m_UpdateFunc);
					read_params(reader, player_state->m_ScriptedAnimParams, player_state->m_ScriptedAnimNumParams);
				}
			}
			else if (mode == ASM_MODE_AI)
			{
				if (const auto ai_state = reader.read_single(state.u.m_AIState, 7))
				{
					read_params(reader, ai_state->m_PowerDownParams, ai_state->m_PowerDownNumParams);
					reader.read_script_string(ai_state->m_AnimMode);
					reader.read_script_string(ai_state->m_OrientMode);
					reader.read_script_string(ai_state->m_AnimModeMP);
					reader.read_script_string(ai_state->m_OrientModeMP);
				}
			}
		}

		void read_player_alias(zone_reader& reader, PlayerAnimsetAlias* alias)
		{
			reader.read_script_string(alias->aimState);
			reader.read_script_string(alias->aimAlias);
			reader.read_script_string(alias->moveTurnState);
			reader.read_script_string(alias->moveTurnAlias);

			if (const auto addons = reader.read_array(alias->aliasAddons, 3, alias->numAliasAddons))
			{
				for (auto i = 0u; i < alias->numAliasAddons; i++)
				{
					reader.read_script_string(addons[i].stateName);
					reader.read_script_string(addons[i].aliasName);
					reader.read_script_string(addons[i].addonType);
				}
			}

			reader.read_script_string_array(alias->patchInfo.strings, alias->patchInfo.numStrings);
		}

		void read_alias(zone_reader& reader, AnimsetAlias& alias, const ASM_Mode mode)
		{
			reader.read_script_string(alias.name);
			read_anims(reader, alias.anims, alias.numAnims);
			read_functions(reader, alias.funcs, alias.numFuncs);
			reader.read_script_string_array(alias.addons, alias.numAddons);
			reader.read_script_string(alias.animAlias);
			reader.read_script_string(alias.curveName);

			if (mode == ASM_MODE_AI)
			{
				if (const auto ai_alias = reader.read_single(alias.u.m_AIAnimsetAlias, 7))
				{
					read_anims(reader, ai_alias->redAnims, ai_alias->numRedAnims);
				}
			}
			else if (mode == ASM_MODE_PLAYER)
			{
				if (const auto player_alias = reader.read_single(alias.u.m_PlayerAnimsetAlias, 7))
				{
					read_player_alias(reader, player_alias);
				}
			}
		}

		void read_player_state(zone_reader& reader, PlayerAnimsetState* state)
		{
			const auto types = reader.read_array(state->aliasesPerConditionType, 7, state->numConditionTypes);
			if (!types)
			{
				return;
			}

			for (auto i = 0u; i < state->numConditionTypes; i++)
			{
				if (const auto aliases = reader.read_array(types[i].aliases, 7, types[i].numConditionValues))
				{
					for (auto j = 0u; j < types[i].numConditionValues; j++)
					{
						reader.read_array(aliases[j].array, 3, aliases[j].wordCount);
					}
				}
			}
		}

		void read_state(zone_reader& reader, AnimsetState& state, const ASM_Mode mode)
		{
			reader.read_script_string(state.name);

			if (const auto aliases = reader.read_array(state.animAliases, 7, state.numAnimAliases))
			{
				for (auto i = 0u; i < state.numAnimAliases; i++)
				{
					read_alias(reader, aliases[i], mode);
				}
			}

			if (const auto aliases = reader.read_array(state.blendtreeAliases, 7, state.numBlendtreeAliases))
			{
				for (auto i = 0u; i < state.numBlendtreeAliases; i++)
				{
					reader.read_script_string(aliases[i].name);
					reader.read_script_string(aliases[i].graftNode);
					reader.read_script_string(aliases[i].blendtreeName);
					read_functions(reader, aliases[i].funcs, aliases[i].numFuncs);
				}
			}

			reader.read_script_string(state.notify);
			reader.read_script_string(state.aimsetTarget);
			reader.read_script_string(state.aimsetRoot);
			reader.read_array(state.animEntryMap, 1, state.numAnims);

			if (mode == ASM_MODE_AI)
			{
				reader.read_array(state.u.m_AIAnimsetState, 0, 1);
			}
			else if (mode == ASM_MODE_PLAYER)
			{
				if (const auto player_state = reader.read_single(state.u.m_PlayerAnimsetState, 7))
				{
					read_player_state(reader, player_state);
				}
			}
		}
	}

	void animset::read(zone_reader& reader, Animset* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_script_string(asset->constName);
		reader.read_script_string(asset->animTree);

		if (const auto states = reader.read_array(asset->states, 7, asset->numStates))
		{
			for (auto i = 0u; i < asset->numStates; i++)
			{
				read_state(reader, states[i], asset->mode);
			}
		}

		reader.read_script_string(asset->startStateName);
		reader.read_script_string(asset->startAliasName);
		reader.read_script_string(asset->clearStateName);
		reader.read_script_string(asset->clearAliasName);
		reader.read_script_string(asset->alwaysOnStateName);

		if (asset->mode == ASM_MODE_AI)
		{
			reader.read_single(asset->u.m_AIAnimset, 3);
		}
		else if (asset->mode == ASM_MODE_PLAYER)
		{
			if (const auto player = reader.read_single(asset->u.m_PlayerAnimset, 7))
			{
				reader.read_array(player->packMap, 1, player->numPackMapEntries);
				reader.read_array(player->unpackMap, 1, player->numUnpackMapEntries);
				reader.read_script_string(player->patchName);
			}
		}
		reader.pop_stream();
	}

	void ai_asm::read(zone_reader& reader, ASM* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->m_szName);
		reader.read_script_string(asset->m_Name);

		if (const auto states = reader.read_array(asset->m_States, 7, asset->m_NumStates))
		{
			for (auto i = 0; i < asset->m_NumStates; i++)
			{
				read_asm_state(reader, states[i], asset->m_Mode);
			}
		}

		if (const auto groups = reader.read_array(asset->m_LogicGroups, 7, asset->m_NumLogicGroups))
		{
			for (auto i = 0; i < asset->m_NumLogicGroups; i++)
			{
				read_function(reader, groups[i].m_InitFunc);
				read_function(reader, groups[i].m_TerminateFunc);
			}
		}

		reader.read_array(asset->m_FuncIDs, 3, asset->m_NumFuncIDs);

		if (asset->m_Mode == ASM_MODE_AI)
		{
			if (const auto ai = reader.read_single(asset->u.m_AIASM, 7))
			{
				reader.read_array(ai->m_Services, 3, ai->m_NumServices);
			}
		}
		else if (asset->m_Mode == ASM_MODE_PLAYER)
		{
			reader.read_array(asset->u.m_PlayerASM, 0, 1);
		}
		reader.pop_stream();
	}
}
