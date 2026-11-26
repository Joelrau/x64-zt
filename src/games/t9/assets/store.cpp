#include <std_include.hpp>
#include "store.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_strings(auto& reader, char**& field, const std::size_t count)
		{
			if (const auto strings = reader.read_array(field, 7, count))
			{
				for (std::size_t i = 0; i < count; i++)
				{
					reader.read_string(strings[i]);
				}
			}
		}
	}

	void storage_file::read(auto& reader, StorageFile* asset)
	{
		reader.read_string(asset->string);
		read_strings(reader, asset->fileNames, asset->fileCount);
		reader.read_asset_array(ASSET_TYPE_RAWFILE, asset->files, asset->fileCount);
	}

	void storage_file_list::read(auto& reader, StorageFileList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_STORAGEFILE, asset->files, asset->fileCount);
	}

	void store_product::read(auto& reader, StoreProduct* asset)
	{
		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		for (auto& string : asset->strings0)
		{
			reader.read_string(string);
		}

		for (auto& string : asset->strings1)
		{
			reader.read_string(string);
		}

		for (auto& string : asset->strings2)
		{
			reader.read_string(string);
		}
	}

	void store_category::read(auto& reader, StoreCategory* asset)
	{
		reader.read_string(asset->string);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset_array(ASSET_TYPE_STOREPRODUCT, asset->products, asset->productCount);
	}

	void store_category_list::read(auto& reader, StoreCategoryList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_STORECATEGORY, asset->categories, asset->categoryCount);
	}

	void rank::read(auto& reader, Rank* asset)
	{
		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		reader.read_array(asset->levels, 7, asset->levelCount);
	}

	void rank_table::read(auto& reader, RankTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_RANK, asset->ranks, asset->rankCount);
	}

	void prestige::read(auto& reader, Prestige* asset)
	{
		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}
	}

	void prestige_table::read(auto& reader, PrestigeTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_PRESTIGE, asset->prestiges, asset->prestigeCount);
	}

	void first_party_entitlement::read(auto& reader, FirstPartyEntitlement* asset)
	{
		reader.read_string(asset->string);
		read_strings(reader, asset->ids, asset->idCount);
	}

	void first_party_entitlement_list::read(auto& reader, FirstPartyEntitlementList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_FIRSTPARTYENTITLEMENT, asset->entitlements, asset->entitlementCount);
	}

	void entitlement::read(auto& reader, Entitlement* asset)
	{
		reader.read_asset(ASSET_TYPE_FIRSTPARTYENTITLEMENT, asset->firstPartyEntitlement);
		reader.read_string(asset->string);
		reader.read_array(asset->values, 3, asset->valueCount);
	}

	void entitlement_list::read(auto& reader, EntitlementList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_ENTITLEMENT, asset->entitlements, asset->entitlementCount);
	}

	void sku::read(auto&, Sku*)
	{
	}

	void label_store::read(auto& reader, LabelStore* asset)
	{
		reader.read_asset_array(ASSET_TYPE_SKU, asset->skus, asset->skuCount);
	}

	void label_store_list::read(auto& reader, LabelStoreList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_LABELSTORE, asset->stores, asset->storeCount);
	}

	void milestone::read(auto& reader, Milestone* asset)
	{
		reader.read_array(asset->tiers, 7, asset->tierCount);
	}

	void milestone_table::read(auto& reader, MilestoneTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_MILESTONE, asset->milestones, asset->milestoneCount);
	}

	void unlockable_item::read(auto& reader, UnlockableItem* asset)
	{
		reader.read_string(asset->string);
		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		reader.read_array(asset->values, 3, asset->valueCount);
		reader.read_array(asset->tags0, 3, asset->tagCount0);
		reader.read_array(asset->tags1, 3, asset->tagCount1);
		read_script_bundle_object(reader, asset->bundle);
	}

	void unlockable_item_table::read(auto& reader, UnlockableItemTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_UNLOCKABLEITEM, asset->items0, asset->itemCount0);
		reader.read_asset_array(ASSET_TYPE_UNLOCKABLEITEM, asset->items1, asset->itemCount1);
	}

	void game_type_table_entry::read(auto& reader, GameTypeTableEntry* asset)
	{
		for (auto& string : asset->strings0)
		{
			reader.read_string(string);
		}

		reader.read_string(asset->string0);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		for (auto& string : asset->strings1)
		{
			reader.read_string(string);
		}

		read_script_bundle_object(reader, asset->bundle);
		reader.read_array(asset->data, 0, asset->dataSize);
		reader.read_string(asset->string1);
		reader.read_array(asset->values, 7, asset->valueCount);
	}

	void game_type_table::read(auto& reader, GameTypeTable* asset)
	{
		reader.read_asset_array(ASSET_TYPE_GAMETYPETABLEENTRY, asset->entries, asset->entryCount);
		reader.read_array(asset->values, 3, asset->entryCount);
	}
}

namespace zonetool::t9
{
	template void storage_file::read(zone_reader&, StorageFile*);
	template void storage_file_list::read(zone_reader&, StorageFileList*);
	template void store_product::read(zone_reader&, StoreProduct*);
	template void store_category::read(zone_reader&, StoreCategory*);
	template void store_category_list::read(zone_reader&, StoreCategoryList*);
	template void rank::read(zone_reader&, Rank*);
	template void rank_table::read(zone_reader&, RankTable*);
	template void prestige::read(zone_reader&, Prestige*);
	template void prestige_table::read(zone_reader&, PrestigeTable*);
	template void first_party_entitlement::read(zone_reader&, FirstPartyEntitlement*);
	template void first_party_entitlement_list::read(zone_reader&, FirstPartyEntitlementList*);
	template void entitlement::read(zone_reader&, Entitlement*);
	template void entitlement_list::read(zone_reader&, EntitlementList*);
	template void sku::read(zone_reader&, Sku*);
	template void label_store::read(zone_reader&, LabelStore*);
	template void label_store_list::read(zone_reader&, LabelStoreList*);
	template void milestone::read(zone_reader&, Milestone*);
	template void milestone_table::read(zone_reader&, MilestoneTable*);
	template void unlockable_item::read(zone_reader&, UnlockableItem*);
	template void unlockable_item_table::read(zone_reader&, UnlockableItemTable*);
	template void game_type_table_entry::read(zone_reader&, GameTypeTableEntry*);
	template void game_type_table::read(zone_reader&, GameTypeTable*);
	template void storage_file::read(zone_writer&, StorageFile*);
	template void storage_file_list::read(zone_writer&, StorageFileList*);
	template void store_product::read(zone_writer&, StoreProduct*);
	template void store_category::read(zone_writer&, StoreCategory*);
	template void store_category_list::read(zone_writer&, StoreCategoryList*);
	template void rank::read(zone_writer&, Rank*);
	template void rank_table::read(zone_writer&, RankTable*);
	template void prestige::read(zone_writer&, Prestige*);
	template void prestige_table::read(zone_writer&, PrestigeTable*);
	template void first_party_entitlement::read(zone_writer&, FirstPartyEntitlement*);
	template void first_party_entitlement_list::read(zone_writer&, FirstPartyEntitlementList*);
	template void entitlement::read(zone_writer&, Entitlement*);
	template void entitlement_list::read(zone_writer&, EntitlementList*);
	template void sku::read(zone_writer&, Sku*);
	template void label_store::read(zone_writer&, LabelStore*);
	template void label_store_list::read(zone_writer&, LabelStoreList*);
	template void milestone::read(zone_writer&, Milestone*);
	template void milestone_table::read(zone_writer&, MilestoneTable*);
	template void unlockable_item::read(zone_writer&, UnlockableItem*);
	template void unlockable_item_table::read(zone_writer&, UnlockableItemTable*);
	template void game_type_table_entry::read(zone_writer&, GameTypeTableEntry*);
	template void game_type_table::read(zone_writer&, GameTypeTable*);
}
