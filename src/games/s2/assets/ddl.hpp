#pragma once

namespace zonetool::s2
{
	struct ddl_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(DDLRoot, name));
			w.entries(offsetof(DDLRoot, ddlDef), sizeof(DDLDef), 1, [](const auto& def, std::size_t)
			{
				ddl_layout::visit_def(def);
			});
		}

	private:
		template <typename V>
		static void visit_hash_table(const V& table)
		{
			table.bytes(offsetof(DDLHashTable, list), 3, sizeof(*DDLHashTable{}.list) * table.template at<int>(offsetof(DDLHashTable, count)));
		}

		template <typename V>
		static void visit_def(const V& def)
		{
			def.string(offsetof(DDLDef, name));
			def.entries(offsetof(DDLDef, structList), sizeof(DDLStruct), def.template at<int>(offsetof(DDLDef, structCount)), [](const auto& ddl_struct, std::size_t)
			{
				ddl_struct.string(offsetof(DDLStruct, name));
				ddl_struct.entries(offsetof(DDLStruct, members), sizeof(DDLMember),
					ddl_struct.template at<int>(offsetof(DDLStruct, memberCount)), [](const auto& member, std::size_t)
				{
					member.string(offsetof(DDLMember, name));
					member.bytes(offsetof(DDLMember, parent), 7, 1);
				});
				ddl_layout::visit_hash_table(ddl_struct.sub(offsetof(DDLStruct, hashTableUpper)));
				ddl_layout::visit_hash_table(ddl_struct.sub(offsetof(DDLStruct, hashTableLower)));
			});

			def.entries(offsetof(DDLDef, enumList), sizeof(DDLEnum), def.template at<int>(offsetof(DDLDef, enumCount)), [](const auto& ddl_enum, std::size_t)
			{
				ddl_enum.string(offsetof(DDLEnum, name));
				ddl_enum.entries(offsetof(DDLEnum, members), sizeof(const char*),
					ddl_enum.template at<int>(offsetof(DDLEnum, memberCount)), [](const auto& member, std::size_t)
				{
					member.string(0);
				});
				ddl_layout::visit_hash_table(ddl_enum.sub(offsetof(DDLEnum, hashTable)));
			});

			def.entries(offsetof(DDLDef, next), sizeof(DDLDef), 1, [](const auto& next, std::size_t)
			{
				ddl_layout::visit_def(next);
			});
		}
	};

	class ddl : public field_layout_asset<DDLRoot, ASSET_TYPE_DDL, ddl_layout>
	{
	};
}
