#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class gesture
	{
	public:
		static void read(auto& reader, Gesture* asset);
	};

	class gesture_table
	{
	public:
		static void read(auto& reader, GestureTable* asset);
	};

	class weapon_style_settings
	{
	public:
		static void read(auto& reader, WeaponStyleSettings* asset);
	};

	class weapon_tunables
	{
	public:
		static void read(auto& reader, WeaponTunables* asset);
	};

	class weapon_secondary_movement
	{
	public:
		static void read(auto& reader, WeaponSecondaryMovement* asset);
	};

	class cinematic_motion
	{
	public:
		static void read(auto& reader, CinematicMotion* asset);
	};

	class weapon_camo
	{
	public:
		static void read(auto& reader, WeaponCamo* asset);
	};

	class weapon_camo_binding
	{
	public:
		static void read(auto& reader, WeaponCamoBinding* asset);
	};

	class shared_weapon_sounds
	{
	public:
		static void read(auto& reader, SharedWeaponSounds* asset);
	};

	class weapon_blueprint
	{
	public:
		static void read(auto& reader, WeaponBlueprint* asset);
	};

	class attachment
	{
	public:
		static void read(auto& reader, Attachment* asset);
	};

	class attachment_unique
	{
	public:
		static void read(auto& reader, AttachmentUnique* asset);
	};

	class weapon_frontend
	{
	public:
		static void read(auto& reader, WeaponFrontend* asset);
	};

	class weapon
	{
	public:
		static void read(auto& reader, Weapon* asset);
	};
}
