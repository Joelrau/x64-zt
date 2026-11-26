#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class storage_file
	{
	public:
		static void read(auto& reader, StorageFile* asset);
	};

	class storage_file_list
	{
	public:
		static void read(auto& reader, StorageFileList* asset);
	};

	class store_product
	{
	public:
		static void read(auto& reader, StoreProduct* asset);
	};

	class store_category
	{
	public:
		static void read(auto& reader, StoreCategory* asset);
	};

	class store_category_list
	{
	public:
		static void read(auto& reader, StoreCategoryList* asset);
	};

	class rank
	{
	public:
		static void read(auto& reader, Rank* asset);
	};

	class rank_table
	{
	public:
		static void read(auto& reader, RankTable* asset);
	};

	class prestige
	{
	public:
		static void read(auto& reader, Prestige* asset);
	};

	class prestige_table
	{
	public:
		static void read(auto& reader, PrestigeTable* asset);
	};

	class first_party_entitlement
	{
	public:
		static void read(auto& reader, FirstPartyEntitlement* asset);
	};

	class first_party_entitlement_list
	{
	public:
		static void read(auto& reader, FirstPartyEntitlementList* asset);
	};

	class entitlement
	{
	public:
		static void read(auto& reader, Entitlement* asset);
	};

	class entitlement_list
	{
	public:
		static void read(auto& reader, EntitlementList* asset);
	};

	class sku
	{
	public:
		static void read(auto& reader, Sku* asset);
	};

	class label_store
	{
	public:
		static void read(auto& reader, LabelStore* asset);
	};

	class label_store_list
	{
	public:
		static void read(auto& reader, LabelStoreList* asset);
	};

	class milestone
	{
	public:
		static void read(auto& reader, Milestone* asset);
	};

	class milestone_table
	{
	public:
		static void read(auto& reader, MilestoneTable* asset);
	};

	class unlockable_item
	{
	public:
		static void read(auto& reader, UnlockableItem* asset);
	};

	class unlockable_item_table
	{
	public:
		static void read(auto& reader, UnlockableItemTable* asset);
	};

	class game_type_table_entry
	{
	public:
		static void read(auto& reader, GameTypeTableEntry* asset);
	};

	class game_type_table
	{
	public:
		static void read(auto& reader, GameTypeTable* asset);
	};
}
