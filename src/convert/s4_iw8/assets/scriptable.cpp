#include <std_include.hpp>

#include "../include.hpp"
#include "scriptable.hpp"
#include "gfximage.hpp"
#include "netconststrings.hpp"

#include <iw8/assets/rawfile.hpp>
#include <iw8/assets/scriptable.hpp>

namespace zonetool::s4::converter::iw8::scriptable
{
	namespace
	{
		namespace source = zonetool::s4;
		namespace target = zonetool::iw8;

		const std::unordered_map<source::Scriptable_EventType, target::Scriptable_EventType> event_types =
		{
			{source::Scriptable_EventType_StateChange, target::Scriptable_EventType_StateChange},
			{source::Scriptable_EventType_Wait, target::Scriptable_EventType_Wait},
			{source::Scriptable_EventType_Random, target::Scriptable_EventType_Random},
			{source::Scriptable_EventType_Script, target::Scriptable_EventType_Script},
			{source::Scriptable_EventType_Model, target::Scriptable_EventType_Model},
			{source::Scriptable_EventType_Unknown8, target::Scriptable_EventType_Collision},
			{source::Scriptable_EventType_Unknown9, target::Scriptable_EventType_Collision},
			{source::Scriptable_EventType_Animation, target::Scriptable_EventType_Animation},
			{source::Scriptable_EventType_HideShowBone, target::Scriptable_EventType_HideShowBone},
			{source::Scriptable_EventType_DisablePhysicsSubShape, target::Scriptable_EventType_DisablePhysicsSubShape},
			{source::Scriptable_EventType_NoteTrack, target::Scriptable_EventType_NoteTrack},
			{source::Scriptable_EventType_ChunkDynent, target::Scriptable_EventType_ChunkDynent},
			{source::Scriptable_EventType_SpawnDynent, target::Scriptable_EventType_SpawnDynent},
			{source::Scriptable_EventType_PFX, target::Scriptable_EventType_PFX},
			{source::Scriptable_EventType_Sound, target::Scriptable_EventType_Sound},
			{source::Scriptable_EventType_Explosion, target::Scriptable_EventType_Explosion},
			{source::Scriptable_EventType_Light, target::Scriptable_EventType_Light},
			{source::Scriptable_EventType_Sun, target::Scriptable_EventType_Sun},
			{source::Scriptable_EventType_Rumble, target::Scriptable_EventType_Rumble},
			{source::Scriptable_EventType_Screenshake, target::Scriptable_EventType_Screenshake},
			{source::Scriptable_EventType_PartDamage, target::Scriptable_EventType_PartDamage},
			{source::Scriptable_EventType_ViewmodelChangeImage, target::Scriptable_EventType_ViewmodelChangeImage},
			{source::Scriptable_EventType_ClientViewSelector, target::Scriptable_EventType_ClientViewSelector},
			{source::Scriptable_EventType_TeamSelector, target::Scriptable_EventType_TeamSelector},
			{source::Scriptable_EventType_AddModel, target::Scriptable_EventType_AddModel},
			{source::Scriptable_EventType_ApplyForce, target::Scriptable_EventType_ApplyForce},
			{source::Scriptable_EventType_ApplyAngularForce, target::Scriptable_EventType_ApplyAngularForce},
			{source::Scriptable_EventType_MaterialOverride, target::Scriptable_EventType_MaterialOverride},
			{source::Scriptable_EventType_Move, target::Scriptable_EventType_Move},
			{source::Scriptable_EventType_SpatialDisable, target::Scriptable_EventType_SpatialDisable},
		};

		std::map<std::string, std::set<std::string>> anim_trees;

		template <typename Target, typename Source>
		void copy_same(Target& result, const Source& value)
		{
			static_assert(sizeof(Target) == sizeof(Source));
			std::memcpy(&result, &value, sizeof(Target));
		}

		int convert_flags(const int flags)
		{
			auto result = flags & 0x6EFF97;
			if (flags & 0x20)
			{
				result |= 0x10020;
			}

			if (flags & 0x10004)
			{
				result |= 0x40;
			}

			return result;
		}

		int convert_flags(const int flags, const child_flags& children)
		{
			return convert_flags(flags) & (convert_flags(flags & ~children.source) | children.converted);
		}

		target::ScriptableEventDef* convert_events(const source::ScriptableEventDef* events, unsigned int count, unsigned int& result_count,
			child_flags& flags, conversion& state);

		void convert_note_tracks(const source::ScriptableEventNoteTrackDef& event, target::ScriptableEventNoteTrackDef& result,
			child_flags& children, conversion& state)
		{
			result.noteTrackCount = event.noteTracks ? event.noteTrackCount : 0;
			result.noteTracks = state.allocator.allocate_array<target::ScriptableNoteTrackDef>(result.noteTrackCount);
			for (auto i = 0u; i < result.noteTrackCount; i++)
			{
				const auto& track = event.noteTracks[i];
				auto& converted = result.noteTracks[i];
				converted.flags = track.flags;
				converted.noteTrackName = track.noteTrackName;
				converted.scrNoteTrackName = static_cast<target::scr_string_t>(track.scrNoteTrackName);
				converted.events = convert_events(track.events, track.numEvents, converted.numEvents, children, state);
			}
		}

		void convert_model_event(const source::ScriptableEventModelDef& event, target::ScriptableEventModelDef& result, conversion& state)
		{
			if (event.dataType == source::SCRIPTABLE_MODEL_DATA_COMPOSITE_MODEL)
			{
				result.dataType = target::SCRIPTABLE_MODEL_DATA_COMPOSITE_MODEL;
				result.data.compositeModel = name_reference<target::XCompositeModelDef>(event.data.compositeModel, state.allocator);
			}
			else if (event.dataType == source::SCRIPTABLE_MODEL_DATA_MODEL)
			{
				result.dataType = target::SCRIPTABLE_MODEL_DATA_MODEL;
				result.data.model = name_reference<target::XModel>(event.data.model, state.allocator);
			}

			result.neverMoves = event.neverMoves;
			result.dynamicSimulation = event.dynamicSimulation;
			result.activatePhysics = event.activatePhysics;
			state.model_events.push_back(&result);
		}

		void convert_collision_event(const source::ScriptableEventUnknown8Def& event, target::ScriptableEventCollisionDef& result)
		{
			result.collmapName = event.collmapName;
			result.aiObstacle = event.aiObstacle;
			result.clipmapCModelIndex = event.clipmapCModelIndex;
			result.neverMoves = event.neverMoves;
			result.canPush = event.canPush;
			result.canTouch = event.canTouch;
			result.destroyOnExit = event.destroyOnExit;
		}

		void convert_model_collision_event(const source::ScriptableEventUnknown9Def& event, target::ScriptableEventCollisionDef& result,
			conversion& state)
		{
			result.collmapName = "";
			result.aiObstacle = event.aiObstacle;
			result.neverMoves = event.neverMoves;
			result.canPush = event.canPush;
			result.canTouch = event.canTouch;
			result.destroyOnExit = event.destroyOnExit;
			result.model = name_reference<target::XModel>(event.model, state.allocator);
		}

		void convert_pfx_event(const source::ScriptableEventPFXDef& event, target::ScriptableEventPFXDef& result, conversion& state)
		{
			static_assert(offsetof(target::ScriptableEventPFXDef, beamLength) == offsetof(source::ScriptableEventPFXDef, beamLength));
			static_assert(offsetof(target::ScriptableEventPFXDef, loopRate) == offsetof(source::ScriptableEventPFXDef, loopRate));

			std::memcpy(&result, &event, sizeof(result));
			result.effectDef.particleSystemDef = name_reference<target::ParticleSystemDef>(event.effectDef.particleSystemDef, state.allocator);
		}

		void convert_sound_event(const source::ScriptableEventSoundDef& event, target::ScriptableEventSoundDef& result)
		{
			result.stateful = event.stateful;
			result.tagName = event.tagName;
			result.scrTagName = static_cast<target::scr_string_t>(event.scrTagName);
			result.soundAlias = event.soundAlias;
			result.looping = event.looping;
			result.externalPlay = event.externalPlay;
			result.allowMissingTag = event.allowMissingTag;
			result.useRootOnMissingTag = event.useRootOnMissingTag;
			result.explosionReflClass = event.explosionReflClass;
			result.groundTraceLength = event.groundTraceLength;
			std::memcpy(result.worldOffset, event.worldOffset, sizeof(result.worldOffset));
		}

		void convert_light_event(const source::ScriptableEventLightDef& event, target::ScriptableEventLightDef& result)
		{
			result.intensityScaleMin = event.intensityScaleMin;
			result.intensityScaleMax = event.intensityScaleMax;
			result.radiusMin = event.radiusMin;
			result.radiusMax = event.radiusMax;
			result.useColor = event.useColor;
			std::memcpy(result.colorLinearSrgb, event.colorLinearSrgb, sizeof(result.colorLinearSrgb));
		}

		void convert_event_data(const source::ScriptableEventDef& event, target::ScriptableEventDef& result, child_flags& children, conversion& state)
		{
			const auto& data = event.data;
			auto& converted = result.data;
			switch (event.type)
			{
			case source::Scriptable_EventType_StateChange:
				copy_same(converted.stateChange, data.stateChange);
				converted.stateChange.part = nullptr;
				state.part_references.push_back(&converted.stateChange.partReference);
				break;
			case source::Scriptable_EventType_Wait:
				copy_same(converted.wait, data.wait);
				break;
			case source::Scriptable_EventType_Random:
				copy_same(converted.random, data.random);
				converted.random.eventsA = convert_events(data.random.eventsA, data.random.eventACount, converted.random.eventACount, children, state);
				converted.random.eventsB = convert_events(data.random.eventsB, data.random.eventBCount, converted.random.eventBCount, children, state);
				break;
			case source::Scriptable_EventType_Script:
				copy_same(converted.script, data.script);
				break;
			case source::Scriptable_EventType_Model:
				convert_model_event(data.model, converted.model, state);
				break;
			case source::Scriptable_EventType_Unknown8:
				convert_collision_event(data.unknown8, converted.collision);
				break;
			case source::Scriptable_EventType_Unknown9:
				convert_model_collision_event(data.unknown9, converted.collision, state);
				break;
			case source::Scriptable_EventType_Animation:
				copy_same(converted.animation, data.animation);
				converted.animation.animation = name_reference<target::XAnimParts>(data.animation.animation, state.allocator);
				if (data.animation.animation)
				{
					state.animations.emplace(data.animation.animation->name);
				}
				converted.animation.eventsAtEnd = convert_events(data.animation.eventsAtEnd, data.animation.eventAtEndCount,
					converted.animation.eventAtEndCount, children, state);
				break;
			case source::Scriptable_EventType_HideShowBone:
				copy_same(converted.hideShowBone, data.hideShowBone);
				break;
			case source::Scriptable_EventType_DisablePhysicsSubShape:
				copy_same(converted.disablePhysicsSubShape, data.disablePhysicsSubShape);
				break;
			case source::Scriptable_EventType_NoteTrack:
				convert_note_tracks(data.noteTrack, converted.noteTrack, children, state);
				break;
			case source::Scriptable_EventType_ChunkDynent:
				copy_same(converted.chunkDynent, data.chunkDynent);
				converted.chunkDynent.part = nullptr;
				state.part_references.push_back(&converted.chunkDynent.partReference);
				break;
			case source::Scriptable_EventType_SpawnDynent:
				copy_same(converted.spawnDynent, data.spawnDynent);
				converted.spawnDynent.model = name_reference<target::XModel>(data.spawnDynent.model, state.allocator);
				converted.spawnDynent.effectDef.particleSystemDef = name_reference<target::ParticleSystemDef>(
					data.spawnDynent.effectDef.particleSystemDef, state.allocator);
				break;
			case source::Scriptable_EventType_PFX:
				convert_pfx_event(data.particleFX, converted.particleFX, state);
				break;
			case source::Scriptable_EventType_Sound:
				convert_sound_event(data.sound, converted.sound);
				break;
			case source::Scriptable_EventType_Explosion:
				copy_same(converted.explosion, data.explosion);
				converted.explosion.weapon = nullptr;
				state.dropped_weapons += data.explosion.weapon != nullptr;
				break;
			case source::Scriptable_EventType_Light:
				convert_light_event(data.light, converted.light);
				break;
			case source::Scriptable_EventType_Sun:
				copy_same(converted.sun, data.sun);
				break;
			case source::Scriptable_EventType_Rumble:
				copy_same(converted.rumble, data.rumble);
				converted.rumble.rumbleAsset = nullptr;
				break;
			case source::Scriptable_EventType_Screenshake:
				copy_same(converted.screenshake, data.screenshake);
				break;
			case source::Scriptable_EventType_PartDamage:
				copy_same(converted.partDamage, data.partDamage);
				converted.partDamage.part = nullptr;
				state.part_references.push_back(&converted.partDamage.partReference);
				break;
			case source::Scriptable_EventType_ViewmodelChangeImage:
				copy_same(converted.viewmodelChangeImage, data.viewmodelChangeImage);
				break;
			case source::Scriptable_EventType_ClientViewSelector:
				copy_same(converted.clientViewSelector, data.clientViewSelector);
				converted.clientViewSelector.events1p = convert_events(data.clientViewSelector.events1p, data.clientViewSelector.event1pCount,
					converted.clientViewSelector.event1pCount, children, state);
				converted.clientViewSelector.events3p = convert_events(data.clientViewSelector.events3p, data.clientViewSelector.event3pCount,
					converted.clientViewSelector.event3pCount, children, state);
				break;
			case source::Scriptable_EventType_TeamSelector:
				copy_same(converted.teamSelector, data.teamSelector);
				converted.teamSelector.eventsPass = convert_events(data.teamSelector.eventsPass, data.teamSelector.eventPassCount,
					converted.teamSelector.eventPassCount, children, state);
				converted.teamSelector.eventsFail = convert_events(data.teamSelector.eventsFail, data.teamSelector.eventFailCount,
					converted.teamSelector.eventFailCount, children, state);
				break;
			case source::Scriptable_EventType_AddModel:
				copy_same(converted.addModel, data.addModel);
				converted.addModel.model = name_reference<target::XModel>(data.addModel.model, state.allocator);
				break;
			case source::Scriptable_EventType_ApplyForce:
				copy_same(converted.applyForce, data.applyForce);
				break;
			case source::Scriptable_EventType_ApplyAngularForce:
				copy_same(converted.applyAngularForce, data.applyAngularForce);
				break;
			case source::Scriptable_EventType_MaterialOverride:
				copy_same(converted.materialOverride, data.materialOverride);
				break;
			case source::Scriptable_EventType_Move:
				copy_same(converted.move, data.move);
				state.moves = true;
				break;
			case source::Scriptable_EventType_SpatialDisable:
				copy_same(converted.spatialDisable, data.spatialDisable);
				break;
			default:
				break;
			}
		}

		unsigned int count_events_before_condition(const source::ScriptableEventDef* events, const unsigned int count)
		{
			if (!events)
			{
				return 0;
			}

			const auto condition = std::find_if(events, events + count, [](const source::ScriptableEventDef& event)
			{
				return event.type == source::Scriptable_EventType_Unknown2;
			});

			return static_cast<unsigned int>(condition - events);
		}

		target::ScriptableEventDef* convert_events(const source::ScriptableEventDef* events, const unsigned int count, unsigned int& result_count,
			child_flags& flags, conversion& state)
		{
			const auto run_count = count_events_before_condition(events, count);

			result_count = 0;
			for (auto i = 0u; events && i < count; i++)
			{
				flags.source |= events[i].base.flags;
				if (i >= run_count)
				{
					state.blocked_events++;
				}
				else if (event_types.contains(events[i].type))
				{
					result_count++;
				}
				else
				{
					state.dropped_events[events[i].type]++;
				}
			}

			if (!result_count)
			{
				return nullptr;
			}

			const auto result = state.allocator.allocate_array<target::ScriptableEventDef>(result_count);
			auto index = 0u;
			for (auto i = 0u; i < run_count; i++)
			{
				const auto type = event_types.find(events[i].type);
				if (type == event_types.end())
				{
					continue;
				}

				auto& converted = result[index++];
				child_flags children{};
				convert_event_data(events[i], converted, children, state);

				converted.type = type->second;
				converted.base.name = events[i].base.name;
				converted.base.flags = convert_flags(events[i].base.flags, children);
				converted.data.anonymous.base = &converted.base;
				flags.converted |= converted.base.flags;
			}

			return result;
		}

		void convert_health_state(const source::ScriptableStateHealthDef& health, target::ScriptableStateHealthDef& result, conversion& state)
		{
			static_assert(sizeof(target::weapClass_t) == sizeof(source::weapClass_t));

			const auto takes_damage = std::any_of(std::begin(health.unknown), std::end(health.unknown), [](const float scale)
			{
				return scale != 0.0f;
			});

			result.health = health.health;
			result.validDamageCause = takes_damage ? target::Scriptable_DamageCause_All : target::Scriptable_DamageCause_None;
			result.validAttackers = target::Scriptable_Attackers_All;
			result.validAttackerTeams = target::Scriptable_Team_Filter_All;
			if (health.weaponClasses && health.numWeaponClasses)
			{
				result.numWeaponClasses = health.numWeaponClasses;
				result.weaponClasses = state.allocator.allocate_array<target::weapClass_t>(health.numWeaponClasses);
				std::memcpy(result.weaponClasses, health.weaponClasses, sizeof(target::weapClass_t) * health.numWeaponClasses);
			}

			state.dropped_weapons += health.weapons ? health.numWeapons : 0;
			result.minimumDamage = health.minimumDamage;
			result.damagePropagationToParent = health.damagePropagationToParent;
			result.damagePropagationToChild = health.damagePropagationToChild;
			result.damagePropagationFromParent = health.damagePropagationFromParent;
			result.damagePropagationFromChild = health.damagePropagationFromChild;
			result.script_id = health.script_id;
			result.scrScript_id = static_cast<target::scr_string_t>(health.scrScript_id);
		}

		target::ScriptableHintHoldDuration convert_hint_hold_duration(const unsigned int duration_ms, conversion& state)
		{
			const std::pair<unsigned int, target::ScriptableHintHoldDuration> durations[] =
			{
				{0, target::SCRIPTABLE_HINT_HOLD_NONE},
				{250, target::SCRIPTABLE_HINT_HOLD_SHORT},
				{1000, target::SCRIPTABLE_HINT_HOLD_MEDIUM},
				{5000, target::SCRIPTABLE_HINT_HOLD_LONG},
			};

			auto nearest = durations[0];
			for (const auto& duration : durations)
			{
				if (std::abs(static_cast<long long>(duration.first) - duration_ms) < std::abs(static_cast<long long>(nearest.first) - duration_ms))
				{
					nearest = duration;
				}
			}

			state.rounded_hint_holds += nearest.first != duration_ms;
			return nearest.second;
		}

		const char* convert_hint_string(const char* hint)
		{
			const std::unordered_map<std::string_view, const char*> hints =
			{
				{"HUD/DOOR_USE_OPEN", "MP/DOOR_USE_OPEN"},
				{"HUD/DOOR_USE_CLOSE", "MP/DOOR_USE_CLOSE"},
			};

			if (!hint)
			{
				return hint;
			}

			const auto found = hints.find(hint);
			return found != hints.end() ? found->second : hint;
		}

		void convert_usable_state(const source::ScriptableStateUsableDef& usable, target::ScriptableStateUsableDef& result, conversion& state)
		{
			result.script_id = usable.script_id;
			result.scrScript_id = static_cast<target::scr_string_t>(usable.scrScript_id);
			result.faceFOV = usable.faceFOV;
			result.useFOV = usable.useFOV;
			result.useRadius = usable.useRadius;
			result.useRadiusAuto = usable.useRadiusAuto;
			result.useRadiusDisplay = usable.useRadiusDisplay;
			result.useRadiusMinimized = usable.useRadiusMinimized;
			result.gotoNextStateOnUse = usable.gotoNextStateOnUse;
			result.allowMissingTag = usable.allowMissingTag;
			result.usableWhenObstructed = usable.usableWhenObstructed;
			std::memcpy(result.useOffset, usable.useOffset, sizeof(vec3_t));
			std::memcpy(result.angleOffset, usable.angleOffset, sizeof(vec3_t));
			result.hintHoldDuration = convert_hint_hold_duration(usable.hintHoldDurationMs, state);
			result.priority = usable.priority;
			result.use2DFOV = usable.use2DFOV;
			result.hintBinding = usable.hintBinding;
			result.hintTag = static_cast<target::scr_string_t>(usable.hintTag);
			result.hintString = convert_hint_string(usable.hintString);
			result.hintStringDisabled = convert_hint_string(usable.hintStringDisabled);
			result.hintIcon = usable.hintIcon;
			result.hintIconImage = gfximage::image_reference(usable.hintIconImage, state.allocator);
		}

		void convert_state(const source::ScriptableStateDef& source_state, target::ScriptableStateDef& result, child_flags& flags, conversion& state)
		{
			child_flags events{};
			result.base.name = source_state.base.name;
			result.base.events = convert_events(source_state.base.events, source_state.base.numEvents, result.base.numEvents, events, state);
			result.base.flags = convert_flags(source_state.base.flags, events);
			result.data.simple.base = &result.base;
			flags.source |= source_state.base.flags;
			flags.converted |= result.base.flags;

			switch (source_state.type)
			{
			case source::Scriptable_StateType_Simple:
				result.type = target::Scriptable_StateType_Simple;
				break;
			case source::Scriptable_StateType_Health:
				result.type = target::Scriptable_StateType_Health;
				convert_health_state(source_state.data.health, result.data.health, state);
				break;
			case source::Scriptable_StateType_Scripted:
				result.type = target::Scriptable_StateType_Scripted;
				result.data.scripted.script_id = source_state.data.scripted.script_id;
				result.data.scripted.scrScript_id = static_cast<target::scr_string_t>(source_state.data.scripted.scrScript_id);
				break;
			case source::Scriptable_StateType_Usable:
				result.type = target::Scriptable_StateType_Usable;
				convert_usable_state(source_state.data.usable, result.data.usable, state);
				break;
			default:
				break;
			}
		}

		void convert_parts(const source::ScriptablePartDef* parts, unsigned int count, target::ScriptablePartDef*& result, unsigned int& result_count,
			child_flags& flags, conversion& state);

		void convert_part(const source::ScriptablePartDef& part, target::ScriptablePartDef& result, conversion& state)
		{
			static_assert(sizeof(target::ScriptableDamageTagOverride) == sizeof(source::ScriptableDamageTagOverride));

			result.name = part.name;
			result.scrName = static_cast<target::scr_string_t>(part.scrName);
			result.tagName = part.tagName;
			result.scrTagName = static_cast<target::scr_string_t>(part.scrTagName);

			child_flags states{};
			if (part.states && part.numStates)
			{
				result.numStates = part.numStates;
				result.states = state.allocator.allocate_array<target::ScriptableStateDef>(part.numStates);
				for (auto i = 0u; i < part.numStates; i++)
				{
					convert_state(part.states[i], result.states[i], states, state);
				}
			}

			result.flags = convert_flags(part.flags, states);

			child_flags child_parts{};
			convert_parts(part.childParts, part.numChildParts, result.childParts, result.numChildParts, child_parts, state);

			if (part.damageTagOverrides && part.numDamageTagOverrides)
			{
				result.numDamageTagOverrides = part.numDamageTagOverrides;
				result.damageTagOverrides = state.allocator.allocate_array<target::ScriptableDamageTagOverride>(part.numDamageTagOverrides);
				std::memcpy(result.damageTagOverrides, part.damageTagOverrides, sizeof(target::ScriptableDamageTagOverride) * part.numDamageTagOverrides);
			}
		}

		void convert_parts(const source::ScriptablePartDef* parts, const unsigned int count, target::ScriptablePartDef*& result, unsigned int& result_count,
			child_flags& flags, conversion& state)
		{
			if (!parts || !count)
			{
				return;
			}

			result_count = count;
			result = state.allocator.allocate_array<target::ScriptablePartDef>(count);
			for (auto i = 0u; i < count; i++)
			{
				convert_part(parts[i], result[i], state);
				flags.source |= parts[i].flags;
				flags.converted |= result[i].flags;
			}
		}

		bool has_usable_state(const target::ScriptablePartDef& part)
		{
			return std::any_of(part.states, part.states + part.numStates, [](const target::ScriptableStateDef& state)
			{
				return state.type == target::Scriptable_StateType_Usable;
			});
		}

		void assign_flat_ids(target::ScriptablePartDef& part, target::ScriptableDef& def)
		{
			for (auto i = 0u; i < part.numChildParts; i++)
			{
				assign_flat_ids(part.childParts[i], def);
			}

			part.flatId = def.partCount++;
			part.serverInstanceFlatId = part.flags & 0x2 ? def.serverInstancedPartCount++ : 0;
			part.serverControlledFlatId = part.flags & 0x4 ? def.serverControlledPartCount++ : 0;
			def.usablePartCount += has_usable_state(part);
		}

		void map_flat_ids(const source::ScriptablePartDef* parts, const target::ScriptablePartDef* result, const unsigned int count,
			std::unordered_map<unsigned int, unsigned int>& flat_ids)
		{
			for (auto i = 0u; i < count; i++)
			{
				flat_ids[parts[i].flatId] = result[i].flatId;
				map_flat_ids(parts[i].childParts, result[i].childParts, result[i].numChildParts, flat_ids);
			}
		}

		void resolve_part_references(const conversion& state, const std::unordered_map<unsigned int, unsigned int>& flat_ids)
		{
			for (const auto reference : state.part_references)
			{
				const auto flat_id = flat_ids.find(reference->flatId & 0xFF);
				if (flat_id == flat_ids.end())
				{
					ZONETOOL_ERROR("scriptable \"%s\": part reference 0x%08X has no part", state.name, reference->flatId);
					continue;
				}

				reference->flatId = flat_id->second;
			}
		}

		unsigned short take(unsigned short& offset, const unsigned short size)
		{
			const auto result = offset;
			offset += size;
			return result;
		}

		unsigned short take_part_slot(std::optional<unsigned short>& slot, unsigned short& offset, const unsigned short size)
		{
			if (!slot)
			{
				slot = take(offset, size);
			}

			return *slot;
		}

		void assign_event_offsets(target::ScriptableEventDef* events, unsigned int count, event_stream& stream);

		void assign_event_offsets(target::ScriptableEventDef& event, event_stream& stream)
		{
			auto& data = event.data;
			switch (event.type)
			{
			case target::Scriptable_EventType_Wait:
				data.wait.eventStreamBufferOffsetServer = take(stream.server, 4);
				data.wait.eventStreamBufferOffsetClient = take(stream.client, 4);
				break;
			case target::Scriptable_EventType_Random:
				assign_event_offsets(data.random.eventsA, data.random.eventACount, stream);
				assign_event_offsets(data.random.eventsB, data.random.eventBCount, stream);
				data.random.eventStreamBufferOffsetServer = take(stream.server, 4);
				data.random.eventStreamBufferOffsetClient = take(stream.client, 4);
				break;
			case target::Scriptable_EventType_Animation:
				assign_event_offsets(data.animation.eventsAtEnd, data.animation.eventAtEndCount, stream);
				data.animation.eventStreamBufferOffsetClient = take_part_slot(stream.part.animation, stream.client, 12);
				data.animation.eventStreamBufferOffsetServer = 0;
				break;
			case target::Scriptable_EventType_NoteTrack:
				for (auto i = 0u; i < data.noteTrack.noteTrackCount; i++)
				{
					assign_event_offsets(data.noteTrack.noteTracks[i].events, data.noteTrack.noteTracks[i].numEvents, stream);
				}
				break;
			case target::Scriptable_EventType_SpawnDynent:
				data.spawnDynent.eventStreamBufferOffsetClient = take(stream.client, 4);
				break;
			case target::Scriptable_EventType_PFX:
				data.particleFX.eventStreamBufferOffsetClient = data.particleFX.oneshotLooping ? take(stream.client, 8) : 0;
				break;
			case target::Scriptable_EventType_Light:
				data.light.eventStreamBufferOffsetClient = take_part_slot(stream.part.light, stream.client, 48); // IW8 uses 0x24 (0x12FB270)
				break;
			case target::Scriptable_EventType_PartDamage:
				data.partDamage.eventStreamBufferOffsetServer = data.partDamage.intervalDoT != 0.0f ? take(stream.server, 4) : 0;
				data.partDamage.eventStreamBufferOffsetClient = data.partDamage.intervalDoT != 0.0f ? take(stream.client, 4) : 0;
				break;
			case target::Scriptable_EventType_ClientViewSelector:
				assign_event_offsets(data.clientViewSelector.events1p, data.clientViewSelector.event1pCount, stream);
				assign_event_offsets(data.clientViewSelector.events3p, data.clientViewSelector.event3pCount, stream);
				data.clientViewSelector.eventStreamBufferOffsetClient = take(stream.client, 4);
				break;
			case target::Scriptable_EventType_TeamSelector:
				assign_event_offsets(data.teamSelector.eventsPass, data.teamSelector.eventPassCount, stream);
				assign_event_offsets(data.teamSelector.eventsFail, data.teamSelector.eventFailCount, stream);
				data.teamSelector.eventStreamBufferOffsetClient = take(stream.client, 4);
				data.teamSelector.eventStreamBufferOffsetServer = 0;
				break;
			case target::Scriptable_EventType_MaterialOverride:
				data.materialOverride.eventStreamBufferOffsetClient = take(stream.client, 5);
				break;
			case target::Scriptable_EventType_Move:
				data.move.eventStreamBufferOffsetServer = take_part_slot(stream.part.move_server, stream.server, 28);
				data.move.eventStreamBufferOffsetClient = take_part_slot(stream.part.move_client, stream.client, 28);
				break;
			default:
				break;
			}
		}

		void assign_event_offsets(target::ScriptableEventDef* events, const unsigned int count, event_stream& stream)
		{
			for (auto i = 0u; i < count; i++)
			{
				assign_event_offsets(events[i], stream);
			}
		}

		void assign_event_offsets(target::ScriptablePartDef& part, event_stream& stream)
		{
			stream.part = {};
			for (auto i = 0u; i < part.numStates; i++)
			{
				assign_event_offsets(part.states[i].base.events, part.states[i].base.numEvents, stream);
			}

			for (auto i = 0u; i < part.numChildParts; i++)
			{
				assign_event_offsets(part.childParts[i], stream);
			}
		}

		void assign_part_offsets(target::ScriptablePartDef& part, event_stream& stream)
		{
			part.eventStreamBufferOffsetServer = stream.server;
			part.eventStreamBufferOffsetClient = stream.client;
			if (part.flags & 0x80)
			{
				stream.client += 4;
				stream.server += part.flags & 0x2 ? 4 : 0;
			}

			for (auto i = 0u; i < part.numChildParts; i++)
			{
				assign_part_offsets(part.childParts[i], stream);
			}
		}

		void assign_event_stream(target::ScriptableDef& def)
		{
			event_stream stream{};
			for (auto i = 0u; i < def.numParts; i++)
			{
				assign_event_offsets(def.parts[i], stream);
			}

			for (auto i = 0u; i < def.numParts; i++)
			{
				assign_part_offsets(def.parts[i], stream);
			}

			def.eventStreamSizeRequiredServer = stream.server;
			def.eventStreamSizeRequiredClient = stream.client;
		}

		target::scr_string_t convert_anim_tree(const scr_string_t tree, const conversion& state)
		{
			if (!tree || state.animations.empty())
			{
				return static_cast<target::scr_string_t>(tree);
			}

			const auto name = std::format("s4_{}", SL_ConvertToString(tree));
			anim_trees[name].insert(state.animations.begin(), state.animations.end());
			return static_cast<target::scr_string_t>(SL_GetString(name.data()));
		}

		void let_models_move(const conversion& state)
		{
			if (!state.moves)
			{
				return;
			}

			for (const auto model : state.model_events)
			{
				model->neverMoves = false; // stock IW8 movers
			}
		}

		void log_losses(const source::ScriptableDef* asset, const conversion& state)
		{
			for (const auto& [type, count] : state.dropped_events)
			{
				ZONETOOL_WARNING("scriptable \"%s\": %u events of S4 type 0x%X have no IW8 event", asset->name, count, type);
			}

			if (state.blocked_events)
			{
				ZONETOOL_WARNING("scriptable \"%s\" loses %u events that wait for an S4 condition", asset->name, state.blocked_events);
			}

			if (state.rounded_hint_holds)
			{
				ZONETOOL_WARNING("scriptable \"%s\": %u hint hold durations have no IW8 value, they use the nearest IW8 value", asset->name, state.rounded_hint_holds);
			}

			if (state.dropped_weapons)
			{
				ZONETOOL_WARNING("scriptable \"%s\" loses %u weapon references", asset->name, state.dropped_weapons);
			}

			if (asset->extra)
			{
				ZONETOOL_WARNING("scriptable \"%s\" loses its extra data, IW8 has no such data", asset->name);
			}
		}
	}

	target::ScriptableDef* convert(const ScriptableDef* asset, utils::memory::allocator& allocator)
	{
		conversion state{asset->name, allocator, {}, 0};

		const auto result = allocator.allocate<target::ScriptableDef>();
		result->name = asset->name;
		result->nextScriptableDef = name_reference<target::ScriptableDef>(asset->nextScriptableDef, allocator);

		child_flags parts{};
		convert_parts(asset->parts, asset->numParts, result->parts, result->numParts, parts, state);
		result->flags = convert_flags(asset->flags, parts);

		let_models_move(state);

		result->maxNumDynEntsRequired = asset->maxNumDynEntsRequired;
		result->maxNumDynEntPartsBase = asset->maxNumDynEntPartsBase;
		result->maxNumDynEntPartsForSpawning = asset->maxNumDynEntPartsForSpawning;
		result->ffMemCost = asset->ffMemCost;
		result->viewRadius = asset->viewRadius;
		result->networkLODRangeOverride = asset->networkLODRangeOverride;
		result->networkLODRangeOverrideDistance = asset->networkLODRangeOverrideDistance;

		if (asset->models && asset->numXModels)
		{
			result->numXModels = asset->numXModels;
			result->models = allocator.allocate_array<target::XModel*>(asset->numXModels);
			for (auto i = 0u; i < asset->numXModels; i++)
			{
				result->models[i] = name_reference<target::XModel>(asset->models[i], allocator);
			}
		}

		for (auto i = 0u; i < result->numParts; i++)
		{
			assign_flat_ids(result->parts[i], *result);
		}

		std::unordered_map<unsigned int, unsigned int> flat_ids;
		map_flat_ids(asset->parts, result->parts, result->numParts, flat_ids);
		resolve_part_references(state, flat_ids);

		result->animationTreeName = convert_anim_tree(asset->animationTreeName, state);
		assign_event_stream(*result);
		log_losses(asset, state);
		return result;
	}

	int converted_flags(const ScriptableDef* asset)
	{
		utils::memory::allocator allocator;
		conversion state{asset->name, allocator, {}, 0};

		target::ScriptablePartDef* parts{};
		unsigned int part_count{};
		child_flags children{};
		convert_parts(asset->parts, asset->numParts, parts, part_count, children, state);
		return convert_flags(asset->flags, children);
	}

	void dump(ScriptableDef* asset)
	{
		utils::memory::allocator allocator;
		const auto converted = convert(asset, allocator);
		target::scriptable_def::dump(converted);
		add_csv_row(target::type_to_string(target::ASSET_TYPE_SCRIPTABLE), asset->name);
	}

	void dump_anim_trees()
	{
		for (const auto& [tree, animations] : anim_trees)
		{
			std::string buffer;
			for (const auto& animation : animations)
			{
				buffer += animation + "\n";
			}

			const auto name = std::format("animtrees/{}.atr", tree);
			target::RawFile rawfile{name.data(), 0, static_cast<int>(buffer.size()), buffer.data()};
			target::rawfile::dump(&rawfile);
			add_csv_row(target::type_to_string(target::ASSET_TYPE_RAWFILE), name);
			netconststrings::add_level_string(target::NETCONSTSTRINGTYPE_ATR, tree);
		}

		anim_trees.clear();
	}
}
