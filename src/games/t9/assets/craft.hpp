#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class craft_icon
	{
	public:
		static void read(auto& reader, CraftIcon* asset);
	};

	class craft_icon_list
	{
	public:
		static void read(auto& reader, CraftIconList* asset);
	};

	class craft_weapon_sticker
	{
	public:
		static void read(auto& reader, CraftWeaponSticker* asset);
	};

	class craft_weapon_sticker_list
	{
	public:
		static void read(auto& reader, CraftWeaponStickerList* asset);
	};

	class craft_background
	{
	public:
		static void read(auto& reader, CraftBackground* asset);
	};

	class craft_background_list
	{
	public:
		static void read(auto& reader, CraftBackgroundList* asset);
	};

	class craft_material
	{
	public:
		static void read(auto& reader, CraftMaterial* asset);
	};

	class craft_material_list
	{
	public:
		static void read(auto& reader, CraftMaterialList* asset);
	};

	class craft_category
	{
	public:
		static void read(auto& reader, CraftCategory* asset);
	};

	class craft_category_list
	{
	public:
		static void read(auto& reader, CraftCategoryList* asset);
	};

	class craft_weapon_icon_transform
	{
	public:
		static void read(auto& reader, CraftWeaponIconTransform* asset);
	};

	class craft_weapon_icon_transform_list
	{
	public:
		static void read(auto& reader, CraftWeaponIconTransformList* asset);
	};
}
