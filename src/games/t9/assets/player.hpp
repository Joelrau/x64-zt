#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class player_sounds_table
	{
	public:
		static void read(auto& reader, PlayerSoundsTable* asset);
	};

	class player_fx_table
	{
	public:
		static void read(auto& reader, PlayerFxTable* asset);
	};

	class player_role_category
	{
	public:
		static void read(auto& reader, PlayerRoleCategory* asset);
	};

	class player_role_category_table
	{
	public:
		static void read(auto& reader, PlayerRoleCategoryTable* asset);
	};

	class player_outfit
	{
	public:
		static void read(auto& reader, PlayerOutfit* asset);
	};

	class character_body_type
	{
	public:
		static void read(auto& reader, CharacterBodyType* asset);
	};

	class player_movement_tunables
	{
	public:
		static void read(auto& reader, PlayerMovementTunables* asset);
	};

	class talent
	{
	public:
		static void read(auto& reader, Talent* asset);
	};

	class player_talent_template
	{
	public:
		static void read(auto& reader, PlayerTalentTemplate* asset);
	};

	class player_role_template
	{
	public:
		static void read(auto& reader, PlayerRoleTemplate* asset);
	};

	class customization_table
	{
	public:
		static void read(auto& reader, CustomizationTable* asset);
	};

	class customization_table_frontend
	{
	public:
		static void read(auto& reader, CustomizationTableFrontend* asset);
	};

	class player_role_template_frontend
	{
	public:
		static void read(auto& reader, PlayerRoleTemplateFrontend* asset);
	};

	class character_body_type_frontend
	{
	public:
		static void read(auto& reader, CharacterBodyTypeFrontend* asset);
	};

	class char_mixer
	{
	public:
		static void read(auto& reader, CharMixer* asset);
	};

	class execution
	{
	public:
		static void read(auto& reader, Execution* asset);
	};

	class player_animation
	{
	public:
		static void read(auto& reader, PlayerAnimation* asset);
	};

	class player_settings
	{
	public:
		static void read(auto& reader, PlayerSettings* asset);
	};

	class motion_matching_input
	{
	public:
		static void read(auto& reader, MotionMatchingInput* asset);
	};

	class spray_or_gesture_item
	{
	public:
		static void read(auto& reader, SprayOrGestureItem* asset);
	};

	class spray_or_gesture_list
	{
	public:
		static void read(auto& reader, SprayOrGestureList* asset);
	};

	class ui_model_data_struct
	{
	public:
		static void read(auto& reader, UIModelDataStruct* asset);
	};
}
