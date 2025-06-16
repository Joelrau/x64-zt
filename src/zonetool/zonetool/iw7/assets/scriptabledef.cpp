#include <std_include.hpp>
#include "scriptabledef.hpp"

namespace zonetool::iw7
{
	void scriptable_def::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* scriptable_def::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	void scriptable_def::parse_scriptable_event(assetmanager::reader& read, ScriptableEventDef* data)
	{
		auto read_event_base = [&]()
		{
			data->data.anonymous.base = read.read_single<ScriptableEventBaseDef>();
			if (data->data.anonymous.base)
			{
				data->data.anonymous.base->name = read.read_string();
			}
		};

		auto read_part_reference = [&](ScriptablePartReference* pr)
		{
			pr->base.name = read.read_string();

			auto read_part_reference_base = [&]()
			{
				pr->u.__0.base = read.read_single<unk_1453E1B90>();
				if (pr->u.__0.base)
				{
					pr->u.__0.base->name = read.read_string();
				}
			};

			switch (pr->type)
			{
			case 0:
			case 1:
			case 2:
				read_part_reference_base();
				break;
			case 3:
				read_part_reference_base();
				pr->u.__3.val = read.read_array<unsigned int>();
				break;
			}
		};

		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			read_event_base();
			read_part_reference(&data->data.stateChange.partReference);
			break;
		case Scriptable_EventType_Wait:
			read_event_base();
			break;
		case Scriptable_EventType_Random:
			read_event_base();
			data->data.random.eventsA = read.read_array<ScriptableEventDef>();
			for (unsigned i = 0; i < data->data.random.eventACount; i++)
			{
				parse_scriptable_event(read, &data->data.random.eventsA[i]);
			}
			data->data.random.eventsB = read.read_array<ScriptableEventDef>();
			for (unsigned i = 0; i < data->data.random.eventBCount; i++)
			{
				parse_scriptable_event(read, &data->data.random.eventsB[i]);
			}
			break;
		case Scriptable_EventType_Script:
			read_event_base();
			data->data.script.notification = read.read_string();
			this->add_script_string(&data->data.script.scrNotification, read.read_string());
			break;
		case Scriptable_EventType_Model:
			read_event_base();
			data->data.model.model = read.read_asset<XModel>();
			break;
		case Scriptable_EventType_Collision:
			read_event_base();
			data->data.collision.collmapName = read.read_string();
			break;
		case Scriptable_EventType_Animation:
			read_event_base();
			data->data.animation.animation = read.read_asset<XAnimParts>();
			data->data.animation.eventsAtEnd = read.read_array<ScriptableEventDef>();
			for (unsigned i = 0; i < data->data.animation.eventAtEndCount; i++)
			{
				parse_scriptable_event(read, &data->data.animation.eventsAtEnd[i]);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			read_event_base();
			data->data.hideShowBone.tagName = read.read_string();
			this->add_script_string(&data->data.hideShowBone.scrTagName, read.read_string());
			break;
		case Scriptable_EventType_NoteTrack:
			read_event_base();
			data->data.noteTrack.noteTracks = read.read_array<ScriptableNoteTrackDef>();
			for (unsigned i = 0; i < data->data.noteTrack.noteTrackCount; i++)
			{
				data->data.noteTrack.noteTracks[i].noteTrackName = read.read_string();
				this->add_script_string(&data->data.noteTrack.noteTracks[i].scrNoteTrackName, read.read_string());
				data->data.noteTrack.noteTracks[i].events = read.read_array<ScriptableEventDef>();
				for (unsigned j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
				{
					parse_scriptable_event(read, &data->data.noteTrack.noteTracks[i].events[j]);
				}
			}
			break;
		case Scriptable_EventType_PFX:
			read_event_base();
			data->data.particleFX.scrTagNames = read.read_array<scr_string_t>();
			for (unsigned i = 0; i < data->data.particleFX.scrTagCount; i++)
			{
				this->add_script_string(&data->data.particleFX.scrTagNames[i], read.read_string());
			}
			data->data.particleFX.scrEndTagNames = read.read_array<scr_string_t>();
			for (unsigned i = 0; i < data->data.particleFX.scrEndTagCount; i++)
			{
				this->add_script_string(&data->data.particleFX.scrEndTagNames[i], read.read_string());
			}
			data->data.particleFX.effectAlias = read.read_string();
			data->data.particleFX.effectDef.u.vfx = read.read_asset<ParticleSystemDef>();
			break;
		case Scriptable_EventType_Sound:
			read_event_base();
			data->data.sound.tagName = read.read_string();
			this->add_script_string(&data->data.sound.scrTagName, read.read_string());
			data->data.sound.soundAlias = read.read_string();
			data->data.sound.soundAliasCache = read.read_string();
			break;
		case Scriptable_EventType_Explosion:
			read_event_base();
			data->data.explosion.tagName = read.read_string();
			this->add_script_string(&data->data.explosion.scrTagName, read.read_string());
			break;
		case Scriptable_EventType_Light:
			read_event_base();
			data->data.light.name = read.read_string();
			this->add_script_string(&data->data.light.scrName, read.read_string());
			break;
		case Scriptable_EventType_Sun:
			read_event_base();
			break;
		case Scriptable_EventType_Rumble:
			read_event_base();
			data->data.rumble.tagName = read.read_string();
			this->add_script_string(&data->data.rumble.scrTagName, read.read_string());
			data->data.rumble.rumble = read.read_string();
			data->data.rumble.rumbleAsset = read.read_asset<RumbleInfo>();
			break;
		case Scriptable_EventType_Screenshake:
			read_event_base();
			data->data.screenshake.tagName = read.read_string();
			this->add_script_string(&data->data.screenshake.scrTagName, read.read_string());
			break;
		case Scriptable_EventType_PartDamage:
			read_event_base();
			read_part_reference(&data->data.partDamage.partReference);
			break;
		case Scriptable_EventType_SetMayhem:
			read_event_base();
			data->data.setMayhem.mayhem = read.read_asset<MayhemData>();
			break;
		case Scriptable_EventType_PlayMayhem:
			read_event_base();
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			read_event_base();
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			read_event_base();
			break;
		case Scriptable_EventType_ClientViewSelector:
			read_event_base();
			data->data.clientViewSelector.events1p = read.read_array<ScriptableEventDef>();
			for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
			{
				parse_scriptable_event(read, &data->data.clientViewSelector.events1p[i]);
			}
			data->data.clientViewSelector.events3p = read.read_array<ScriptableEventDef>();
			for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
			{
				parse_scriptable_event(read, &data->data.clientViewSelector.events3p[i]);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			read_event_base();
			data->data.teamSelector.eventsPass = read.read_array<ScriptableEventDef>();
			for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
			{
				parse_scriptable_event(read, &data->data.teamSelector.eventsPass[i]);
			}
			data->data.teamSelector.eventsFail = read.read_array<ScriptableEventDef>();
			for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
			{
				parse_scriptable_event(read, &data->data.teamSelector.eventsFail[i]);
			}
			break;
		case Scriptable_EventType_AddModel:
			read_event_base();
			data->data.addModel.tagName = read.read_string();
			this->add_script_string(&data->data.addModel.scrTagName, read.read_string());
			data->data.addModel.model = read.read_asset<XModel>();
			break;
		case Scriptable_EventType_ApplyForce:
			read_event_base();
			break;
		case Scriptable_EventType_CompassIcon:
			read_event_base();
			data->data.compassIcon.friendlyArrow = read.read_asset<Material>();
			data->data.compassIcon.friendlyFiring = read.read_asset<Material>();
			data->data.compassIcon.friendlyChatting = read.read_asset<Material>();
			data->data.compassIcon.friendlyYelling = read.read_asset<Material>();
			data->data.compassIcon.partyArrow = read.read_asset<Material>();
			data->data.compassIcon.partyFiring = read.read_asset<Material>();
			data->data.compassIcon.partyChatting = read.read_asset<Material>();
			data->data.compassIcon.partyYelling = read.read_asset<Material>();
			data->data.compassIcon.squadArrow = read.read_asset<Material>();
			data->data.compassIcon.squadFiring = read.read_asset<Material>();
			for (auto i = 0; i < 3; i++)
			{
				data->data.compassIcon.enemyCompassIconQuiet[i] = read.read_asset<Material>();
			}
			for (auto i = 0; i < 3; i++)
			{
				data->data.compassIcon.enemyCompassIconFiring[i] = read.read_asset<Material>();
			}
			data->data.compassIcon.enemyCompassIconDirectional = read.read_asset<Material>();
			break;
		case Scriptable_EventType_MaterialOverride:
			read_event_base();
			data->data.materialOverride.material = read.read_asset<Material>();
			break;
		}
	}

	void scriptable_def::parse_state_base(assetmanager::reader& read, ScriptableStateBaseDef* data)
	{
		data->name = read.read_string();
		data->events = read.read_array<ScriptableEventDef>();
		for (unsigned i = 0; i < data->numEvents; i++)
		{
			parse_scriptable_event(read, &data->events[i]);
		}
	}

	void scriptable_def::parse_state(assetmanager::reader& read, ScriptableStateDef* data)
	{
		parse_state_base(read, &data->base);

		switch (data->type) {
		case Scriptable_StateType_Simple:
			data->data.simple.base = read.read_single<ScriptableStateBaseDef>();
			if (data->data.simple.base)
			{
				parse_state_base(read, data->data.simple.base);
			}
			break;

		case Scriptable_StateType_Health:
			data->data.health.base = read.read_single<ScriptableStateBaseDef>();
			if (data->data.health.base)
			{
				parse_state_base(read, data->data.health.base);
			}
			data->data.health.script_id = read.read_string();
			this->add_script_string(&data->data.health.scrScript_id, read.read_string());
			break;

		case Scriptable_StateType_Scripted:
			data->data.scripted.base = read.read_single<ScriptableStateBaseDef>();
			if (data->data.scripted.base)
			{
				parse_state_base(read, data->data.scripted.base);
			}
			data->data.scripted.script_id = read.read_string();
			this->add_script_string(&data->data.scripted.scrScript_id, read.read_string());
			break;

		default:
			break;
		}
	}

	void scriptable_def::parse_part(assetmanager::reader& read, ScriptablePartDef* data)
	{
		data->name = read.read_string();
		this->add_script_string(&data->scrName, read.read_string());
		data->tagName = read.read_string();
		this->add_script_string(&data->scrTagName, read.read_string());

		data->states = read.read_array<ScriptableStateDef>();
		for (unsigned i = 0; i < data->numStates; i++)
		{
			parse_state(read, &data->states[i]);
		}

		data->childParts = read.read_array<ScriptablePartDef>();
		for (unsigned i = 0; i < data->numChildParts; i++)
		{
			parse_part(read, &data->childParts[i]);
		}

		data->damageTagOverrides = read.read_array<ScriptableDamageTagOverride>();
		for (unsigned i = 0; i < data->numDamageTagOverrides; i++)
		{
			data->damageTagOverrides[i].tag = read.read_string();
			this->add_script_string(&data->damageTagOverrides[i].scrTag, read.read_string());
		}
	}

	ScriptableDef* scriptable_def::parse(std::string name, zone_memory* mem)
	{
		const auto path = "scriptable\\"s + name;

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing scriptable \"%s\"...", name.data());

		ScriptableDef* asset = read.read_single<ScriptableDef>();
		asset->name = read.read_string();

		asset->nextScriptableDef = read.read_asset<ScriptableDef>();

		asset->parts = read.read_array<ScriptablePartDef>();
		for (unsigned int i = 0; i < asset->numParts; i++)
		{
			parse_part(read, &asset->parts[i]);
		}

		this->add_script_string(&asset->animationTreeName, read.read_string());

		asset->models = read.read_array<XModel*>();
		for (unsigned int i = 0; i < asset->numXModels; i++)
		{
			asset->models[i] = read.read_asset<XModel>();
		}

		read.close();
		return asset;
	}

	void scriptable_def::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			ZONETOOL_FATAL("Could not parse scriptable \"%s\"", name.data());
		}
	}

	void scriptable_def::prepare_scriptable_event(zone_buffer* buf, ScriptableEventDef* data)
	{
		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			break;
		case Scriptable_EventType_Wait:
			break;
		case Scriptable_EventType_Random:
			for (unsigned i = 0; i < data->data.random.eventACount; i++)
			{
				prepare_scriptable_event(buf, &data->data.random.eventsA[i]);
			}
			for (unsigned i = 0; i < data->data.random.eventBCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.random.eventsB[i]);
			}
			break;
		case Scriptable_EventType_Script:
			data->data.script.scrNotification = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.script.scrNotification)));
			break;
		case Scriptable_EventType_Model:
			break;
		case Scriptable_EventType_Collision:
			break;
		case Scriptable_EventType_Animation:
			for (unsigned i = 0; i < data->data.animation.eventAtEndCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.animation.eventsAtEnd[i]);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			data->data.hideShowBone.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.hideShowBone.scrTagName)));
			break;
		case Scriptable_EventType_NoteTrack:
			for (unsigned i = 0; i < data->data.noteTrack.noteTrackCount; i++)
			{
				data->data.noteTrack.noteTracks[i].scrNoteTrackName = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->data.noteTrack.noteTracks[i].scrNoteTrackName)));
				for (unsigned j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
				{
					prepare_scriptable_event(buf, &data->data.noteTrack.noteTracks[i].events[j]);
				}
			}
			break;
		case Scriptable_EventType_PFX:
			for (unsigned i = 0; i < data->data.particleFX.scrTagCount; i++)
			{
				data->data.particleFX.scrTagNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->data.particleFX.scrTagNames[i])));
			}
			for (unsigned i = 0; i < data->data.particleFX.scrEndTagCount; i++)
			{
				data->data.particleFX.scrEndTagNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&data->data.particleFX.scrEndTagNames[i])));
			}
			break;
		case Scriptable_EventType_Sound:
			data->data.sound.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.sound.scrTagName)));
			break;
		case Scriptable_EventType_Explosion:
			data->data.explosion.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.explosion.scrTagName)));
			break;
		case Scriptable_EventType_Light:
			data->data.light.scrName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.light.scrName)));
			break;
		case Scriptable_EventType_Sun:
			break;
		case Scriptable_EventType_Rumble:
			data->data.rumble.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.rumble.scrTagName)));
			break;
		case Scriptable_EventType_Screenshake:
			data->data.screenshake.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.screenshake.scrTagName)));
			break;
		case Scriptable_EventType_PartDamage:
			break;
		case Scriptable_EventType_SetMayhem:
			break;
		case Scriptable_EventType_PlayMayhem:
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			break;
		case Scriptable_EventType_ClientViewSelector:
			for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.clientViewSelector.events1p[i]);
			}
			for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.clientViewSelector.events3p[i]);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.teamSelector.eventsPass[i]);
			}
			for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
			{
				prepare_scriptable_event(buf, &data->data.teamSelector.eventsFail[i]);
			}
			break;
		case Scriptable_EventType_AddModel:
			data->data.addModel.scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.addModel.scrTagName)));
			break;
		case Scriptable_EventType_ApplyForce:
			break;
		case Scriptable_EventType_CompassIcon:
			break;
		case Scriptable_EventType_MaterialOverride:
			break;
		}
	}

	void scriptable_def::prepare_state_base(zone_buffer* buf, ScriptableStateBaseDef* data)
	{
		for (unsigned i = 0; i < data->numEvents; i++)
		{
			prepare_scriptable_event(buf, &data->events[i]);
		}
	}

	void scriptable_def::prepare_state(zone_buffer* buf, ScriptableStateDef* data)
	{
		prepare_state_base(buf, &data->base);

		switch (data->type) {
		case Scriptable_StateType_Simple:
			if (data->data.simple.base)
			{
				prepare_state_base(buf, data->data.simple.base);
			}
			break;

		case Scriptable_StateType_Health:
			if (data->data.health.base)
			{
				prepare_state_base(buf, data->data.health.base);
			}
			data->data.health.scrScript_id = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.health.scrScript_id)));
			break;

		case Scriptable_StateType_Scripted:
			if (data->data.scripted.base)
			{
				prepare_state_base(buf, data->data.scripted.base);
			}
			data->data.scripted.scrScript_id = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->data.scripted.scrScript_id)));
			break;

		default:
			break;
		}
	}

	void scriptable_def::prepare_part(zone_buffer* buf, ScriptablePartDef* data)
	{
		data->scrName = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&data->scrName)));

		data->scrTagName = static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&data->scrTagName)));

		for (unsigned i = 0; i < data->numStates; i++)
		{
			prepare_state(buf, &data->states[i]);
		}

		for (unsigned i = 0; i < data->numChildParts; i++)
		{
			prepare_part(buf, &data->childParts[i]);
		}

		for (unsigned i = 0; i < data->numDamageTagOverrides; i++)
		{
			data->damageTagOverrides[i].scrTag = static_cast<scr_string_t>(buf->write_scriptstring(
				this->get_script_string(&data->damageTagOverrides[i].scrTag)));
		}
	}

	void scriptable_def::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (unsigned int i = 0; i < data->numParts; i++)
		{
			prepare_part(buf, &data->parts[i]);
		}

		data->animationTreeName	= static_cast<scr_string_t>(buf->write_scriptstring(
			this->get_script_string(&data->animationTreeName)));
	}

	void scriptable_def::load_depending_scriptable_event(zone_base* zone, ScriptableEventDef* data)
	{
		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			break;
		case Scriptable_EventType_Wait:
			break;
		case Scriptable_EventType_Random:
			for (unsigned i = 0; i < data->data.random.eventACount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.random.eventsA[i]);
			}
			for (unsigned i = 0; i < data->data.random.eventBCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.random.eventsB[i]);
			}
			break;
		case Scriptable_EventType_Script:
			break;
		case Scriptable_EventType_Model:
			if (data->data.model.model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->data.model.model->name);
			}
			break;
		case Scriptable_EventType_Collision:
			if (data->data.collision.collmapName)
			{
				zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->data.collision.collmapName);
			}
			break;
		case Scriptable_EventType_Animation:
			if (data->data.animation.animation)
			{
				zone->add_asset_of_type(ASSET_TYPE_XANIMPARTS, data->data.animation.animation->name);
			}
			for (unsigned i = 0; i < data->data.animation.eventAtEndCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.animation.eventsAtEnd[i]);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			break;
		case Scriptable_EventType_NoteTrack:
			for (unsigned i = 0; i < data->data.noteTrack.noteTrackCount; i++)
			{
				for (unsigned j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
				{
					load_depending_scriptable_event(zone, &data->data.noteTrack.noteTracks[i].events[j]);
				}
			}
			break;
		case Scriptable_EventType_PFX:
			if (data->data.particleFX.effectDef.u.vfx)
			{
				zone->add_asset_of_type(ASSET_TYPE_VFX, data->data.particleFX.effectDef.u.vfx->name);
			}
			break;
		case Scriptable_EventType_Sound:
			// data->data.sound.soundAlias
			break;
		case Scriptable_EventType_Explosion:
			break;
		case Scriptable_EventType_Light:
			break;
		case Scriptable_EventType_Sun:
			break;
		case Scriptable_EventType_Rumble:
			if (data->data.rumble.rumbleAsset)
			{
				zone->add_asset_of_type(ASSET_TYPE_RUMBLE, data->data.rumble.rumbleAsset->name);
			}
			break;
		case Scriptable_EventType_Screenshake:
			break;
		case Scriptable_EventType_PartDamage:
			break;
		case Scriptable_EventType_SetMayhem:
			if (data->data.setMayhem.mayhem)
			{
				zone->add_asset_of_type(ASSET_TYPE_MAYHEM, data->data.setMayhem.mayhem->name);
			}
			break;
		case Scriptable_EventType_PlayMayhem:
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			break;
		case Scriptable_EventType_ClientViewSelector:
			for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.clientViewSelector.events1p[i]);
			}
			for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.clientViewSelector.events3p[i]);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.teamSelector.eventsPass[i]);
			}
			for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
			{
				load_depending_scriptable_event(zone, &data->data.teamSelector.eventsFail[i]);
			}
			break;
		case Scriptable_EventType_AddModel:
			if (data->data.addModel.model)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->data.addModel.model->name);
			}
			break;
		case Scriptable_EventType_ApplyForce:
			break;
		case Scriptable_EventType_CompassIcon:
			if (data->data.compassIcon.friendlyArrow)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.friendlyArrow->name);
			}
			if (data->data.compassIcon.friendlyFiring)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.friendlyFiring->name);
			}
			if (data->data.compassIcon.friendlyChatting)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.friendlyChatting->name);
			}
			if (data->data.compassIcon.friendlyYelling)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.friendlyYelling->name);
			}
			if (data->data.compassIcon.partyArrow)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.partyArrow->name);
			}
			if (data->data.compassIcon.partyFiring)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.partyFiring->name);
			}
			if (data->data.compassIcon.partyChatting)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.partyChatting->name);
			}
			if (data->data.compassIcon.partyYelling)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.partyYelling->name);
			}
			if (data->data.compassIcon.squadArrow)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.squadArrow->name);
			}
			if (data->data.compassIcon.squadFiring)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.squadFiring->name);
			}
			for (auto i = 0; i < 3; i++)
			{
				if (data->data.compassIcon.enemyCompassIconQuiet[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.enemyCompassIconQuiet[i]->name);
				}
			}
			for (auto i = 0; i < 3; i++)
			{
				if (data->data.compassIcon.enemyCompassIconFiring[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.enemyCompassIconFiring[i]->name);
				}
			}
			if (data->data.compassIcon.enemyCompassIconDirectional)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.compassIcon.enemyCompassIconDirectional->name);
			}
			break;
		case Scriptable_EventType_MaterialOverride:
			if (data->data.materialOverride.material)
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->data.materialOverride.material->name);
			}
			break;
		}
	}

	void scriptable_def::load_depending_state_base(zone_base* zone, ScriptableStateBaseDef* data)
	{
		for (unsigned int i = 0; i < data->numEvents; i++)
		{
			load_depending_scriptable_event(zone, &data->events[i]);
		}
	}

	void scriptable_def::load_depending_state(zone_base* zone, ScriptableStateDef* data)
	{
		if (data->type == Scriptable_StateType_Simple)
		{
			if (data->data.simple.base)
			{
				load_depending_state_base(zone, data->data.simple.base);
			}
		}
		else if (data->type == Scriptable_StateType_Health)
		{
			if (data->data.health.base)
			{
				load_depending_state_base(zone, data->data.scripted.base);
			}
		}
		else if (data->type == Scriptable_StateType_Scripted)
		{
			if (data->data.scripted.base)
			{
				load_depending_state_base(zone, data->data.scripted.base);
			}
		}
	}

	void scriptable_def::load_depending_part(zone_base* zone, ScriptablePartDef* data)
	{
		for (unsigned int i = 0; i < data->numStates; i++)
		{
			load_depending_state(zone, &data->states[i]);
		}

		for (unsigned int i = 0; i < data->numChildParts; i++)
		{
			load_depending_part(zone, &data->childParts[i]);
		}
	}

	void scriptable_def::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if(data->nextScriptableDef)
		{
			zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->nextScriptableDef->name);
		}

		for (unsigned int i = 0; i < data->numParts; i++)
		{
			load_depending_part(zone, &data->parts[i]);
		}

		for (unsigned int i = 0; i < data->numXModels; i++)
		{
			if (data->models[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->models[i]->name);
			}
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

	void write_scriptable_event(zone_base* zone, zone_buffer* buf, ScriptableEventDef* data, ScriptableEventDef* dest)
	{
		if (data->base.name)
		{
			dest->base.name = buf->write_str(data->base.name);
		}

		const auto write_event_base = [&]()
		{
			if (data->data.anonymous.base)
			{
				buf->align(7);
				dest->data.anonymous.base = buf->write(data->data.anonymous.base);
				if (data->data.anonymous.base->name)
				{
					dest->data.anonymous.base->name = buf->write_str(data->data.anonymous.base->name);
				}
				buf->clear_pointer(&dest->data.anonymous.base);
			}
		};

		const auto write_part_reference = [&](ScriptablePartReference* data_, ScriptablePartReference* dest_)
		{
			if (data_->base.name)
			{
				dest_->base.name = buf->write_str(data_->base.name);
			}

			const auto write_part_reference_base = [&]()
			{
				if (data_->u.__0.base)
				{
					buf->align(7);
					dest_->u.__0.base = buf->write(data_->u.__0.base);
					if (data_->u.__0.base)
					{
						dest_->u.__0.base->name = buf->write_str(data_->u.__0.base->name);
					}
					buf->clear_pointer(&dest_->u.__0.base);
				}
			};

			switch (data_->type)
			{
			case 0:
			case 1:
			case 2:
				write_part_reference_base();
				break;
			case 3:
				write_part_reference_base();
				if (data_->u.__3.val)
				{
					buf->align(3);
					buf->write(data_->u.__3.val, data_->u.__3.count);
					buf->clear_pointer(&dest_->u.__3.val);
				}
			}
		};

		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			write_event_base();
			write_part_reference(&data->data.stateChange.partReference, &dest->data.stateChange.partReference);
			break;
		case Scriptable_EventType_Wait:
			write_event_base();
			break;
		case Scriptable_EventType_Random:
			write_event_base();
			if (data->data.random.eventsA)
			{
				buf->align(7);
				buf->write(data->data.random.eventsA, data->data.random.eventACount);
				buf->clear_pointer(&dest->data.random.eventsA);
			}
			if (data->data.random.eventsB)
			{
				buf->align(7);
				buf->write(data->data.random.eventsB, data->data.random.eventBCount);
				buf->clear_pointer(&dest->data.random.eventsB);
			}
			break;
		case Scriptable_EventType_Script:
			write_event_base();
			if (data->data.script.notification)
			{
				buf->write_str(data->data.script.notification);
			}
			break;
		case Scriptable_EventType_Model:
			write_event_base();
			if (data->data.model.model)
			{
				dest->data.model.model = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.model.model->name));
			}
			break;
		case Scriptable_EventType_Collision:
			write_event_base();
			if (data->data.collision.collmapName)
			{
				dest->data.collision.collmapName = buf->write_str(data->data.collision.collmapName);
			}
			break;
		case Scriptable_EventType_Animation:
			write_event_base();
			if (data->data.animation.animation)
			{
				dest->data.animation.animation = reinterpret_cast<XAnimParts*>(
					zone->get_asset_pointer(ASSET_TYPE_XANIMPARTS, data->data.animation.animation->name));
			}
			if (data->data.animation.eventsAtEnd)
			{
				buf->align(7);
				dest->data.animation.eventsAtEnd = buf->write(data->data.animation.eventsAtEnd, data->data.animation.eventAtEndCount);
				for (unsigned int i = 0; i < data->data.animation.eventAtEndCount; i++)
				{
					write_scriptable_event(zone, buf, &data->data.animation.eventsAtEnd[i], &dest->data.animation.eventsAtEnd[i]);
				}
				buf->clear_pointer(&dest->data.animation.eventsAtEnd);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			write_event_base();
			if (data->data.hideShowBone.tagName)
			{
				dest->data.hideShowBone.tagName = buf->write_str(data->data.hideShowBone.tagName);
			}
			break;
		case Scriptable_EventType_NoteTrack:
			write_event_base();
			if (data->data.noteTrack.noteTracks)
			{
				buf->align(7);

				dest->data.noteTrack.noteTracks = buf->write(data->data.noteTrack.noteTracks, data->data.noteTrack.noteTrackCount);
				for (unsigned int i = 0; i < data->data.noteTrack.noteTrackCount; i++)
				{
					if (data->data.noteTrack.noteTracks[i].noteTrackName)
					{
						dest->data.noteTrack.noteTracks[i].noteTrackName = buf->write_str(data->data.noteTrack.noteTracks[i].noteTrackName);
					}
					if (data->data.noteTrack.noteTracks[i].events)
					{
						buf->align(7);
						dest->data.noteTrack.noteTracks[i].events = buf->write(data->data.noteTrack.noteTracks[i].events, data->data.noteTrack.noteTracks[i].numEvents);
						for (unsigned int j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
						{
							write_scriptable_event(zone, buf, &data->data.noteTrack.noteTracks[i].events[j], &dest->data.noteTrack.noteTracks[i].events[j]);
						}
						buf->clear_pointer(&dest->data.noteTrack.noteTracks[i].events);
					}
				}

				buf->clear_pointer(&dest->data.noteTrack.noteTracks);
			}
			break;
		case Scriptable_EventType_ChunkDynent:
			write_event_base();
			write_part_reference(&data->data.chunkDynent.partReference, &dest->data.chunkDynent.partReference);
			break;
		case Scriptable_EventType_SpawnDynent:
			write_event_base();
			if (data->data.spawnDynent.model)
			{
				dest->data.spawnDynent.model = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.spawnDynent.model->name));
			}
			if (data->data.spawnDynent.tagName)
			{
				dest->data.spawnDynent.tagName = buf->write_str(data->data.spawnDynent.tagName);
			}
			break;
		case Scriptable_EventType_PFX:
			write_event_base();
			if (data->data.particleFX.scrTagNames)
			{
				buf->align(3);
				buf->write(data->data.particleFX.scrTagNames, data->data.particleFX.scrTagCount);
				buf->clear_pointer(&dest->data.particleFX.scrTagNames);
			}
			if (data->data.particleFX.scrEndTagNames)
			{
				buf->align(3);
				buf->write(data->data.particleFX.scrEndTagNames, data->data.particleFX.scrEndTagCount);
				buf->clear_pointer(&dest->data.particleFX.scrEndTagNames);
			}
			if (data->data.particleFX.effectAlias)
			{
				dest->data.particleFX.effectAlias = buf->write_str(data->data.particleFX.effectAlias);
			}
			if (data->data.particleFX.effectDef.u.vfx)
			{
				dest->data.particleFX.effectDef.u.vfx = reinterpret_cast<ParticleSystemDef*>(
					zone->get_asset_pointer(ASSET_TYPE_VFX, data->data.particleFX.effectDef.u.vfx->name));
			}
			break;
		case Scriptable_EventType_Sound:
			write_event_base();
			if (data->data.sound.tagName)
			{
				dest->data.sound.tagName = buf->write_str(data->data.sound.tagName);
			}
			if (data->data.sound.soundAlias)
			{
				dest->data.sound.soundAlias = buf->write_str(data->data.sound.soundAlias);
			}
			if (data->data.sound.soundAliasCache)
			{
				dest->data.sound.soundAliasCache = buf->write_str(data->data.sound.soundAliasCache);
			}
			break;
		case Scriptable_EventType_Explosion:
			write_event_base();
			if (data->data.explosion.tagName)
			{
				dest->data.explosion.tagName = buf->write_str(data->data.explosion.tagName);
			}
			break;
		case Scriptable_EventType_Light:
			write_event_base();
			if (data->data.light.name)
			{
				dest->data.light.name = buf->write_str(data->data.light.name);
			}
			break;
		case Scriptable_EventType_Sun:
			write_event_base();
			break;
		case Scriptable_EventType_Rumble:
			write_event_base();
			if (data->data.rumble.tagName)
			{
				dest->data.rumble.tagName = buf->write_str(data->data.rumble.tagName);
			}
			if (data->data.rumble.rumble)
			{
				dest->data.rumble.rumble = buf->write_str(data->data.rumble.rumble);
			}
			if (data->data.rumble.rumbleAsset)
			{
				dest->data.rumble.rumbleAsset = reinterpret_cast<RumbleInfo*>(
					zone->get_asset_pointer(ASSET_TYPE_RUMBLE, data->data.rumble.rumbleAsset->name));
			}
			break;
		case Scriptable_EventType_Screenshake:
			write_event_base();
			if (data->data.screenshake.tagName)
			{
				dest->data.screenshake.tagName = buf->write_str(data->data.screenshake.tagName);
			}
			break;
		case Scriptable_EventType_PartDamage:
			write_event_base();
			write_part_reference(&data->data.partDamage.partReference, &dest->data.partDamage.partReference);
			break;
		case Scriptable_EventType_SetMayhem:
			write_event_base();
			if (data->data.setMayhem.mayhem)
			{
				dest->data.setMayhem.mayhem = reinterpret_cast<MayhemData*>(
					zone->get_asset_pointer(ASSET_TYPE_MAYHEM, data->data.setMayhem.mayhem->name));
			}
			break;
		case Scriptable_EventType_PlayMayhem:
			write_event_base();
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			write_event_base();
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			write_event_base();
			break;
		case Scriptable_EventType_ClientViewSelector:
			write_event_base();
			if (data->data.clientViewSelector.events1p)
			{
				buf->align(7);
				auto* destEvents1p = buf->write(data->data.clientViewSelector.events1p, data->data.clientViewSelector.event1pCount);
				auto* dataEvents1p = data->data.clientViewSelector.events1p;
				for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
				{
					write_scriptable_event(zone, buf, &dataEvents1p[i], &destEvents1p[i]);
				}
				buf->clear_pointer(&dest->data.clientViewSelector.events1p);
			}
			if (data->data.clientViewSelector.events3p)
			{
				buf->align(7);
				auto* destEvents3p = buf->write(data->data.clientViewSelector.events3p, data->data.clientViewSelector.event3pCount);
				auto* dataEvents3p = data->data.clientViewSelector.events3p;
				for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
				{
					write_scriptable_event(zone, buf, &dataEvents3p[i], &destEvents3p[i]);
				}
				buf->clear_pointer(&dest->data.clientViewSelector.events3p);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			write_event_base();
			if (data->data.teamSelector.eventsPass)
			{
				buf->align(7);
				auto* destEvents = buf->write(data->data.teamSelector.eventsPass, data->data.teamSelector.eventPassCount);
				auto* dataEvents = data->data.teamSelector.eventsPass;
				for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
				{
					write_scriptable_event(zone, buf, &dataEvents[i], &destEvents[i]);
				}
				buf->clear_pointer(&dest->data.teamSelector.eventsPass);
			}
			if (data->data.teamSelector.eventsFail)
			{
				buf->align(7);
				auto* destEvents = buf->write(data->data.teamSelector.eventsFail, data->data.teamSelector.eventFailCount);
				auto* dataEvents = data->data.teamSelector.eventsFail;
				for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
				{
					write_scriptable_event(zone, buf, &dataEvents[i], &destEvents[i]);
				}
				buf->clear_pointer(&dest->data.teamSelector.eventsFail);
			}
			break;
		case Scriptable_EventType_AddModel:
			write_event_base();
			if (data->data.addModel.tagName)
			{
				dest->data.addModel.tagName = buf->write_str(data->data.addModel.tagName);
			}
			if (data->data.addModel.model)
			{
				dest->data.addModel.model = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.addModel.model->name));
			}
			break;
		case Scriptable_EventType_ApplyForce:
			write_event_base();
			break;
		case Scriptable_EventType_CompassIcon:
			write_event_base();
			if (data->data.compassIcon.friendlyArrow)
			{
				dest->data.compassIcon.friendlyArrow = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.friendlyArrow->name));
			}
			if (data->data.compassIcon.friendlyFiring)
			{
				dest->data.compassIcon.friendlyFiring = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.friendlyFiring->name));
			}
			if (data->data.compassIcon.friendlyChatting)
			{
				dest->data.compassIcon.friendlyChatting = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.friendlyChatting->name));
			}
			if (data->data.compassIcon.friendlyYelling)
			{
				dest->data.compassIcon.friendlyYelling = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.friendlyYelling->name));
			}
			if (data->data.compassIcon.partyArrow)
			{
				dest->data.compassIcon.partyArrow = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.partyArrow->name));
			}
			if (data->data.compassIcon.partyFiring)
			{
				dest->data.compassIcon.partyFiring = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.partyFiring->name));
			}
			if (data->data.compassIcon.partyChatting)
			{
				dest->data.compassIcon.partyChatting = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.partyChatting->name));
			}
			if (data->data.compassIcon.partyYelling)
			{
				dest->data.compassIcon.partyYelling = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.partyYelling->name));
			}
			if (data->data.compassIcon.squadArrow)
			{
				dest->data.compassIcon.squadArrow = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.squadArrow->name));
			}
			if (data->data.compassIcon.squadFiring)
			{
				dest->data.compassIcon.squadFiring = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.squadFiring->name));
			}
			for (auto i = 0; i < 3; i++)
			{
				if (data->data.compassIcon.enemyCompassIconQuiet[i])
				{
					dest->data.compassIcon.enemyCompassIconQuiet[i] = reinterpret_cast<Material*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.enemyCompassIconQuiet[i]->name));
				}
			}
			for (auto i = 0; i < 3; i++)
			{
				if (data->data.compassIcon.enemyCompassIconFiring[i])
				{
					dest->data.compassIcon.enemyCompassIconFiring[i] = reinterpret_cast<Material*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.enemyCompassIconFiring[i]->name));
				}
			}
			if (data->data.compassIcon.enemyCompassIconDirectional)
			{
				dest->data.compassIcon.enemyCompassIconDirectional = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.compassIcon.enemyCompassIconDirectional->name));
			}
			break;
		case Scriptable_EventType_MaterialOverride:
			write_event_base();
			if (data->data.materialOverride.material)
			{
				dest->data.materialOverride.material = reinterpret_cast<Material*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->data.materialOverride.material->name));
			}
			break;
		}
	}

	void write_state_base(zone_base* zone, zone_buffer* buf, ScriptableStateBaseDef* data, ScriptableStateBaseDef* dest)
	{
		if (data->name)
		{
			dest->name = buf->write_str(data->name);
		}

		if (data->events)
		{
			buf->align(7);

			auto* destEvents = buf->write(data->events, data->numEvents);
			auto* dataEvents = data->events;
			for (unsigned int i = 0; i < data->numEvents; i++)
			{
				auto* destEvent = &destEvents[i];
				auto* dataEvent = &dataEvents[i];
				
				write_scriptable_event(zone, buf, dataEvent, destEvent);
			}

			buf->clear_pointer(&dest->events);
		}
	}

	void write_state(zone_base* zone, zone_buffer* buf, ScriptableStateDef* data, ScriptableStateDef* dest)
	{
		write_state_base(zone, buf, &data->base, &dest->base);

		if (data->type == Scriptable_StateType_Simple)
		{
			if (data->data.simple.base)
			{
				buf->align(7);
				dest->data.simple.base = buf->write(data->data.simple.base);
				write_state_base(zone, buf, data->data.simple.base, dest->data.simple.base);
				buf->clear_pointer(&dest->data.simple.base);
			}
		}
		else if (data->type == Scriptable_StateType_Health)
		{
			if (data->data.health.base)
			{
				buf->align(7);
				dest->data.health.base = buf->write(data->data.health.base);
				write_state_base(zone, buf, data->data.health.base, dest->data.health.base);
				buf->clear_pointer(&dest->data.health.base);
			}

			if (data->data.health.script_id)
			{
				dest->data.health.script_id = buf->write_str(data->data.health.script_id);
			}
		}
		else if (data->type == Scriptable_StateType_Scripted)
		{
			if (data->data.scripted.base)
			{
				buf->align(7);
				dest->data.scripted.base = buf->write(data->data.scripted.base);
				write_state_base(zone, buf, data->data.scripted.base, dest->data.scripted.base);
				buf->clear_pointer(&dest->data.scripted.base);
			}

			if (data->data.scripted.script_id)
			{
				dest->data.scripted.script_id = buf->write_str(data->data.scripted.script_id);
			}
		}
	}

	void write_part(zone_base* zone, zone_buffer* buf, ScriptablePartDef* data, ScriptablePartDef* dest)
	{
		if (data->name)
		{
			dest->name = buf->write_str(data->name);
		}
		if (data->tagName)
		{
			dest->tagName = buf->write_str(data->tagName);
		}
		if (data->states)
		{
			buf->align(7);

			dest->states = buf->write(data->states, data->numStates);
			for (unsigned int i = 0; i < data->numStates; i++)
			{
				write_state(zone, buf, &data->states[i], &dest->states[i]);
			}

			buf->clear_pointer(&dest->states);
		}
		if (data->childParts)
		{
			buf->align(7);

			dest->childParts = buf->write(data->childParts, data->numChildParts);
			for (unsigned int i = 0; i < data->numChildParts; i++)
			{
				write_part(zone, buf, &data->childParts[i], &dest->childParts[i]);
			}
			buf->clear_pointer(&dest->childParts);
		}
		if (data->damageTagOverrides)
		{
			buf->align(7);
			dest->damageTagOverrides = buf->write(data->damageTagOverrides, data->numDamageTagOverrides);
			for (unsigned int i = 0; i < data->numDamageTagOverrides; i++)
			{
				if (data->damageTagOverrides[i].tag)
				{
					dest->damageTagOverrides[i].tag = buf->write_str(data->damageTagOverrides[i].tag);
				}
			}
			buf->clear_pointer(&dest->damageTagOverrides);
		}
	}

	void scriptable_def::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->nextScriptableDef)
		{
			dest->nextScriptableDef = reinterpret_cast<ScriptableDef*>(
				zone->get_asset_pointer(ASSET_TYPE_SCRIPTABLE, data->nextScriptableDef->name));
		}

		if (data->parts)
		{
			buf->align(7);
			dest->parts = buf->write(data->parts, data->numParts);

			for (unsigned int i = 0; i < data->numParts; i++)
			{
				write_part(zone, buf, &data->parts[i], &dest->parts[i]);
			}

			buf->clear_pointer(&dest->parts);
		}

		if (data->models)
		{
			buf->align(7);
			auto* destModels = buf->write(data->models, data->numXModels);
			for (unsigned int i = 0; i < data->numXModels; i++)
			{
				destModels[i] = reinterpret_cast<XModel*>(
					zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->models[i]->name));
			}
			buf->clear_pointer(&dest->models);
		}

		buf->pop_stream();
	}

	void scriptable_def::dump_scriptable_event(assetmanager::dumper& dump, ScriptableEventDef* data)
	{
		const auto dump_event_base = [&]()
		{
			dump.dump_single(data->data.anonymous.base);
			if (data->data.anonymous.base)
			{
				dump.dump_string(data->data.anonymous.base->name);
			}
		};

		const auto dump_part_reference = [&](ScriptablePartReference* data_)
		{
			dump.dump_string(data_->base.name);

			const auto dump_part_reference_base = [&]()
			{
				dump.dump_single(data_->u.__0.base);
				if (data_->u.__0.base)
				{
					dump.dump_string(data_->u.__0.base->name);
				}
			};

			switch (data_->type)
			{
			case 0:
			case 1:
			case 2:
				dump_part_reference_base();
				break;
			case 3:
				dump_part_reference_base();
				dump.dump_array(data_->u.__3.val, data_->u.__3.count);
			}
		};

		switch (data->type)
		{
		case Scriptable_EventType_StateChange:
			dump_event_base();
			dump_part_reference(&data->data.stateChange.partReference);
			break;
		case Scriptable_EventType_Wait:
			dump_event_base();
			break;
		case Scriptable_EventType_Random:
			dump_event_base();
			dump.dump_array(data->data.random.eventsA, data->data.random.eventACount);
			for (unsigned int i = 0; i < data->data.random.eventACount; i++)
			{
				dump_scriptable_event(dump, &data->data.random.eventsA[i]);
			}
			dump.dump_array(data->data.random.eventsB, data->data.random.eventBCount);
			for (unsigned int i = 0; i < data->data.random.eventBCount; i++)
			{
				dump_scriptable_event(dump, &data->data.random.eventsB[i]);
			}
			break;
		case Scriptable_EventType_Script:
			dump_event_base();
			dump.dump_string(data->data.script.notification);
			dump.dump_string(SL_ConvertToString(data->data.script.scrNotification));
			break;
		case Scriptable_EventType_Model:
			dump_event_base();
			dump.dump_asset(data->data.model.model);
			break;
		case Scriptable_EventType_Collision:
			dump_event_base();
			dump.dump_string(data->data.collision.collmapName);
			break;
		case Scriptable_EventType_Animation:
			dump_event_base();
			dump.dump_asset(data->data.animation.animation);
			dump.dump_array(data->data.animation.eventsAtEnd, data->data.animation.eventAtEndCount);
			for (unsigned int i = 0; i < data->data.animation.eventAtEndCount; i++)
			{
				dump_scriptable_event(dump, &data->data.animation.eventsAtEnd[i]);
			}
			break;
		case Scriptable_EventType_HideShowBone:
			dump_event_base();
			dump.dump_string(data->data.hideShowBone.tagName);
			dump.dump_string(SL_ConvertToString(data->data.hideShowBone.scrTagName));
			break;
		case Scriptable_EventType_NoteTrack:
			dump_event_base();
			dump.dump_array(data->data.noteTrack.noteTracks, data->data.noteTrack.noteTrackCount);
			for (unsigned int i = 0; i < data->data.noteTrack.noteTrackCount; i++)
			{
				dump.dump_string(data->data.noteTrack.noteTracks[i].noteTrackName);
				dump.dump_string(SL_ConvertToString(data->data.noteTrack.noteTracks[i].scrNoteTrackName));
				dump.dump_array(data->data.noteTrack.noteTracks[i].events, data->data.noteTrack.noteTracks[i].numEvents);
				for (unsigned int j = 0; j < data->data.noteTrack.noteTracks[i].numEvents; j++)
				{
					dump_scriptable_event(dump, &data->data.noteTrack.noteTracks[i].events[i]);
				}
			}
			break;
		case Scriptable_EventType_ChunkDynent:
			dump_event_base();
			dump_part_reference(&data->data.chunkDynent.partReference);
			break;
		case Scriptable_EventType_SpawnDynent:
			dump_event_base();
			dump.dump_asset(data->data.spawnDynent.model);
			dump.dump_string(data->data.spawnDynent.tagName);
			dump.dump_string(SL_ConvertToString(data->data.spawnDynent.scrTagName));
			break;
		case Scriptable_EventType_PFX:
			dump_event_base();
			dump.dump_array(data->data.particleFX.scrTagNames, data->data.particleFX.scrTagCount);
			for (unsigned int i = 0; i < data->data.particleFX.scrTagCount; i++)
			{
				dump.dump_string(SL_ConvertToString(data->data.particleFX.scrTagNames[i]));
			}
			dump.dump_array(data->data.particleFX.scrEndTagNames, data->data.particleFX.scrEndTagCount);
			for (unsigned int i = 0; i < data->data.particleFX.scrEndTagCount; i++)
			{
				dump.dump_string(SL_ConvertToString(data->data.particleFX.scrEndTagNames[i]));
			}
			dump.dump_string(data->data.particleFX.effectAlias);
			dump.dump_asset(data->data.particleFX.effectDef.u.vfx);
			break;
		case Scriptable_EventType_Sound:
			dump_event_base();
			dump.dump_string(data->data.sound.tagName);
			dump.dump_string(SL_ConvertToString(data->data.sound.scrTagName));
			dump.dump_string(data->data.sound.soundAlias);
			dump.dump_string(data->data.sound.soundAliasCache);
			break;
		case Scriptable_EventType_Explosion:
			dump_event_base();
			dump.dump_string(data->data.explosion.tagName);
			dump.dump_string(SL_ConvertToString(data->data.explosion.scrTagName));
			break;
		case Scriptable_EventType_Light:
			dump_event_base();
			dump.dump_string(data->data.light.name);
			dump.dump_string(SL_ConvertToString(data->data.light.scrName));
			break;
		case Scriptable_EventType_Sun:
			dump_event_base();
			break;
		case Scriptable_EventType_Rumble:
			dump_event_base();
			dump.dump_string(data->data.rumble.tagName);
			dump.dump_string(SL_ConvertToString(data->data.rumble.scrTagName));
			dump.dump_string(data->data.rumble.rumble);
			dump.dump_asset(data->data.rumble.rumbleAsset);
			break;
		case Scriptable_EventType_Screenshake:
			dump_event_base();
			dump.dump_string(data->data.screenshake.tagName);
			dump.dump_string(SL_ConvertToString(data->data.screenshake.scrTagName));
			break;
		case Scriptable_EventType_PartDamage:
			dump_event_base();
			dump_part_reference(&data->data.partDamage.partReference);
			break;
		case Scriptable_EventType_SetMayhem:
			dump_event_base();
			dump.dump_asset(data->data.setMayhem.mayhem);
			break;
		case Scriptable_EventType_PlayMayhem:
			dump_event_base();
			break;
		case Scriptable_EventType_ViewmodelShaderParam:
			dump_event_base();
			break;
		case Scriptable_EventType_ViewmodelChangeImage:
			dump_event_base();
			break;
		case Scriptable_EventType_ClientViewSelector:
			dump_event_base();
			dump.dump_array(data->data.clientViewSelector.events1p, data->data.clientViewSelector.event1pCount);
			for (unsigned int i = 0; i < data->data.clientViewSelector.event1pCount; i++)
			{
				dump_scriptable_event(dump, &data->data.clientViewSelector.events1p[i]);
			}
			dump.dump_array(data->data.clientViewSelector.events3p, data->data.clientViewSelector.event3pCount);
			for (unsigned int i = 0; i < data->data.clientViewSelector.event3pCount; i++)
			{
				dump_scriptable_event(dump, &data->data.clientViewSelector.events3p[i]);
			}
			break;
		case Scriptable_EventType_TeamSelector:
			dump_event_base();
			dump.dump_array(data->data.teamSelector.eventsPass, data->data.teamSelector.eventPassCount);
			for (unsigned int i = 0; i < data->data.teamSelector.eventPassCount; i++)
			{
				dump_scriptable_event(dump, &data->data.teamSelector.eventsPass[i]);
			}
			dump.dump_array(data->data.teamSelector.eventsFail, data->data.teamSelector.eventFailCount);
			for (unsigned int i = 0; i < data->data.teamSelector.eventFailCount; i++)
			{
				dump_scriptable_event(dump, &data->data.teamSelector.eventsFail[i]);
			}
			break;
		case Scriptable_EventType_AddModel:
			dump_event_base();
			dump.dump_string(data->data.addModel.tagName);
			dump.dump_string(SL_ConvertToString(data->data.addModel.scrTagName));
			dump.dump_asset(data->data.addModel.model);
			break;
		case Scriptable_EventType_ApplyForce:
			dump_event_base();
			break;
		case Scriptable_EventType_CompassIcon:
			dump_event_base();
			dump.dump_asset(data->data.compassIcon.friendlyArrow);
			dump.dump_asset(data->data.compassIcon.friendlyFiring);
			dump.dump_asset(data->data.compassIcon.friendlyChatting);
			dump.dump_asset(data->data.compassIcon.friendlyYelling);
			dump.dump_asset(data->data.compassIcon.partyArrow);
			dump.dump_asset(data->data.compassIcon.partyFiring);
			dump.dump_asset(data->data.compassIcon.partyChatting);
			dump.dump_asset(data->data.compassIcon.partyYelling);
			dump.dump_asset(data->data.compassIcon.squadArrow);
			dump.dump_asset(data->data.compassIcon.squadFiring);
			for (auto i = 0; i < 3; i++)
			{
				dump.dump_asset(data->data.compassIcon.enemyCompassIconQuiet[i]);
			}
			for (auto i = 0; i < 3; i++)
			{
				dump.dump_asset(data->data.compassIcon.enemyCompassIconFiring[i]);
			}
			dump.dump_asset(data->data.compassIcon.enemyCompassIconDirectional);
			break;
		case Scriptable_EventType_MaterialOverride:
			dump_event_base();
			dump.dump_asset(data->data.materialOverride.material);
			break;
		}
	}

	void scriptable_def::dump_state_base(assetmanager::dumper& dump, ScriptableStateBaseDef* data)
	{
		dump.dump_string(data->name);
		dump.dump_array(data->events, data->numEvents);
		for (unsigned int i = 0; i < data->numEvents; i++)
		{
			dump_scriptable_event(dump, &data->events[i]);
		}
	}

	void scriptable_def::dump_state(assetmanager::dumper& dump, ScriptableStateDef* data)
	{
		dump_state_base(dump, &data->base);
		if (data->type == Scriptable_StateType_Simple)
		{
			dump.dump_single(data->data.simple.base);
			if (data->data.simple.base)
			{
				dump_state_base(dump, data->data.simple.base);
			}
		}
		else if (data->type == Scriptable_StateType_Health)
		{
			dump.dump_single(data->data.health.base);
			if (data->data.health.base)
			{
				dump_state_base(dump, data->data.scripted.base);
			}
			dump.dump_string(data->data.health.script_id);
			dump.dump_string(SL_ConvertToString(data->data.health.scrScript_id));
		}
		else if (data->type == Scriptable_StateType_Scripted)
		{
			dump.dump_single(data->data.scripted.base);
			if (data->data.scripted.base)
			{
				dump_state_base(dump, data->data.scripted.base);
			}
			dump.dump_string(data->data.scripted.script_id);
			dump.dump_string(SL_ConvertToString(data->data.scripted.scrScript_id));
		}
	}

	void scriptable_def::dump_part(assetmanager::dumper& dump, ScriptablePartDef* data)
	{
		dump.dump_string(data->name);
		dump.dump_string(SL_ConvertToString(data->scrName));
		dump.dump_string(data->tagName);
		dump.dump_string(SL_ConvertToString(data->scrTagName));

		dump.dump_array(data->states, data->numStates);
		for (unsigned int i = 0; i < data->numStates; i++)
		{
			dump_state(dump, &data->states[i]);
		}

		dump.dump_array(data->childParts, data->numChildParts);
		for (unsigned int i = 0; i < data->numChildParts; i++)
		{
			dump_part(dump, &data->childParts[i]);
		}

		dump.dump_array(data->damageTagOverrides, data->numDamageTagOverrides);
		for (unsigned int i = 0; i < data->numDamageTagOverrides; i++)
		{
			dump.dump_string(data->damageTagOverrides[i].tag);
			dump.dump_string(SL_ConvertToString(data->damageTagOverrides[i].scrTag));
		}
	}

	void scriptable_def::dump(ScriptableDef* asset)
	{
		const auto path = "scriptable\\"s + asset->name;

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->nextScriptableDef);

		dump.dump_array(asset->parts, asset->numParts);
		for (unsigned int i = 0; i < asset->numParts; i++)
		{
			dump_part(dump, &asset->parts[i]);
		}

		dump.dump_string(SL_ConvertToString(asset->animationTreeName));

		dump.dump_array(asset->models, asset->numXModels);
		for (unsigned int i = 0; i < asset->numXModels; i++)
		{
			dump.dump_asset(asset->models[i]);
		}

		dump.close();
	}
}