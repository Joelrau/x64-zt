#include <std_include.hpp>

#include "../include.hpp"
#include "scriptable.hpp"
#include "sound.hpp"
#include "techset.hpp"
#include "xanim.hpp"

namespace zonetool::h1::converter::s2::scriptable
{
	namespace
	{
		// event stream bytes per event, 2.5 sub_140608F00 (anim playback is 12 bytes) and ScriptableSv_UpdateLoopingEvents (drain time + badplace id)
		constexpr unsigned int anim_stream_size = 12;
		constexpr unsigned int health_drain_stream_size = 8;
		constexpr unsigned int time_stream_size = 4;
		constexpr unsigned int light_stream_size = 0x28; // h1 0x20


		// h1 damage bits: 1 generic, 2 splash, 4 melee, 8 player, 0x10 ai/vehicle, 0x20 other (ScriptableBg_Damage_ProcessPartDamage)
		// s2 bits: 0x8F bullet classes, 0x10 splash, 0x20 melee, 0x40 mod 11, 0x100 player, 0x200 ai/vehicle, 0x400 other (2.5 sub_140609C00)
		unsigned short convert_damage_flags(const unsigned char flags)
		{
			unsigned short new_flags = 0;
			new_flags |= (flags & 0x1) ? 0x8F : 0;
			new_flags |= (flags & 0x2) ? 0x10 : 0;
			new_flags |= (flags & 0x4) ? 0x20 : 0;
			new_flags |= (flags & 0x8) ? 0x100 : 0;
			new_flags |= (flags & 0x10) ? 0x200 : 0;
			new_flags |= (flags & 0x20) ? 0x400 : 0;
			return new_flags;
		}

		struct stream_allocator
		{
			unsigned int part_base;
			unsigned int cursor;
			unsigned int part_size;

			unsigned char allocate(const unsigned int size)
			{
				const auto index = this->cursor;
				this->cursor += size;
				this->part_size = std::max(this->part_size, this->cursor - this->part_base);
				return static_cast<unsigned char>(index);
			}
		};

		struct def_context
		{
			ScriptableDef* asset;
			std::vector<zonetool::s2::XModel*> models;
			std::vector<unsigned char> part_anim_entries;
		};

		unsigned char model_index(def_context& ctx, XModel* model)
		{
			const auto new_model = reinterpret_cast<zonetool::s2::XModel*>(model);
			const auto itr = std::find(ctx.models.begin(), ctx.models.end(), new_model);
			if (itr != ctx.models.end())
			{
				return static_cast<unsigned char>(itr - ctx.models.begin());
			}

			ctx.models.emplace_back(new_model);
			return static_cast<unsigned char>(ctx.models.size() - 1);
		}

		constexpr std::string_view anim_tree_name = "animtrees/destructibles.atr";
		constexpr std::string_view anim_tree_override = "override/animtrees/destructibles.atr"; // s2x reads override/<name> rawfiles from the map zone

		std::set<std::string> map_anims;

		// assets from the h1 common zones are converted with the scriptable, s2 skips the ones it already ships
		void dump_used_anim(const char* name)
		{
			if (name && !xanim::dump_used(name))
			{
				ZONETOOL_WARNING("xanim \"%s\" not found in the loaded h1 zones", name);
			}
		}

		bool convert_event(def_context& ctx, const ScriptableEventDef& event, zonetool::s2::ScriptableEventDef& new_event, stream_allocator& streams)
		{
			const auto& data = event.data;
			auto& new_data = new_event.data;
			switch (event.type)
			{
			case SCRIPTABLE_EVENT_MODEL:
				// h1 swaps the instance model, s2 does it with the state model event (index into def->models)
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_STATE_MODEL;
				new_data.stateModel.model = reinterpret_cast<zonetool::s2::XModel*>(data.setModel.model);
				new_data.stateModel.name = "";
				new_data.stateModel.modelIndex = model_index(ctx, data.setModel.model);
				new_data.stateModel.noModel = data.setModel.model == nullptr;
				new_data.stateModel.noBrush = true;
				return true;
			case SCRIPTABLE_EVENT_FX:
			case SCRIPTABLE_EVENT_STOP_FX:
				new_event.type = event.type == SCRIPTABLE_EVENT_FX ? zonetool::s2::SCRIPTABLE_EVENT_FX : zonetool::s2::SCRIPTABLE_EVENT_STOP_FX;
				new_data.fx.effect = reinterpret_cast<zonetool::s2::FxEffectDef*>(data.playFx.handle);
				new_data.fx.tag = static_cast<zonetool::s2::scr_string_t>(data.playFx.tagName);
				new_data.fx.loopTime = data.playFx.loopTime;
				new_data.fx.loopTimeStreamIndex = data.playFx.loopTime ? streams.allocate(time_stream_size) : 0;
				new_data.fx.tagUseAngles = data.playFx.tagUseAngles;
				return true;
			case SCRIPTABLE_EVENT_SOUND:
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_SOUND;
				new_data.sound.alias = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(data.playSound.alias);
				sound::dump_used(data.playSound.alias);
				new_data.sound.looping = data.playSound.looping;
				return true;
			case SCRIPTABLE_EVENT_ANIMATION:
				// the entry index points into the map anim entries (destructibles animtree), the part lists the entries it blends between
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_ANIMATION;
				new_data.anim.animName = data.playAnim.animName;
				dump_used_anim(data.playAnim.animName);
				new_data.anim.timeOffsetMin = data.playAnim.timeOffsetMin;
				new_data.anim.timeOffsetMax = data.playAnim.timeOffsetMax;
				new_data.anim.playbackRateMin = data.playAnim.playbackRateMin;
				new_data.anim.playbackRateMax = data.playAnim.playbackRateMax;
				new_data.anim.blendTime = data.playAnim.blendTime;
				new_data.anim.override = data.playAnim.override;
				new_data.anim.animEntryIndex = data.playAnim.animEntryIndex;
				new_data.anim.animPlaybackStreamIndex = streams.allocate(anim_stream_size);
				if (std::find(ctx.part_anim_entries.begin(), ctx.part_anim_entries.end(), data.playAnim.animEntryIndex) == ctx.part_anim_entries.end())
				{
					ctx.part_anim_entries.emplace_back(data.playAnim.animEntryIndex);
				}
				return true;
			case SCRIPTABLE_EVENT_EXPLODE:
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_EXPLODE;
				new_data.explode.forceMin = data.doExplosion.forceMin;
				new_data.explode.forceMax = data.doExplosion.forceMax;
				new_data.explode.forceRadius = data.doExplosion.radius;
				new_data.explode.radius = data.doExplosion.radius;
				new_data.explode.damageMin = data.doExplosion.damageMin;
				new_data.explode.damageMax = data.doExplosion.damageMax;
				new_data.explode.aiAvoid = data.doExplosion.aiAvoid;
				return true;
			case SCRIPTABLE_EVENT_HEALTHDRAIN:
				// s2 places the badplace once health is at or below badPlaceHealth, h1 places it right away
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_HEALTHDRAIN;
				new_data.healthDrain.amount = data.healthDrain.amount;
				new_data.healthDrain.interval = data.healthDrain.interval;
				new_data.healthDrain.badPlaceHealth = std::numeric_limits<unsigned short>::max();
				new_data.healthDrain.badPlaceHeight = 64;
				new_data.healthDrain.badPlaceRadius = data.healthDrain.badPlaceRadius;
				new_data.healthDrain.streamIndex = streams.allocate(health_drain_stream_size);
				return true;
			case SCRIPTABLE_EVENT_PHYSICSLAUNCH:
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_PHYSICSLAUNCH;
				new_data.physicsLaunch.model = reinterpret_cast<zonetool::s2::XModel*>(data.physicsLaunch.model);
				new_data.physicsLaunch.launchDirX = data.physicsLaunch.launchDirX;
				new_data.physicsLaunch.launchDirY = data.physicsLaunch.launchDirY;
				new_data.physicsLaunch.launchDirZ = data.physicsLaunch.launchDirZ;
				new_data.physicsLaunch.explForceScale = data.physicsLaunch.explForceScale;
				new_data.physicsLaunch.bulletForceScale = data.physicsLaunch.bulletForceScale;
				return true;
			case SCRIPTABLE_EVENT_LIGHTSETTINGS:
				// same fields after a float color, the light index stays in the instance constants (s2 flags buffer)
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_LIGHTSETTINGS;
				for (auto i = 0; i < 4; i++)
				{
					new_data.lightSettings.color[i] = data.lightSettings.color[i] / 255.0f;
				}

				new_data.lightSettings.lightIndexConstIndex = data.lightSettings.lightIndexConstIndex;
				new_data.lightSettings.transStateStreamIndex = streams.allocate(light_stream_size);
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
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_SHAKE;
				static_assert(sizeof(new_data.shake) == sizeof(data.shake));
				std::memcpy(&new_data.shake, &data.shake, sizeof(data.shake));
				return true;
			case SCRIPTABLE_EVENT_STATECHANGE:
				new_event.type = zonetool::s2::SCRIPTABLE_EVENT_STATECHANGE;
				new_data.stateChange.targetIndex = data.stateChange.targetIndex;
				new_data.stateChange.delayStreamIndex = streams.allocate(time_stream_size);
				new_data.stateChange.delayMin = data.stateChange.delayMin;
				new_data.stateChange.delayMax = data.stateChange.delayMax;
				return true;
			default:
				ZONETOOL_WARNING("scriptable \"%s\": dropping unsupported event type %d", ctx.asset->name, event.type);
				return false;
			}
		}

		void convert_state(def_context& ctx, const ScriptableStateDef& state, zonetool::s2::ScriptableStateDef& new_state,
			stream_allocator& streams, utils::memory::allocator& allocator)
		{
			new_state.name = static_cast<zonetool::s2::scr_string_t>(state.name);
			new_state.tagName = static_cast<zonetool::s2::scr_string_t>(state.tagName);
			new_state.damageFlags = convert_damage_flags(state.damageFlags);
			new_state.damageParentTransferRate = state.damageParentTransferRate;
			new_state.damageParentReceiveRate = state.damageParentReceiveRate;
			new_state.maxHealth = state.maxHealth;

			if (!state.onEnterEvents || !state.onEnterEventCount)
			{
				return;
			}

			new_state.events = allocator.allocate_array<zonetool::s2::ScriptableEventDef>(state.onEnterEventCount);
			for (auto i = 0; i < state.onEnterEventCount; i++)
			{
				if (convert_event(ctx, state.onEnterEvents[i], new_state.events[new_state.eventCount], streams))
				{
					new_state.eventCount++;
				}
			}
		}

		bool has_state_change(const ScriptablePartDef& part)
		{
			for (auto i = 0; part.states && i < part.stateCount; i++)
			{
				const auto& state = part.states[i];
				for (auto j = 0; state.onEnterEvents && j < state.onEnterEventCount; j++)
				{
					if (state.onEnterEvents[j].type == SCRIPTABLE_EVENT_STATECHANGE)
					{
						return true;
					}
				}
			}

			return false;
		}

		// event streams are laid out per part, the states of a part never run together so they share it
		unsigned int convert_part(def_context& ctx, const ScriptablePartDef& part, zonetool::s2::ScriptablePartDef& new_part,
			const unsigned int stream_base, utils::memory::allocator& allocator)
		{
			new_part.name = static_cast<zonetool::s2::scr_string_t>(part.name);
			new_part.partFlags = part.flags;

			stream_allocator streams{stream_base, stream_base, 0};
			if (has_state_change(part))
			{
				new_part.eventStreamTimeRemainIndex = streams.allocate(time_stream_size);
				new_part.eventStreamNextChangeTimeIndex = streams.allocate(time_stream_size);
			}

			const auto state_base = streams.cursor;
			ctx.part_anim_entries.clear();
			new_part.stateCount = part.stateCount;
			new_part.states = allocator.allocate_array<zonetool::s2::ScriptableStateDef>(part.stateCount);
			for (auto i = 0; i < part.stateCount; i++)
			{
				streams.cursor = state_base;
				convert_state(ctx, part.states[i], new_part.states[i], streams, allocator);
			}

			new_part.flagCount = static_cast<unsigned char>(ctx.part_anim_entries.size());
			new_part.flags = copy_array(ctx.part_anim_entries.data(), ctx.part_anim_entries.size(), allocator);

			return streams.part_size;
		}

		void convert_notetracks(const ScriptableDef* asset, zonetool::s2::ScriptableDef* new_asset, utils::memory::allocator& allocator)
		{
			if (!asset->notetracks || !asset->notetrackCount)
			{
				return;
			}

			new_asset->notetrackCount = asset->notetrackCount;
			new_asset->notetracks = allocator.allocate_array<zonetool::s2::ScriptableNotetrackDef>(asset->notetrackCount);
			for (auto i = 0; i < asset->notetrackCount; i++)
			{
				const auto& notetrack = asset->notetracks[i];
				auto& new_notetrack = new_asset->notetracks[i];
				new_notetrack.name = static_cast<zonetool::s2::scr_string_t>(notetrack.name);
				new_notetrack.type = notetrack.type;
				if (notetrack.type == SCRIPTABLE_NT_FX)
				{
					new_notetrack.data.fx.effect = reinterpret_cast<zonetool::s2::FxEffectDef*>(notetrack.data.playFx.handle);
					new_notetrack.data.fx.tag = static_cast<zonetool::s2::scr_string_t>(notetrack.data.playFx.tagName);
					new_notetrack.data.fx.tagUseAngles = notetrack.data.playFx.useAngles;
				}
				else
				{
					new_notetrack.data.sound.alias = reinterpret_cast<zonetool::s2::snd_alias_list_t*>(notetrack.data.playSound.alias);
					sound::dump_used(notetrack.data.playSound.alias);
				}
			}
		}

		// the target lights of a light scriptable (h1 targetData) are s2 part states with a 4 float color
		void convert_target_lights(const ScriptableInstance& instance, zonetool::s2::ScriptableInstance& new_instance, utils::memory::allocator& allocator)
		{
			if (!instance.targetData || !instance.targetCount)
			{
				return;
			}

			new_instance.partStateCount = instance.targetCount;
			new_instance.partStates = allocator.allocate_array<zonetool::s2::ScriptableInstancePartState>(instance.targetCount);
			for (auto i = 0; i < instance.targetCount; i++)
			{
				const auto& target = instance.targetData[i];
				auto& state = new_instance.partStates[i];
				state.lightIndex = target.lightIndex;
				std::memcpy(state.lightAxis, target.lightAxis, sizeof(state.lightAxis));
				std::memcpy(state.lightColorStorage, target.lightColorStorage, sizeof(target.lightColorStorage));
				state.lightRadiusStorage = target.lightRadiusStorage;
			}
		}

		unsigned int instance_state_size(const zonetool::s2::ScriptableDef* def, const zonetool::s2::ScriptableInstance& instance)
		{
			if (!def)
			{
				return 0;
			}

			// sub_140602750
			const auto size = def->partCount * sizeof(*instance.partRuntime) + def->eventStreamSize + def->flagsSize
				+ instance.partStateCount * sizeof(zonetool::s2::ScriptableInstancePartState) + def->modelTagCount;
			return static_cast<unsigned int>(size);
		}

		zonetool::s2::ScriptableDef* convert(ScriptableDef* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::ScriptableDef>();
			new_asset->name = asset->name;
			new_asset->baseModel = reinterpret_cast<zonetool::s2::XModel*>(asset->baseModel);
			new_asset->baseCollision = asset->baseCollisionBrush ? asset->baseCollisionBrush : "";
			new_asset->serverInstancePartCount = asset->serverInstancePartCount;
			new_asset->serverControlledPartCount = asset->serverControlledPartCount;

			def_context ctx{asset};
			if (asset->baseModel)
			{
				model_index(ctx, asset->baseModel);
			}

			auto stream_size = 0u;
			new_asset->partCount = asset->partCount;
			new_asset->parts = allocator.allocate_array<zonetool::s2::ScriptablePartDef>(asset->partCount);
			for (auto i = 0; i < asset->partCount; i++)
			{
				stream_size += convert_part(ctx, asset->parts[i], new_asset->parts[i], stream_size, allocator);
			}

			if (stream_size > std::numeric_limits<unsigned char>::max())
			{
				ZONETOOL_FATAL("scriptable \"%s\": event stream size %u does not fit s2", asset->name, stream_size);
			}

			new_asset->eventStreamSize = static_cast<unsigned char>(stream_size);
			new_asset->flagsSize = asset->eventConstantsSize;
			convert_notetracks(asset, new_asset, allocator);

			new_asset->modelCount = static_cast<unsigned char>(ctx.models.size());
			new_asset->models = allocator.allocate_array<zonetool::s2::XModel*>(ctx.models.size());
			std::copy(ctx.models.begin(), ctx.models.end(), new_asset->models);

			return new_asset;
		}
	}

	zonetool::s2::ScriptableMapEnts convert_map_ents(const ScriptableMapEnts& map_ents, utils::memory::allocator& allocator)
	{
		zonetool::s2::ScriptableMapEnts new_map_ents{};
		if (!map_ents.instances || !map_ents.instanceCount)
		{
			return new_map_ents;
		}

		new_map_ents.animEntryCount = map_ents.animEntryCount;
		new_map_ents.animEntries = map_ents.animEntryCount ? allocator.allocate_array<zonetool::s2::ScriptableAnimationEntry>(map_ents.animEntryCount) : nullptr;
		for (auto i = 0u; map_ents.animEntries && i < map_ents.animEntryCount; i++)
		{
			new_map_ents.animEntries[i].animName = map_ents.animEntries[i].animName;
			dump_used_anim(map_ents.animEntries[i].animName);
			if (map_ents.animEntries[i].animName)
			{
				map_anims.emplace(map_ents.animEntries[i].animName);
			}
		}

		std::unordered_map<ScriptableDef*, zonetool::s2::ScriptableDef*> defs;
		new_map_ents.instanceCount = map_ents.instanceCount;
		new_map_ents.instances = allocator.allocate_array<zonetool::s2::ScriptableInstance>(map_ents.instanceCount);
		for (auto i = 0u; i < map_ents.instanceCount; i++)
		{
			const auto& instance = map_ents.instances[i];
			auto& new_instance = new_map_ents.instances[i];

			if (instance.def)
			{
				auto& def = defs[instance.def];
				if (!def)
				{
					def = convert(instance.def, allocator);
				}

				// s2 clears these on the mapents instances themselves (2.5 sub_14060E850), the writer only emits the non null ones
				new_instance.def = def;
				new_instance.partRuntime = allocator.allocate_array<char[4]>(def->partCount);
				new_instance.eventStreamBuffer = def->eventStreamSize ? allocator.allocate_array<unsigned char>(def->eventStreamSize) : nullptr;
				new_instance.modelTagRuntime = def->modelTagCount ? allocator.allocate_array<unsigned char>(def->modelTagCount) : nullptr;
				new_instance.flags = copy_array(instance.eventConstantsBuf, def->flagsSize, allocator);
			}

			convert_target_lights(instance, new_instance, allocator);

			std::memcpy(new_instance.origin, instance.origin, sizeof(new_instance.origin));
			std::memcpy(new_instance.angles, instance.angles, sizeof(new_instance.angles));
			std::memcpy(new_instance.startOrigin, instance.startOrigin, sizeof(new_instance.startOrigin));
			std::memcpy(new_instance.startAngles, instance.startAngles, sizeof(new_instance.startAngles));
			new_instance.name = static_cast<zonetool::s2::scr_string_t>(instance.targetname);
			new_instance.updateNextInstance = instance.updateNextInstance;
			new_instance.currentModelIndex = -1;

			new_map_ents.instanceStateSize += instance_state_size(new_instance.def, new_instance);
			if (new_instance.def && new_instance.def->serverInstancePartCount)
			{
				new_map_ents.replicatedInstanceCount++;
			}
		}

		return new_map_ents;
	}

	// s2 resolves the map anim entries with Scr_FindAnim("destructibles", name), so anims stock s2 lacks go into a copy of its anim tree
	void finish()
	{
		if (map_anims.empty())
		{
			return;
		}

		const auto& common = techset::common_rawfiles();
		const auto stock_tree = common.find(std::string(anim_tree_name));
		auto tree = stock_tree != common.end() ? stock_tree->second : std::string{};
		if (tree.empty())
		{
			ZONETOOL_WARNING("%s not found in s2 common_mp, scriptable anims are not added", anim_tree_name.data());
			map_anims.clear();
			return;
		}

		std::unordered_set<std::string> stock;
		for (auto line : utils::string::split(tree, '\n'))
		{
			while (!line.empty() && (line.back() == '\r' || line.back() == ' '))
			{
				line.pop_back();
			}
			stock.insert(utils::string::to_lower(line));
		}

		while (!tree.empty() && (tree.back() == '\n' || tree.back() == '\r' || tree.back() == '\0'))
		{
			tree.pop_back();
		}

		auto added = 0u;
		for (const auto& anim : map_anims)
		{
			if (!stock.contains(utils::string::to_lower(anim)))
			{
				tree += "\n" + anim;
				added++;
			}
		}
		map_anims.clear();

		if (!added)
		{
			return;
		}

		tree += "\n";
		zonetool::s2::RawFile new_asset{anim_tree_override.data(), 0, static_cast<int>(tree.size()), tree.data()};
		zonetool::s2::rawfile::dump(&new_asset);
		add_csv_row("rawfile", anim_tree_override);
	}

	void dump(ScriptableDef* asset)
	{
		utils::memory::allocator allocator;
		zonetool::s2::scriptable_def::dump(convert(asset, allocator));
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_SCRIPTABLE), asset->name);
	}
}
