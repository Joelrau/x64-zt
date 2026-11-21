#include <std_include.hpp>

#include "../include.hpp"
#include "scriptable.hpp"
#include "sound.hpp"
#include "xanim.hpp"
#include "xmodel.hpp"

#include <iw8/assets/scriptable.hpp>

namespace zonetool::iw7::converter::iw8::scriptable
{
	namespace
	{
		namespace target = zonetool::iw8;

		bool is_supported(const ScriptableEventDef* events, unsigned int count);

		bool is_supported(const ScriptableEventDef& event)
		{
			switch (event.type)
			{
			case Scriptable_EventType_Model:
			case Scriptable_EventType_Wait:
			case Scriptable_EventType_Sound:
				return true;
			case Scriptable_EventType_StateChange:
				return event.data.stateChange.partReference.type == type_1453E1C00; // TODO: other part reference types
			case Scriptable_EventType_Animation:
				return is_supported(event.data.animation.eventsAtEnd, event.data.animation.eventAtEndCount);
			default:
				return false;
			}
		}

		bool is_supported(const ScriptableEventDef* events, const unsigned int count)
		{
			for (auto i = 0u; i < count; i++)
			{
				if (!is_supported(events[i]))
				{
					return false;
				}
			}

			return true;
		}

		bool is_supported(const ScriptablePartDef* parts, const unsigned int count)
		{
			for (auto i = 0u; i < count; i++)
			{
				const auto& part = parts[i];
				for (auto j = 0u; j < part.numStates; j++)
				{
					const auto& state = part.states[j];
					if (state.type != Scriptable_StateType_Simple && state.type != Scriptable_StateType_Scripted)
					{
						return false;
					}

					if (!is_supported(state.base.events, state.base.numEvents))
					{
						return false;
					}
				}

				if (!is_supported(part.childParts, part.numChildParts))
				{
					return false;
				}
			}

			return true;
		}

		bool is_supported(const ScriptableDef& asset)
		{
			return asset.numParts && is_supported(asset.parts, asset.numParts);
		}

		template <typename T>
		T* make_named(const char* name, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<T>();
			result->name = name;
			return result;
		}

		target::XModel* convert_model(const XModel* model, utils::memory::allocator& allocator)
		{
			if (!model)
			{
				return nullptr;
			}

			if (const auto loaded = static_cast<XModel*>(db_find_asset(ASSET_TYPE_XMODEL, model->name)); loaded && loaded->numsurfs)
			{
				xmodel::dump(loaded);
			}

			return make_named<target::XModel>(model->name, allocator);
		}

		target::XAnimParts* convert_animation(const XAnimParts* animation, utils::memory::allocator& allocator)
		{
			if (!animation)
			{
				return nullptr;
			}

			xanim::dump(static_cast<XAnimParts*>(db_find_asset(ASSET_TYPE_XANIMPARTS, animation->name)));
			return make_named<target::XAnimParts>(animation->name, allocator);
		}

		target::ScriptableEventDef* convert_events(const ScriptableEventDef* events, unsigned int count, unsigned int part_flat_id,
			utils::memory::allocator& allocator);

		void convert_model_event(const ScriptableEventModelDef& event, target::ScriptableEventDef& result, utils::memory::allocator& allocator)
		{
			result.type = target::Scriptable_EventType_Model;
			result.base.flags = 0x2; // TODO: stock model event value

			auto& model = result.data.model;
			model.base = &result.base;
			model.dataType = target::SCRIPTABLE_MODEL_DATA_MODEL;
			model.data.model = convert_model(event.model, allocator);
			model.hudOutlineColor = event.hudOutlineColor;
			model.hudOutlineActive = event.hudOutlineActive;
			model.hudOutlineFill = event.hudOutlineFill;
			model.neverMoves = event.neverMoves;
			model.dynamicSimulation = event.dynamicSimulation;
			model.activatePhysics = event.activatePhysics;
		}

		void convert_wait_event(const ScriptableEventWaitDef& event, target::ScriptableEventDef& result)
		{
			result.type = target::Scriptable_EventType_Wait;
			result.base.flags = 0x200;

			auto& wait = result.data.wait;
			wait.base = &result.base;
			wait.delayMin = event.delayMin;
			wait.delayMax = event.delayMax;
			wait.eventStreamBufferOffsetServer = static_cast<unsigned short>(event.eventStreamBufferOffsetServer);
			wait.eventStreamBufferOffsetClient = static_cast<unsigned short>(event.eventStreamBufferOffsetClient);
		}

		void convert_sound_event(const ScriptableEventSoundDef& event, target::ScriptableEventDef& result)
		{
			auto& converted = result.data.sound;
			converted.looping = event.soundAlias && sound::is_looping(event.soundAlias);

			result.type = target::Scriptable_EventType_Sound;
			result.base.flags = converted.looping ? 0x200 : 0;

			converted.base = &result.base;
			converted.stateful = event.stateful;
			converted.tagName = event.tagName;
			converted.scrTagName = static_cast<target::scr_string_t>(event.scrTagName);
			converted.soundAlias = event.soundAlias;
			converted.groundTraceLength = 40.0f; // TODO: stock value
		}

		void convert_state_change_event(const ScriptableEventStateChangeDef& event, target::ScriptableEventDef& result, const unsigned int part_flat_id)
		{
			result.type = target::Scriptable_EventType_StateChange;
			result.base.flags = 0;

			auto& state_change = result.data.stateChange;
			state_change.base = &result.base;
			state_change.partReference.flatId = part_flat_id;
			state_change.stateIdx = event.stateIdx;
		}

		void convert_animation_event(const ScriptableEventAnimationDef& event, target::ScriptableEventDef& result, const unsigned int part_flat_id,
			utils::memory::allocator& allocator)
		{
			result.type = target::Scriptable_EventType_Animation;
			result.base.flags = 0x10262; // TODO: stock animation event value

			auto& animation = result.data.animation;
			animation.base = &result.base;
			animation.animation = convert_animation(event.animation, allocator);
			animation.stateful = event.stateful;
			animation.overrideAnimation = event.overrideAnimation;
			animation.pauseOnExit = event.pauseOnExit;
			animation.clientOnlyInMP = event.clientOnlyInMP;
			animation.startTimeMin = event.startTimeMin;
			animation.startTimeMax = event.startTimeMax;
			animation.startTimeNormalized = event.startTimeNormalized;
			animation.playbackRateMin = event.playbackRateMin;
			animation.playbackRateMax = event.playbackRateMax;
			animation.blendTime = event.blendTime;
			animation.eventAtEndCount = event.eventAtEndCount;
			animation.eventsAtEnd = convert_events(event.eventsAtEnd, event.eventAtEndCount, part_flat_id, allocator);
			animation.eventStreamBufferOffsetClient = event.eventStreamBufferOffsetClient;
			animation.eventStreamBufferOffsetServer = event.eventStreamBufferOffsetServer;
		}

		target::ScriptableEventDef* convert_events(const ScriptableEventDef* events, const unsigned int count, const unsigned int part_flat_id,
			utils::memory::allocator& allocator)
		{
			if (!count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<target::ScriptableEventDef>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& event = events[i];
				result[i].base.name = event.base.name;

				switch (event.type)
				{
				case Scriptable_EventType_Model:
					convert_model_event(event.data.model, result[i], allocator);
					break;
				case Scriptable_EventType_Wait:
					convert_wait_event(event.data.wait, result[i]);
					break;
				case Scriptable_EventType_Sound:
					convert_sound_event(event.data.sound, result[i]);
					break;
				case Scriptable_EventType_StateChange:
					convert_state_change_event(event.data.stateChange, result[i], part_flat_id);
					break;
				default:
					convert_animation_event(event.data.animation, result[i], part_flat_id, allocator);
					break;
				}
			}

			return result;
		}

		target::ScriptableStateDef* convert_states(const ScriptableStateDef* states, const unsigned int count, const unsigned int part_flat_id,
			utils::memory::allocator& allocator)
		{
			if (!count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<target::ScriptableStateDef>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& state = states[i];
				auto& converted = result[i];
				converted.base.name = state.base.name;
				converted.base.flags = 0x10262; // TODO: stock state value
				converted.base.numEvents = state.base.numEvents;
				converted.base.events = convert_events(state.base.events, state.base.numEvents, part_flat_id, allocator);

				if (state.type == Scriptable_StateType_Scripted)
				{
					converted.type = target::Scriptable_StateType_Scripted;
					converted.data.scripted.base = &converted.base;
					converted.data.scripted.script_id = state.data.scripted.script_id;
					converted.data.scripted.scrScript_id = static_cast<target::scr_string_t>(state.data.scripted.scrScript_id);
				}
				else
				{
					converted.type = target::Scriptable_StateType_Simple;
					converted.data.simple.base = &converted.base;
				}
			}

			return result;
		}

		target::ScriptableDamageTagOverride* convert_damage_tag_overrides(const ScriptableDamageTagOverride* overrides, const unsigned int count,
			utils::memory::allocator& allocator)
		{
			if (!count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<target::ScriptableDamageTagOverride>(count);
			for (auto i = 0u; i < count; i++)
			{
				result[i].tag = overrides[i].tag;
				result[i].scrTag = static_cast<target::scr_string_t>(overrides[i].scrTag);
			}

			return result;
		}

		target::ScriptablePartDef* convert_parts(const ScriptablePartDef* parts, const unsigned int count, utils::memory::allocator& allocator)
		{
			if (!count)
			{
				return nullptr;
			}

			const auto result = allocator.allocate_array<target::ScriptablePartDef>(count);
			for (auto i = 0u; i < count; i++)
			{
				const auto& part = parts[i];
				auto& converted = result[i];
				converted.name = part.name;
				converted.scrName = static_cast<target::scr_string_t>(part.scrName);
				converted.flags = 0x10262; // TODO: stock part value
				converted.flatId = part.flatId;
				converted.serverInstanceFlatId = part.serverInstanceFlatId;
				converted.serverControlledFlatId = part.serverControlledFlatId;
				converted.tagName = part.tagName;
				converted.scrTagName = static_cast<target::scr_string_t>(part.scrTagName);
				converted.numStates = part.numStates;
				converted.states = convert_states(part.states, part.numStates, part.flatId, allocator);
				converted.numChildParts = part.numChildParts;
				converted.childParts = convert_parts(part.childParts, part.numChildParts, allocator);
				converted.numDamageTagOverrides = part.numDamageTagOverrides;
				converted.damageTagOverrides = convert_damage_tag_overrides(part.damageTagOverrides, part.numDamageTagOverrides, allocator);
				converted.eventStreamBufferOffsetServer = part.eventStreamBufferOffsetServer;
				converted.eventStreamBufferOffsetClient = part.eventStreamBufferOffsetClient;
			}

			return result;
		}

		target::ScriptableDef* convert(const ScriptableDef* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::ScriptableDef>();
			result->name = asset->name;
			result->flags = 0x410263; // TODO: stock def value
			result->numParts = asset->numParts;
			result->parts = convert_parts(asset->parts, asset->numParts, allocator);
			result->maxNumDynEntsRequired = asset->maxNumDynEntsRequired;
			result->partCount = asset->partCount;
			result->serverInstancedPartCount = asset->serverInstancedPartCount;
			result->serverControlledPartCount = asset->serverControlledPartCount;
			result->maxNumDynEntPartsBase = asset->maxNumDynEntPartsBase;
			result->maxNumDynEntPartsForSpawning = asset->maxNumDynEntPartsForSpawning;
			result->eventStreamSizeRequiredServer = asset->eventStreamSizeRequiredServer;
			result->eventStreamSizeRequiredClient = asset->eventStreamSize; // TODO: inferred from stock
			result->ffMemCost = asset->ffMemCost;
			result->animationTreeName = static_cast<target::scr_string_t>(asset->animationTreeName);
			result->networkLODRangeOverrideDistance = 8000.0f; // TODO: stock def value

			result->numXModels = asset->numXModels;
			if (asset->numXModels)
			{
				result->models = allocator.allocate_array<target::XModel*>(asset->numXModels);
				for (auto i = 0u; i < asset->numXModels; i++)
				{
					result->models[i] = convert_model(asset->models[i], allocator);
				}
			}

			return result;
		}
	}

	void dump(ScriptableDef* asset)
	{
		if (!asset)
		{
			return;
		}

		if (!is_supported(*asset))
		{
			ZONETOOL_WARNING("Scriptable \"%s\" has states or events that are not converted", asset->name);
			return;
		}

		if (!mark_dumped(ASSET_TYPE_SCRIPTABLE, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::iw8::scriptable_def::dump(convert(asset, allocator));
		add_csv_row("scriptable", asset->name);
	}

	ScriptableDef* dump_used(const ScriptableDef* asset)
	{
		if (!asset)
		{
			return nullptr;
		}

		const auto loaded = static_cast<ScriptableDef*>(db_find_asset(ASSET_TYPE_SCRIPTABLE, asset->name));
		if (!loaded || !is_supported(*loaded))
		{
			return nullptr;
		}

		dump(loaded);
		return loaded;
	}
}
