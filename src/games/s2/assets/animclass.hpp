#pragma once

namespace zonetool::s2
{
	struct anim_class_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(AnimationClass, name));
			w.entries(offsetof(AnimationClass, stateMachine), sizeof(AnimationStateMachine), 1, [](const auto& machine, std::size_t)
			{
				anim_class_layout::visit_state_machine(machine);
			});
			w.script_string(offsetof(AnimationClass, animTree));
			w.asset(ASSET_TYPE_SCRIPTABLE, offsetof(AnimationClass, scriptable));

			const auto sound_count = w.template at<unsigned short>(offsetof(AnimationClass, soundCount));
			const auto effect_count = w.template at<unsigned short>(offsetof(AnimationClass, effectCount));
			w.script_strings(offsetof(AnimationClass, soundNotes), sound_count);
			w.script_strings(offsetof(AnimationClass, soundNames), sound_count);
			w.script_strings(offsetof(AnimationClass, soundOptions), sound_count);
			w.script_strings(offsetof(AnimationClass, effectNotes), effect_count);
			w.asset_array(ASSET_TYPE_FX, offsetof(AnimationClass, effects), effect_count);
			w.script_strings(offsetof(AnimationClass, effectTags), effect_count);
		}

	private:
		template <typename V>
		static void visit_aim_set(const V& aim_set)
		{
			aim_set.script_string(offsetof(AnimationAimSet, name));
			aim_set.script_string(offsetof(AnimationAimSet, rootName));
			for (auto i = 0u; i < std::size(AnimationAimSet{}.animName); i++)
			{
				aim_set.script_string(offsetof(AnimationAimSet, animName) + i * sizeof(scr_string_t));
			}
		}

		template <typename V>
		static void visit_state(const V& state)
		{
			state.script_string(offsetof(AnimationState, name));
			state.script_string(offsetof(AnimationState, notify));
			state.entries(offsetof(AnimationState, aimSet), sizeof(AnimationAimSet), 1, [](const auto& aim_set, std::size_t)
			{
				anim_class_layout::visit_aim_set(aim_set);
			});

			const auto entry_count = state.template at<unsigned char>(offsetof(AnimationState, entryCount));
			state.aligned_entries(offsetof(AnimationState, animEntries), 3, sizeof(AnimationEntry), entry_count, [](const auto& entry, std::size_t)
			{
				entry.script_string(offsetof(AnimationEntry, animName));
				entry.script_string(offsetof(AnimationEntry, alias));
			});
			state.runtime_bytes(offsetof(AnimationState, runtime), 7, sizeof(void*) * entry_count);
		}

		template <typename V>
		static void visit_state_machine(const V& machine)
		{
			machine.script_string(offsetof(AnimationStateMachine, name));
			machine.entries(offsetof(AnimationStateMachine, states), sizeof(AnimationState),
				machine.template at<unsigned short>(offsetof(AnimationStateMachine, stateCount)), [](const auto& state, std::size_t)
			{
				anim_class_layout::visit_state(state);
			});
			machine.entries(offsetof(AnimationStateMachine, aimSets), sizeof(AnimationAimSet),
				machine.template at<unsigned short>(offsetof(AnimationStateMachine, aimSetCount)), [](const auto& aim_set, std::size_t)
			{
				anim_class_layout::visit_aim_set(aim_set);
			});
		}
	};

	class anim_class : public field_layout_asset<AnimationClass, ASSET_TYPE_ANIMCLASS, anim_class_layout>
	{
	};
}
