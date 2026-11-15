#include <std_include.hpp>
#include "scriptable.hpp"

namespace zonetool::iw8
{
	namespace
	{
		template <typename Walk>
		void walk_events(Walk walk, ScriptableEventDef*& field, unsigned int count);

		template <typename Walk>
		void walk_tag(Walk walk, const char*& name, scr_string_t& scr_name)
		{
			walk.string(name);
			walk.script_string(scr_name);
		}

		template <typename Walk>
		void walk_model_event(Walk walk, ScriptableEventModelDef& event)
		{
			if (event.dataType == SCRIPTABLE_MODEL_DATA_COMPOSITE_MODEL)
			{
				walk.asset(ASSET_TYPE_XCOMPOSITEMODEL, event.data.compositeModel);
			}
			else if (event.dataType == SCRIPTABLE_MODEL_DATA_MODEL)
			{
				walk.asset(ASSET_TYPE_XMODEL, event.data.model);
			}
		}

		template <typename Walk>
		void walk_note_tracks(Walk walk, ScriptableEventNoteTrackDef& event)
		{
			if (const auto note_tracks = walk.array(event.noteTracks, event.noteTrackCount, 7))
			{
				for (auto i = 0u; i < event.noteTrackCount; i++)
				{
					walk_tag(walk, note_tracks[i].noteTrackName, note_tracks[i].scrNoteTrackName);
					walk_events(walk, note_tracks[i].events, note_tracks[i].numEvents);
				}
			}
		}

		template <typename Walk>
		void walk_compass_icon(Walk walk, ScriptableEventCompassIconDef& event)
		{
			for (auto material : {&event.friendlyArrow, &event.friendlyFiring, &event.friendlyChatting, &event.friendlyYelling,
				&event.partyArrow, &event.partyFiring, &event.partyChatting, &event.partyYelling, &event.squadArrow, &event.squadFiring})
			{
				walk.asset(ASSET_TYPE_MATERIAL, *material);
			}

			for (auto& material : event.enemyCompassIconQuiet)
			{
				walk.asset(ASSET_TYPE_MATERIAL, material);
			}

			for (auto& material : event.enemyCompassIconFiring)
			{
				walk.asset(ASSET_TYPE_MATERIAL, material);
			}

			walk.asset(ASSET_TYPE_MATERIAL, event.enemyCompassIconDirectional);
		}

		template <typename Walk>
		void walk_event_data(Walk walk, ScriptableEventDef& event)
		{
			auto& data = event.data;
			if (const auto base = walk.array(data.anonymous.base, 1, 7))
			{
				walk.string(base->name);
			}

			switch (event.type)
			{
			case Scriptable_EventType_Random:
				walk_events(walk, data.random.eventsA, data.random.eventACount);
				walk_events(walk, data.random.eventsB, data.random.eventBCount);
				break;
			case Scriptable_EventType_Script:
				walk_tag(walk, data.script.notification, data.script.scrNotification);
				break;
			case Scriptable_EventType_Model:
				walk_model_event(walk, data.model);
				break;
			case Scriptable_EventType_Collision:
				walk.string(data.collision.collmapName);
				walk.asset(ASSET_TYPE_XMODEL, data.collision.model);
				break;
			case Scriptable_EventType_Animation:
				walk.asset(ASSET_TYPE_XANIMPARTS, data.animation.animation);
				walk_events(walk, data.animation.eventsAtEnd, data.animation.eventAtEndCount);
				break;
			case Scriptable_EventType_HideShowBone:
				walk_tag(walk, data.hideShowBone.tagName, data.hideShowBone.scrTagName);
				break;
			case Scriptable_EventType_DisablePhysicsSubShape:
				walk.string(data.disablePhysicsSubShape.mutableShapeName);
				break;
			case Scriptable_EventType_NoteTrack:
				walk_note_tracks(walk, data.noteTrack);
				break;
			case Scriptable_EventType_SpawnDynent:
				walk.asset(ASSET_TYPE_XMODEL, data.spawnDynent.model);
				walk_tag(walk, data.spawnDynent.tagName, data.spawnDynent.scrTagName);
				walk.string(data.spawnDynent.effectAlias);
				walk.asset(ASSET_TYPE_VFX, data.spawnDynent.effectDef.particleSystemDef);
				break;
			case Scriptable_EventType_PFX:
				walk.script_strings(data.particleFX.scrTagNames, data.particleFX.scrTagCount);
				walk.script_strings(data.particleFX.scrEndTagNames, data.particleFX.scrEndTagCount);
				walk.string(data.particleFX.effectAlias);
				walk.asset(ASSET_TYPE_VFX, data.particleFX.effectDef.particleSystemDef);
				break;
			case Scriptable_EventType_Sound:
				walk_tag(walk, data.sound.tagName, data.sound.scrTagName);
				walk.string(data.sound.soundAlias);
				walk.string(data.sound.explosionReflClass);
				break;
			case Scriptable_EventType_Explosion:
				walk_tag(walk, data.explosion.tagName, data.explosion.scrTagName);
				walk.asset(ASSET_TYPE_WEAPON, data.explosion.weapon);
				break;
			case Scriptable_EventType_Rumble:
				walk_tag(walk, data.rumble.tagName, data.rumble.scrTagName);
				walk.string(data.rumble.rumble);
				walk.asset(ASSET_TYPE_RUMBLE, data.rumble.rumbleAsset);
				break;
			case Scriptable_EventType_Screenshake:
				walk_tag(walk, data.screenshake.tagName, data.screenshake.scrTagName);
				break;
			case Scriptable_EventType_SetMayhem:
				walk.asset(ASSET_TYPE_MAYHEM, data.setMayhem.mayhem);
				break;
			case Scriptable_EventType_ClientViewSelector:
				walk_events(walk, data.clientViewSelector.events1p, data.clientViewSelector.event1pCount);
				walk_events(walk, data.clientViewSelector.events3p, data.clientViewSelector.event3pCount);
				break;
			case Scriptable_EventType_TeamSelector:
				walk_events(walk, data.teamSelector.eventsPass, data.teamSelector.eventPassCount);
				walk_events(walk, data.teamSelector.eventsFail, data.teamSelector.eventFailCount);
				break;
			case Scriptable_EventType_AddModel:
				walk_tag(walk, data.addModel.tagName, data.addModel.scrTagName);
				walk.asset(ASSET_TYPE_XMODEL, data.addModel.model);
				break;
			case Scriptable_EventType_CompassIcon:
				walk_compass_icon(walk, data.compassIcon);
				break;
			case Scriptable_EventType_DynamicBoneNoiseCurve:
				walk.asset(ASSET_TYPE_XANIMCURVE, data.dynamicBoneNoiseCurve.curve);
				break;
			case Scriptable_EventType_Footstep:
				walk_tag(walk, data.footstep.tagName, data.footstep.scrTagName);
				walk.string(data.footstep.soundAlias);
				walk.asset(ASSET_TYPE_FOOTSTEP_VFX, data.footstep.rightFootstepVFX);
				walk.asset(ASSET_TYPE_FOOTSTEP_VFX, data.footstep.leftFootstepVFX);
				break;
			case Scriptable_EventType_GravityArc:
				walk_events(walk, data.gravityArc.eventsAtEnd, data.gravityArc.eventAtEndCount);
				break;
			case Scriptable_EventType_Objective:
				walk.string(data.objective.description);
				walk.string(data.objective.icon);
				walk.string(data.objective.label);
				walk.asset(ASSET_TYPE_IMAGE, data.objective.iconImage);
				break;
			default:
				break;
			}
		}

		template <typename Walk>
		void walk_events(Walk walk, ScriptableEventDef*& field, const unsigned int count)
		{
			if (const auto events = walk.array(field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk.string(events[i].base.name);
					walk_event_data(walk, events[i]);
				}
			}
		}

		template <typename Walk>
		void walk_state_base(Walk walk, ScriptableStateBaseDef& base)
		{
			walk.string(base.name);
			walk_events(walk, base.events, base.numEvents);
		}

		template <typename Walk>
		void walk_state_data(Walk walk, ScriptableStateDef& state)
		{
			auto& data = state.data;
			if (const auto base = walk.array(data.simple.base, 1, 7))
			{
				walk_state_base(walk, *base);
			}

			switch (state.type)
			{
			case Scriptable_StateType_Health:
				walk.array(data.health.weaponClasses, data.health.numWeaponClasses, 3);
				walk_asset_array(walk, ASSET_TYPE_WEAPON, data.health.weapons, data.health.numWeapons);
				walk_tag(walk, data.health.script_id, data.health.scrScript_id);
				break;
			case Scriptable_StateType_Scripted:
				walk_tag(walk, data.scripted.script_id, data.scripted.scrScript_id);
				break;
			case Scriptable_StateType_Usable:
				walk_tag(walk, data.usable.script_id, data.usable.scrScript_id);
				walk.script_string(data.usable.hintTag);
				walk.string(data.usable.hintString);
				walk.string(data.usable.hintStringDisabled);
				walk.string(data.usable.hintIcon);
				walk.asset(ASSET_TYPE_IMAGE, data.usable.hintIconImage);
				break;
			default:
				break;
			}
		}

		template <typename Walk>
		void walk_parts(Walk walk, ScriptablePartDef*& field, unsigned int count);

		template <typename Walk>
		void walk_part(Walk walk, ScriptablePartDef& part)
		{
			walk.string(part.name);
			walk.script_string(part.scrName);
			walk_tag(walk, part.tagName, part.scrTagName);

			if (const auto states = walk.array(part.states, part.numStates, 7))
			{
				for (auto i = 0u; i < part.numStates; i++)
				{
					walk_state_base(walk, states[i].base);
					walk_state_data(walk, states[i]);
				}
			}

			walk_parts(walk, part.childParts, part.numChildParts);

			if (const auto overrides = walk.array(part.damageTagOverrides, part.numDamageTagOverrides, 7))
			{
				for (auto i = 0u; i < part.numDamageTagOverrides; i++)
				{
					walk_tag(walk, overrides[i].tag, overrides[i].scrTag);
				}
			}
		}

		template <typename Walk>
		void walk_parts(Walk walk, ScriptablePartDef*& field, const unsigned int count)
		{
			if (const auto parts = walk.array(field, count, 7))
			{
				for (auto i = 0u; i < count; i++)
				{
					walk_part(walk, parts[i]);
				}
			}
		}

		template <typename Walk>
		void walk_scriptable(Walk walk, ScriptableDef& asset)
		{
			walk.asset(ASSET_TYPE_SCRIPTABLE, asset.nextScriptableDef);
			walk_parts(walk, asset.parts, asset.numParts);
			walk.script_string(asset.animationTreeName);
			walk_asset_array(walk, ASSET_TYPE_XMODEL, asset.models, asset.numXModels);
		}

		template <typename Walk>
		void walk_composite_model(Walk walk, XCompositeModelDef& asset)
		{
			if (const auto submodels = walk.array(asset.submodels, 10, 7))
			{
				for (auto i = 0; i < 10; i++)
				{
					walk.string(submodels[i].modelName);
					walk.script_string(submodels[i].parentBoneName);
					walk.asset(ASSET_TYPE_XMODEL, submodels[i].model);
				}
			}

			if (const auto composites = walk.array(asset.subCompositeModels, 10, 7))
			{
				for (auto i = 0; i < 10; i++)
				{
					walk.string(composites[i].compositeModelName);
					walk.script_string(composites[i].parentBoneName);
					walk.asset(ASSET_TYPE_XCOMPOSITEMODEL, composites[i].compositeModel);
				}
			}
		}
	}

	void scriptable_def::read(zone_reader& reader, ScriptableDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_scriptable(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void scriptable_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_scriptable(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void scriptable_def::dump(ScriptableDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("scriptable\\"s + asset->name + ".scriptableb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_scriptable(dump_walk{dump}, *asset);
		dump.close();
	}

	ScriptableDef* scriptable_def::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("scriptable\\"s + name + ".scriptableb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing scriptable \"%s\"...", name.data());

		auto* asset = read.read_single<ScriptableDef>();
		asset->name = read.read_string();
		walk_scriptable(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void scriptable_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<ScriptableDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void scriptable_def::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_scriptable(depend_walk{zone}, *this->asset_);
	}

	void scriptable_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_scriptable(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string scriptable_def::name()
	{
		return this->name_;
	}

	std::int32_t scriptable_def::type()
	{
		return ASSET_TYPE_SCRIPTABLE;
	}

	void composite_model::read(zone_reader& reader, XCompositeModelDef* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		walk_composite_model(read_walk{reader}, *asset);
		reader.pop_stream();
	}

	void composite_model::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		walk_composite_model(write_walk{zone, buf}, *dest);
		buf->pop_stream();
	}

	void composite_model::dump(XCompositeModelDef* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("xcompositemodel\\"s + asset->name + ".xcompositemodelb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		walk_composite_model(dump_walk{dump}, *asset);
		dump.close();
	}

	XCompositeModelDef* composite_model::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("xcompositemodel\\"s + name + ".xcompositemodelb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xcompositemodel \"%s\"...", name.data());

		auto* asset = read.read_single<XCompositeModelDef>();
		asset->name = read.read_string();
		walk_composite_model(parse_script_walk{{read}, this->script_strings_}, *asset);
		read.close();
		return asset;
	}

	void composite_model::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XCompositeModelDef>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void composite_model::load_depending(zone_base* zone)
	{
		if (this->referenced())
		{
			return;
		}

		walk_composite_model(depend_walk{zone}, *this->asset_);
	}

	void composite_model::prepare(zone_buffer* buf, zone_memory* mem)
	{
		if (!this->referenced())
		{
			walk_composite_model(prepare_walk{buf, this->script_strings_}, *this->asset_);
		}
	}

	std::string composite_model::name()
	{
		return this->name_;
	}

	std::int32_t composite_model::type()
	{
		return ASSET_TYPE_XCOMPOSITEMODEL;
	}
}
