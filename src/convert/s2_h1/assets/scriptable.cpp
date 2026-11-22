#include <std_include.hpp>

#include "../include.hpp"
#include "fxeffectdef.hpp"
#include "scriptable.hpp"
#include "sound.hpp"
#include "xanim.hpp"
#include "xmodel.hpp"

namespace zonetool::s2::converter::h1::scriptable
{
	namespace
	{
		constexpr unsigned int part_timer_size = 4;

std::unordered_set<std::string> dumped_defs;

		struct stream_allocator
		{
			unsigned int cursor;
			unsigned int part_size;

			unsigned char allocate(const unsigned int size)
			{
				const auto index = this->cursor;
				this->cursor += size;
				this->part_size = std::max(this->part_size, this->cursor);
				return static_cast<unsigned char>(index);
			}
		};

		unsigned char convert_damage_flags(const unsigned short flags)
		{
			unsigned char new_flags = 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_BULLETS) ? zonetool::h1::SCRIPTABLE_DAMAGE_GENERIC : 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_SPLASH) ? zonetool::h1::SCRIPTABLE_DAMAGE_SPLASH : 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_MELEE) ? zonetool::h1::SCRIPTABLE_DAMAGE_MELEE : 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_PLAYER) ? zonetool::h1::SCRIPTABLE_DAMAGE_PLAYER : 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_AI) ? zonetool::h1::SCRIPTABLE_DAMAGE_AI : 0;
			new_flags |= (flags & SCRIPTABLE_DAMAGE_OTHER) ? zonetool::h1::SCRIPTABLE_DAMAGE_OTHER : 0;
			return new_flags;
		}

		zonetool::h1::XModel* use_model(XModel* model)
		{
			xmodel::reference(model);
			return reinterpret_cast<zonetool::h1::XModel*>(model);
		}

		zonetool::h1::FxEffectDef* use_effect(FxEffectDef* effect)
		{
			if (effect)
			{
				fxeffectdef::dump(effect);
			}

			return reinterpret_cast<zonetool::h1::FxEffectDef*>(effect);
		}

		zonetool::h1::snd_alias_list_t* use_sound(snd_alias_list_t* alias)
		{
			if (!alias || !alias->name || !sound::dump_used(alias->name))
			{
				return nullptr;
			}

			return reinterpret_cast<zonetool::h1::snd_alias_list_t*>(alias);
		}

		bool convert_event(const ScriptableDef* asset, const ScriptableEventDef& event, zonetool::h1::ScriptableEventDef& new_event, stream_allocator& streams)
		{
			const auto& data = event.data;
			auto& new_data = new_event.data;
			switch (event.type)
			{
			case SCRIPTABLE_EVENT_STATE_MODEL:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_MODEL;
				new_data.setModel.model = data.stateModel.noModel ? nullptr : use_model(data.stateModel.model);
				return true;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				new_event.type = event.type == SCRIPTABLE_EVENT_FX ? zonetool::h1::SCRIPTABLE_EVENT_FX : zonetool::h1::SCRIPTABLE_EVENT_STOP_FX;
				new_data.playFx.handle = use_effect(data.fx.effect);
				new_data.playFx.tagName = static_cast<zonetool::h1::scr_string_t>(data.fx.tag);
				new_data.playFx.loopTime = data.fx.loopTime;
				new_data.playFx.loopTimeStreamIndex = data.fx.loopTime ? streams.allocate(4) : 0;
				new_data.playFx.tagUseAngles = data.fx.tagUseAngles;
				return true;
			case SCRIPTABLE_EVENT_SOUND:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_SOUND;
				new_data.playSound.alias = use_sound(data.sound.alias);
				new_data.playSound.looping = data.sound.looping;
				return new_data.playSound.alias != nullptr;
			case SCRIPTABLE_EVENT_ANIMATION:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_ANIMATION;
				new_data.playAnim.animName = data.anim.animName;
				if (data.anim.animName && !xanim::dump_used(data.anim.animName))
				{
					ZONETOOL_WARNING("scriptable \"%s\": xanim \"%s\" is not converted", asset->name, data.anim.animName);
				}

				new_data.playAnim.override = data.anim.override;
				new_data.playAnim.animEntryIndex = data.anim.animEntryIndex;
				new_data.playAnim.animPlaybackStreamIndex = streams.allocate(16);
				new_data.playAnim.timeOffsetMin = data.anim.timeOffsetMin;
				new_data.playAnim.timeOffsetMax = data.anim.timeOffsetMax;
				new_data.playAnim.playbackRateMin = data.anim.playbackRateMin;
				new_data.playAnim.playbackRateMax = data.anim.playbackRateMax;
				new_data.playAnim.blendTime = data.anim.blendTime;
				return true;
			case SCRIPTABLE_EVENT_EXPLODE:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_EXPLODE;
				new_data.doExplosion.forceMin = data.explode.forceMin;
				new_data.doExplosion.forceMax = data.explode.forceMax;
				new_data.doExplosion.radius = data.explode.radius;
				new_data.doExplosion.damageMin = data.explode.damageMin;
				new_data.doExplosion.damageMax = data.explode.damageMax;
				new_data.doExplosion.aiAvoid = data.explode.aiAvoid;
				return true;
			case SCRIPTABLE_EVENT_HEALTHDRAIN:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_HEALTHDRAIN;
				new_data.healthDrain.amount = data.healthDrain.amount;
				new_data.healthDrain.interval = data.healthDrain.interval;
				new_data.healthDrain.badPlaceRadius = data.healthDrain.badPlaceRadius;
				new_data.healthDrain.streamIndex = streams.allocate(8);
				return true;
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_PHYSICSLAUNCH;
				new_data.physicsLaunch.model = use_model(data.physicsLaunch.model);
				new_data.physicsLaunch.launchDirX = data.physicsLaunch.launchDirX;
				new_data.physicsLaunch.launchDirY = data.physicsLaunch.launchDirY;
				new_data.physicsLaunch.launchDirZ = data.physicsLaunch.launchDirZ;
				new_data.physicsLaunch.explForceScale = data.physicsLaunch.explForceScale;
				new_data.physicsLaunch.bulletForceScale = data.physicsLaunch.bulletForceScale;
				return true;
			case SCRIPTABLE_EVENT_LIGHTSETTINGS:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_LIGHTSETTINGS;
				for (auto i = 0; i < 4; i++)
				{
					new_data.lightSettings.color[i] = static_cast<unsigned char>(std::clamp(data.lightSettings.color[i] * 255.0f + 0.5f, 0.0f, 255.0f));
				}

				new_data.lightSettings.lightIndexConstIndex = data.lightSettings.lightIndexConstIndex;
				new_data.lightSettings.transStateStreamIndex = streams.allocate(0x20);
				new_data.lightSettings.flags = data.lightSettings.flags;
				new_data.lightSettings.intensityScaleMin = data.lightSettings.intensityScaleMin;
				new_data.lightSettings.intensityScaleMax = data.lightSettings.intensityScaleMax;
				new_data.lightSettings.radiusScaleMin = data.lightSettings.radiusScaleMin;
				new_data.lightSettings.radiusScaleMax = data.lightSettings.radiusScaleMax;
				new_data.lightSettings.transitionTimeMin = data.lightSettings.transitionTimeMin;
				new_data.lightSettings.transitionTimeMax = data.lightSettings.transitionTimeMax;
				new_data.lightSettings.noteworthy = data.lightSettings.noteworthy;
				return true;
			case SCRIPTABLE_EVENT_SHAKE:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_SHAKE;
				static_assert(sizeof(new_data.shake) == sizeof(data.shake));
				std::memcpy(&new_data.shake, &data.shake, sizeof(data.shake));
				return true;
			case SCRIPTABLE_EVENT_TRANSLATE:
			case SCRIPTABLE_EVENT_ROTATE:
				new_event.type = event.type == SCRIPTABLE_EVENT_TRANSLATE ? zonetool::h1::SCRIPTABLE_EVENT_TRANSLATE : zonetool::h1::SCRIPTABLE_EVENT_ROTATE;
				static_assert(sizeof(new_data.translate) == sizeof(data.translate) && sizeof(new_data.rotate) == sizeof(data.rotate));
				std::memcpy(&new_data.translate, &data.translate, sizeof(data.translate));
				new_data.translate.transStateStreamIndex = streams.allocate(0x20);
				return true;
			case SCRIPTABLE_EVENT_STATECHANGE:
				new_event.type = zonetool::h1::SCRIPTABLE_EVENT_STATECHANGE;
				new_data.stateChange.targetIndex = data.stateChange.targetIndex;
				new_data.stateChange.delayStreamIndex = streams.allocate(4);
				new_data.stateChange.delayMin = data.stateChange.delayMin;
				new_data.stateChange.delayMax = data.stateChange.delayMax;
				return true;
			default:
				ZONETOOL_WARNING("scriptable \"%s\": dropping s2 event type %d", asset->name, event.type);
				return false;
			}
		}

		bool has_state_change(const ScriptablePartDef& part)
		{
			for (auto i = 0; part.states && i < part.stateCount; i++)
			{
				for (auto j = 0; part.states[i].events && j < part.states[i].eventCount; j++)
				{
					if (part.states[i].events[j].type == SCRIPTABLE_EVENT_STATECHANGE)
					{
						return true;
					}
				}
			}

			return false;
		}

		unsigned int convert_part(const ScriptableDef* asset, const ScriptablePartDef& part, zonetool::h1::ScriptablePartDef& new_part,
			const unsigned int stream_base, utils::memory::allocator& allocator)
		{
			new_part.name = static_cast<zonetool::h1::scr_string_t>(part.name);
			new_part.flags = part.partFlags;

			stream_allocator streams{stream_base, stream_base};
			if (has_state_change(part))
			{
				new_part.eventStreamTimeRemainIndex = streams.allocate(part_timer_size);
				new_part.eventStreamNextChangeTimeIndex = streams.allocate(part_timer_size);
			}

			const auto state_base = streams.cursor;
			new_part.stateCount = part.stateCount;
			new_part.states = allocator.allocate_array<zonetool::h1::ScriptableStateDef>(part.stateCount);
			for (auto i = 0; i < part.stateCount; i++)
			{
				const auto& state = part.states[i];
				auto& new_state = new_part.states[i];
				new_state.name = static_cast<zonetool::h1::scr_string_t>(state.name);
				new_state.tagName = static_cast<zonetool::h1::scr_string_t>(state.tagName);
				new_state.damageFlags = convert_damage_flags(state.damageFlags);
				new_state.damageParentTransferRate = state.damageParentTransferRate;
				new_state.damageParentReceiveRate = state.damageParentReceiveRate;
				new_state.maxHealth = state.maxHealth;

				streams.cursor = state_base;
				if (!state.events || !state.eventCount)
				{
					continue;
				}

				new_state.onEnterEvents = allocator.allocate_array<zonetool::h1::ScriptableEventDef>(state.eventCount);
				for (auto j = 0; j < state.eventCount; j++)
				{
					if (convert_event(asset, state.events[j], new_state.onEnterEvents[new_state.onEnterEventCount], streams))
					{
						new_state.onEnterEventCount++;
					}
				}
			}

			return streams.part_size - stream_base;
		}

		void convert_notetracks(const ScriptableDef* asset, zonetool::h1::ScriptableDef* new_asset, utils::memory::allocator& allocator)
		{
			if (!asset->notetracks || !asset->notetrackCount)
			{
				return;
			}

			new_asset->notetracks = allocator.allocate_array<zonetool::h1::ScriptableNotetrackDef>(asset->notetrackCount);
			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				const auto& notetrack = asset->notetracks[i];
				if (notetrack.type == SCRIPTABLE_NOTETRACK_SOUND)
				{
					if (auto* alias = use_sound(notetrack.data.sound.alias))
					{
						auto& new_notetrack = new_asset->notetracks[new_asset->notetrackCount++];
						new_notetrack.name = static_cast<zonetool::h1::scr_string_t>(notetrack.name);
						new_notetrack.type = zonetool::h1::SCRIPTABLE_NT_SOUND;
						new_notetrack.data.playSound.alias = alias;
					}

					continue;
				}

				auto& new_notetrack = new_asset->notetracks[new_asset->notetrackCount++];
				new_notetrack.name = static_cast<zonetool::h1::scr_string_t>(notetrack.name);
				new_notetrack.type = zonetool::h1::SCRIPTABLE_NT_FX;
				new_notetrack.data.playFx.handle = use_effect(notetrack.data.fx.effect);
				new_notetrack.data.playFx.tagName = static_cast<zonetool::h1::scr_string_t>(notetrack.data.fx.tag);
				new_notetrack.data.playFx.useAngles = notetrack.data.fx.tagUseAngles;
			}
		}

		zonetool::h1::ScriptableDef* convert(ScriptableDef* asset, utils::memory::allocator& allocator)
		{
			if (asset->partCount > std::numeric_limits<unsigned char>::max())
			{
				ZONETOOL_WARNING("scriptable \"%s\": %u parts do not fit h1", asset->name, asset->partCount);
				return nullptr;
			}

			const auto new_asset = allocator.allocate<zonetool::h1::ScriptableDef>();
			new_asset->name = asset->name;
			new_asset->baseModel = use_model(asset->baseModel);
			new_asset->baseCollisionBrush = asset->baseCollision && *asset->baseCollision == '*' ? asset->baseCollision : nullptr;
			new_asset->type = zonetool::h1::SCRIPTABLE_TYPE_GENERAL;
			new_asset->serverInstancePartCount = static_cast<unsigned char>(asset->serverInstancePartCount);
			new_asset->serverControlledPartCount = static_cast<unsigned char>(asset->serverControlledPartCount);

			auto stream_size = 0u;
			new_asset->partCount = static_cast<unsigned char>(asset->partCount);
			new_asset->parts = allocator.allocate_array<zonetool::h1::ScriptablePartDef>(asset->partCount);
			for (auto i = 0; i < asset->partCount; i++)
			{
				stream_size += convert_part(asset, asset->parts[i], new_asset->parts[i], stream_size, allocator);
			}

			if (stream_size > std::numeric_limits<unsigned char>::max())
			{
				ZONETOOL_WARNING("scriptable \"%s\": event stream size %u does not fit h1", asset->name, stream_size);
				return nullptr;
			}

			new_asset->eventStreamSize = static_cast<unsigned char>(stream_size);
			new_asset->eventConstantsSize = asset->flagsSize;
			convert_notetracks(asset, new_asset, allocator);
			return new_asset;
		}

		zonetool::h1::ScriptableDef* dump_def(ScriptableDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = convert(asset, allocator);
			if (new_asset && dumped_defs.insert(asset->name).second)
			{
				zonetool::h1::scriptable_def::dump(new_asset);
				add_csv_row("scriptable", asset->name);
			}

			return new_asset;
		}
	}

	zonetool::h1::ScriptableMapEnts convert_map_ents(const ScriptableMapEnts& map_ents, utils::memory::allocator& allocator)
	{
		zonetool::h1::ScriptableMapEnts new_map_ents{};
		if (!map_ents.instances || !map_ents.instanceCount)
		{
			return new_map_ents;
		}

		new_map_ents.animEntryCount = map_ents.animEntryCount;
		new_map_ents.animEntries = map_ents.animEntryCount ? allocator.allocate_array<zonetool::h1::ScriptableAnimationEntry>(map_ents.animEntryCount) : nullptr;
		for (auto i = 0u; map_ents.animEntries && i < map_ents.animEntryCount; i++)
		{
			new_map_ents.animEntries[i].animName = map_ents.animEntries[i].animName;
			if (map_ents.animEntries[i].animName)
			{
				xanim::dump_used(map_ents.animEntries[i].animName);
			}
		}

		std::unordered_map<ScriptableDef*, zonetool::h1::ScriptableDef*> defs;
		std::vector<zonetool::h1::ScriptableInstance> instances;
		for (auto i = 0u; i < map_ents.instanceCount; i++)
		{
			const auto& instance = map_ents.instances[i];
			if (!instance.def)
			{
				continue;
			}

			auto [entry, inserted] = defs.try_emplace(instance.def, nullptr);
			if (inserted)
			{
				entry->second = dump_def(instance.def, allocator);
			}

			const auto def = entry->second;
			if (!def)
			{
				continue;
			}

			auto& new_instance = instances.emplace_back();
			new_instance.def = def;
			new_instance.eventConstantsBuf = def->eventConstantsSize ? allocator.allocate_array<unsigned char>(def->eventConstantsSize) : nullptr;
			if (new_instance.eventConstantsBuf && instance.flags)
			{
				std::memcpy(new_instance.eventConstantsBuf, instance.flags, def->eventConstantsSize);
			}

			new_instance.partStates = allocator.allocate_array<zonetool::h1::ScriptableInstancePartState>(def->partCount);
			new_instance.eventStreamBuf = def->eventStreamSize ? allocator.allocate_array<unsigned char>(def->eventStreamSize) : nullptr;
			if (instance.partStates && instance.partStateCount)
			{
				new_instance.targetCount = instance.partStateCount;
				new_instance.targetData = allocator.allocate_array<zonetool::h1::ScriptableInstanceTargetData>(instance.partStateCount);
				for (auto j = 0; j < instance.partStateCount; j++)
				{
					const auto& state = instance.partStates[j];
					auto& target = new_instance.targetData[j];
					target.lightIndex = state.lightIndex;
					std::memcpy(target.lightAxis, state.lightAxis, sizeof(target.lightAxis));
					std::memcpy(target.lightColorStorage, state.lightColorStorage, sizeof(target.lightColorStorage));
					target.lightRadiusStorage = state.lightRadiusStorage;
				}
			}

			std::memcpy(new_instance.origin, instance.origin, sizeof(new_instance.origin));
			std::memcpy(new_instance.angles, instance.angles, sizeof(new_instance.angles));
			std::memcpy(new_instance.startOrigin, instance.startOrigin, sizeof(new_instance.startOrigin));
			std::memcpy(new_instance.startAngles, instance.startAngles, sizeof(new_instance.startAngles));
			new_instance.targetname = static_cast<zonetool::h1::scr_string_t>(instance.name);
			new_instance.updateNextInstance = 65000;

			new_map_ents.instanceStateSize += def->partCount * sizeof(zonetool::h1::ScriptableInstancePartState) + def->eventStreamSize + def->eventConstantsSize
				+ new_instance.targetCount * sizeof(zonetool::h1::ScriptableInstanceTargetData);
			if (def->serverInstancePartCount)
			{
				new_map_ents.replicatedInstanceCount++;
			}
		}

		new_map_ents.instanceCount = static_cast<unsigned int>(instances.size());
		new_map_ents.instances = allocator.allocate_array<zonetool::h1::ScriptableInstance>(instances.size());
		std::ranges::copy(instances, new_map_ents.instances);
		ZONETOOL_INFO("scriptables: %u of %u instances, %zu defs, %u anim entries", new_map_ents.instanceCount, map_ents.instanceCount, defs.size(),
			new_map_ents.animEntryCount);
		return new_map_ents;
	}
}
