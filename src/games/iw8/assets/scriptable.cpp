#include <std_include.hpp>
#include "scriptable.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_events(zone_reader& reader, ScriptableEventDef*& field, unsigned int count);

		void read_event_base(zone_reader& reader, ScriptableEventBaseDef*& field)
		{
			if (const auto base = reader.read_single(field, 7))
			{
				reader.read_string(base->name);
			}
		}

		void read_tag(zone_reader& reader, const char*& name, scr_string_t& scr_name)
		{
			reader.read_string(name);
			reader.read_script_string(scr_name);
		}

		void read_model_event(zone_reader& reader, ScriptableEventModelDef& event)
		{
			if (event.dataType == SCRIPTABLE_MODEL_DATA_COMPOSITE_MODEL)
			{
				reader.read_asset(ASSET_TYPE_XCOMPOSITEMODEL, event.data.compositeModel);
			}
			else if (event.dataType == SCRIPTABLE_MODEL_DATA_MODEL)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, event.data.model);
			}
		}

		void read_note_tracks(zone_reader& reader, ScriptableEventNoteTrackDef& event)
		{
			if (const auto note_tracks = reader.read_array(event.noteTracks, 7, event.noteTrackCount))
			{
				for (auto i = 0u; i < event.noteTrackCount; i++)
				{
					read_tag(reader, note_tracks[i].noteTrackName, note_tracks[i].scrNoteTrackName);
					read_events(reader, note_tracks[i].events, note_tracks[i].numEvents);
				}
			}
		}

		void read_compass_icon(zone_reader& reader, ScriptableEventCompassIconDef& event)
		{
			for (auto material : {&event.friendlyArrow, &event.friendlyFiring, &event.friendlyChatting, &event.friendlyYelling,
				&event.partyArrow, &event.partyFiring, &event.partyChatting, &event.partyYelling, &event.squadArrow, &event.squadFiring})
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, *material);
			}

			for (auto& material : event.enemyCompassIconQuiet)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, material);
			}

			for (auto& material : event.enemyCompassIconFiring)
			{
				reader.read_asset(ASSET_TYPE_MATERIAL, material);
			}

			reader.read_asset(ASSET_TYPE_MATERIAL, event.enemyCompassIconDirectional);
		}

		void read_event_data(zone_reader& reader, ScriptableEventDef& event)
		{
			auto& data = event.data;
			read_event_base(reader, data.anonymous.base);

			switch (event.type)
			{
			case Scriptable_EventType_Random:
				read_events(reader, data.random.eventsA, data.random.eventACount);
				read_events(reader, data.random.eventsB, data.random.eventBCount);
				break;
			case Scriptable_EventType_Script:
				read_tag(reader, data.script.notification, data.script.scrNotification);
				break;
			case Scriptable_EventType_Model:
				read_model_event(reader, data.model);
				break;
			case Scriptable_EventType_Collision:
				reader.read_string(data.collision.collmapName);
				reader.read_asset(ASSET_TYPE_XMODEL, data.collision.model);
				break;
			case Scriptable_EventType_Animation:
				reader.read_asset(ASSET_TYPE_XANIMPARTS, data.animation.animation);
				read_events(reader, data.animation.eventsAtEnd, data.animation.eventAtEndCount);
				break;
			case Scriptable_EventType_HideShowBone:
				read_tag(reader, data.hideShowBone.tagName, data.hideShowBone.scrTagName);
				break;
			case Scriptable_EventType_DisablePhysicsSubShape:
				reader.read_string(data.disablePhysicsSubShape.mutableShapeName);
				break;
			case Scriptable_EventType_NoteTrack:
				read_note_tracks(reader, data.noteTrack);
				break;
			case Scriptable_EventType_SpawnDynent:
				reader.read_asset(ASSET_TYPE_XMODEL, data.spawnDynent.model);
				read_tag(reader, data.spawnDynent.tagName, data.spawnDynent.scrTagName);
				reader.read_string(data.spawnDynent.effectAlias);
				reader.read_asset(ASSET_TYPE_VFX, data.spawnDynent.effectDef.particleSystemDef);
				break;
			case Scriptable_EventType_PFX:
				reader.read_script_string_array(data.particleFX.scrTagNames, data.particleFX.scrTagCount);
				reader.read_script_string_array(data.particleFX.scrEndTagNames, data.particleFX.scrEndTagCount);
				reader.read_string(data.particleFX.effectAlias);
				reader.read_asset(ASSET_TYPE_VFX, data.particleFX.effectDef.particleSystemDef);
				break;
			case Scriptable_EventType_Sound:
				read_tag(reader, data.sound.tagName, data.sound.scrTagName);
				reader.read_string(data.sound.soundAlias);
				reader.read_string(data.sound.explosionReflClass);
				break;
			case Scriptable_EventType_Explosion:
				read_tag(reader, data.explosion.tagName, data.explosion.scrTagName);
				reader.read_asset(ASSET_TYPE_WEAPON, data.explosion.weapon);
				break;
			case Scriptable_EventType_Rumble:
				read_tag(reader, data.rumble.tagName, data.rumble.scrTagName);
				reader.read_string(data.rumble.rumble);
				reader.read_asset(ASSET_TYPE_RUMBLE, data.rumble.rumbleAsset);
				break;
			case Scriptable_EventType_Screenshake:
				read_tag(reader, data.screenshake.tagName, data.screenshake.scrTagName);
				break;
			case Scriptable_EventType_SetMayhem:
				reader.read_asset(ASSET_TYPE_MAYHEM, data.setMayhem.mayhem);
				break;
			case Scriptable_EventType_ClientViewSelector:
				read_events(reader, data.clientViewSelector.events1p, data.clientViewSelector.event1pCount);
				read_events(reader, data.clientViewSelector.events3p, data.clientViewSelector.event3pCount);
				break;
			case Scriptable_EventType_TeamSelector:
				read_events(reader, data.teamSelector.eventsPass, data.teamSelector.eventPassCount);
				read_events(reader, data.teamSelector.eventsFail, data.teamSelector.eventFailCount);
				break;
			case Scriptable_EventType_AddModel:
				read_tag(reader, data.addModel.tagName, data.addModel.scrTagName);
				reader.read_asset(ASSET_TYPE_XMODEL, data.addModel.model);
				break;
			case Scriptable_EventType_CompassIcon:
				read_compass_icon(reader, data.compassIcon);
				break;
			case Scriptable_EventType_DynamicBoneNoiseCurve:
				reader.read_asset(ASSET_TYPE_XANIMCURVE, data.dynamicBoneNoiseCurve.curve);
				break;
			case Scriptable_EventType_Footstep:
				read_tag(reader, data.footstep.tagName, data.footstep.scrTagName);
				reader.read_string(data.footstep.soundAlias);
				reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, data.footstep.rightFootstepVFX);
				reader.read_asset(ASSET_TYPE_FOOTSTEP_VFX, data.footstep.leftFootstepVFX);
				break;
			case Scriptable_EventType_GravityArc:
				read_events(reader, data.gravityArc.eventsAtEnd, data.gravityArc.eventAtEndCount);
				break;
			case Scriptable_EventType_Objective:
				reader.read_string(data.objective.description);
				reader.read_string(data.objective.icon);
				reader.read_string(data.objective.label);
				reader.read_asset(ASSET_TYPE_IMAGE, data.objective.iconImage);
				break;
			default:
				break;
			}
		}

		void read_events(zone_reader& reader, ScriptableEventDef*& field, const unsigned int count)
		{
			if (const auto events = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					reader.read_string(events[i].base.name);
					read_event_data(reader, events[i]);
				}
			}
		}

		void read_state_base(zone_reader& reader, ScriptableStateBaseDef& base)
		{
			reader.read_string(base.name);
			read_events(reader, base.events, base.numEvents);
		}

		void read_state_data(zone_reader& reader, ScriptableStateDef& state)
		{
			auto& data = state.data;
			if (const auto base = reader.read_single(data.simple.base, 7))
			{
				read_state_base(reader, *base);
			}

			switch (state.type)
			{
			case Scriptable_StateType_Health:
				reader.read_array(data.health.weaponClasses, 3, data.health.numWeaponClasses);
				reader.read_asset_array(ASSET_TYPE_WEAPON, data.health.weapons, data.health.numWeapons);
				read_tag(reader, data.health.script_id, data.health.scrScript_id);
				break;
			case Scriptable_StateType_Scripted:
				read_tag(reader, data.scripted.script_id, data.scripted.scrScript_id);
				break;
			case Scriptable_StateType_Usable:
				read_tag(reader, data.usable.script_id, data.usable.scrScript_id);
				reader.read_script_string(data.usable.hintTag);
				reader.read_string(data.usable.hintString);
				reader.read_string(data.usable.hintStringDisabled);
				reader.read_string(data.usable.hintIcon);
				reader.read_asset(ASSET_TYPE_IMAGE, data.usable.hintIconImage);
				break;
			default:
				break;
			}
		}

		void read_parts(zone_reader& reader, ScriptablePartDef*& field, unsigned int count);

		void read_part(zone_reader& reader, ScriptablePartDef& part)
		{
			reader.read_string(part.name);
			reader.read_script_string(part.scrName);
			read_tag(reader, part.tagName, part.scrTagName);

			if (const auto states = reader.read_array(part.states, 7, part.numStates))
			{
				for (auto i = 0u; i < part.numStates; i++)
				{
					read_state_base(reader, states[i].base);
					read_state_data(reader, states[i]);
				}
			}

			read_parts(reader, part.childParts, part.numChildParts);

			if (const auto overrides = reader.read_array(part.damageTagOverrides, 7, part.numDamageTagOverrides))
			{
				for (auto i = 0u; i < part.numDamageTagOverrides; i++)
				{
					read_tag(reader, overrides[i].tag, overrides[i].scrTag);
				}
			}
		}

		void read_parts(zone_reader& reader, ScriptablePartDef*& field, const unsigned int count)
		{
			if (const auto parts = reader.read_array(field, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					read_part(reader, parts[i]);
				}
			}
		}
	}

	void scriptable_def::read(zone_reader& reader, ScriptableDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->nextScriptableDef);
		read_parts(reader, asset->parts, asset->numParts);
		reader.read_script_string(asset->animationTreeName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->models, asset->numXModels);
		reader.pop_stream();
	}

	void composite_model::read(zone_reader& reader, XCompositeModelDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto submodels = reader.read_array(asset->submodels, 7, 10))
		{
			for (auto i = 0; i < 10; i++)
			{
				reader.read_string(submodels[i].modelName);
				reader.read_script_string(submodels[i].parentBoneName);
				reader.read_asset(ASSET_TYPE_XMODEL, submodels[i].model);
			}
		}

		if (const auto composites = reader.read_array(asset->subCompositeModels, 7, 10))
		{
			for (auto i = 0; i < 10; i++)
			{
				reader.read_string(composites[i].compositeModelName);
				reader.read_script_string(composites[i].parentBoneName);
				reader.read_asset(ASSET_TYPE_XCOMPOSITEMODEL, composites[i].compositeModel);
			}
		}
		reader.pop_stream();
	}
}
