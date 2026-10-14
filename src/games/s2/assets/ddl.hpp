#pragma once

namespace zonetool::s2
{
	class ddl
	{
	public:
		static void read_hash_table(zone_reader& reader, DDLHashTable& table)
		{
			reader.read_array(table.list, 3, table.count);
		}

		static void read_def(zone_reader& reader, DDLDef* def)
		{
			reader.read_string(def->name);

			if (const auto structs = reader.read_array(def->structList, 7, def->structCount))
			{
				for (auto i = 0; i < def->structCount; i++)
				{
					auto& ddl_struct = structs[i];
					reader.read_string(ddl_struct.name);
					if (const auto members = reader.read_array(ddl_struct.members, 7, ddl_struct.memberCount))
					{
						for (auto j = 0; j < ddl_struct.memberCount; j++)
						{
							reader.read_string(members[j].name);
							reader.read_stream(members[j].parent, 7, 1);
						}
					}
					read_hash_table(reader, ddl_struct.hashTableUpper);
					read_hash_table(reader, ddl_struct.hashTableLower);
				}
			}

			if (const auto enums = reader.read_array(def->enumList, 7, def->enumCount))
			{
				for (auto i = 0; i < def->enumCount; i++)
				{
					reader.read_string(enums[i].name);
					if (const auto members = reader.read_array(enums[i].members, 7, enums[i].memberCount))
					{
						for (auto j = 0; j < enums[i].memberCount; j++)
						{
							reader.read_string(members[j]);
						}
					}
					read_hash_table(reader, enums[i].hashTable);
				}
			}

			if (const auto next = reader.read_single(def->next, 7))
			{
				read_def(reader, next);
			}
		}

		static void read(zone_reader& reader, DDLRoot* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto def = reader.read_single(asset->ddlDef, 7))
			{
				read_def(reader, def);
			}
			reader.pop_stream();
		}
	};
}
