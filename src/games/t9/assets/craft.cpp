#include <std_include.hpp>
#include "craft.hpp"

namespace zonetool::t9
{
	void craft_icon::read(auto& reader, CraftIcon* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
	}

	void craft_icon_list::read(auto& reader, CraftIconList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_CRAFTICON, asset->icons, asset->iconCount);
	}

	void craft_weapon_sticker::read(auto& reader, CraftWeaponSticker* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
	}

	void craft_weapon_sticker_list::read(auto& reader, CraftWeaponStickerList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_CRAFTWEAPONSTICKER, asset->stickers, asset->stickerCount);
	}

	void craft_background::read(auto& reader, CraftBackground* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image0);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image1);
	}

	void craft_background_list::read(auto& reader, CraftBackgroundList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_CRAFTBACKGROUND, asset->backgrounds, asset->backgroundCount);
	}

	void craft_material::read(auto& reader, CraftMaterial* asset)
	{
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image0);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image1);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->material);
	}

	void craft_material_list::read(auto& reader, CraftMaterialList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_CRAFTMATERIAL, asset->materials, asset->materialCount);
	}

	void craft_category::read(auto&, CraftCategory*)
	{
	}

	void craft_category_list::read(auto& reader, CraftCategoryList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_CRAFTCATEGORY, asset->categories0, asset->categoryCount0);
		reader.read_asset_array(ASSET_TYPE_CRAFTCATEGORY, asset->categories1, asset->categoryCount1);
		reader.read_asset_array(ASSET_TYPE_CRAFTCATEGORY, asset->categories2, asset->categoryCount2);
	}

	void craft_weapon_icon_transform::read(auto&, CraftWeaponIconTransform*)
	{
	}

	void craft_weapon_icon_transform_list::read(auto& reader, CraftWeaponIconTransformList* asset)
	{
		for (auto& transform : asset->transforms)
		{
			reader.read_asset(ASSET_TYPE_CRAFTWEAPONICONTRANSFORM, transform);
		}
	}
}

namespace zonetool::t9
{
	template void craft_icon::read(zone_reader&, CraftIcon*);
	template void craft_icon_list::read(zone_reader&, CraftIconList*);
	template void craft_weapon_sticker::read(zone_reader&, CraftWeaponSticker*);
	template void craft_weapon_sticker_list::read(zone_reader&, CraftWeaponStickerList*);
	template void craft_background::read(zone_reader&, CraftBackground*);
	template void craft_background_list::read(zone_reader&, CraftBackgroundList*);
	template void craft_material::read(zone_reader&, CraftMaterial*);
	template void craft_material_list::read(zone_reader&, CraftMaterialList*);
	template void craft_category::read(zone_reader&, CraftCategory*);
	template void craft_category_list::read(zone_reader&, CraftCategoryList*);
	template void craft_weapon_icon_transform::read(zone_reader&, CraftWeaponIconTransform*);
	template void craft_weapon_icon_transform_list::read(zone_reader&, CraftWeaponIconTransformList*);
	template void craft_icon::read(zone_writer&, CraftIcon*);
	template void craft_icon_list::read(zone_writer&, CraftIconList*);
	template void craft_weapon_sticker::read(zone_writer&, CraftWeaponSticker*);
	template void craft_weapon_sticker_list::read(zone_writer&, CraftWeaponStickerList*);
	template void craft_background::read(zone_writer&, CraftBackground*);
	template void craft_background_list::read(zone_writer&, CraftBackgroundList*);
	template void craft_material::read(zone_writer&, CraftMaterial*);
	template void craft_material_list::read(zone_writer&, CraftMaterialList*);
	template void craft_category::read(zone_writer&, CraftCategory*);
	template void craft_category_list::read(zone_writer&, CraftCategoryList*);
	template void craft_weapon_icon_transform::read(zone_writer&, CraftWeaponIconTransform*);
	template void craft_weapon_icon_transform_list::read(zone_writer&, CraftWeaponIconTransformList*);
}
