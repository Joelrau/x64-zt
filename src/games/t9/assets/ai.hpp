#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class aim_table
	{
	public:
		static void read(auto& reader, AimTable* asset);
	};

	class shoot_table
	{
	public:
		static void read(auto& reader, ShootTable* asset);
	};

	class anim_selector_table
	{
	public:
		static void read(auto& reader, AnimSelectorTable* asset);
	};

	class anim_mapping_table
	{
	public:
		static void read(auto& reader, AnimMappingTable* asset);
	};

	class anim_state_machine
	{
	public:
		static void read(auto& reader, AnimStateMachine* asset);
	};

	class behavior_tree
	{
	public:
		static void read(auto& reader, BehaviorTree* asset);
	};

	class behavior_state_machine
	{
	public:
		static void read(auto& reader, BehaviorStateMachine* asset);
	};

	class xmodel_alias
	{
	public:
		static void read(auto& reader, XModelAlias* asset);
	};

	class character
	{
	public:
		static void read(auto& reader, Character* asset);
	};

	class ai_type
	{
	public:
		static void read(auto& reader, AIType* asset);
	};

	class tactical_query
	{
	public:
		static void read(auto& reader, TacticalQuery* asset);
	};
}
