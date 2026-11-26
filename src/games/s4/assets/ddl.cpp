#include <std_include.hpp>
#include "ddl.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_hash_table(zone_reader& reader, DDLHashTable& table)
		{
			reader.read_array(table.list, 3, table.count);
		}

		void read_def(zone_reader& reader, DDLDef* def)
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
					auto& ddl_enum = enums[i];
					reader.read_string(ddl_enum.name);
					if (const auto members = reader.read_array(ddl_enum.members, 7, ddl_enum.memberCount))
					{
						for (auto j = 0; j < ddl_enum.memberCount; j++)
						{
							reader.read_string(members[j]);
						}
					}
					read_hash_table(reader, ddl_enum.hashTable);
				}
			}

			if (const auto next = reader.read_single(def->next, 7))
			{
				read_def(reader, next);
			}
		}
	}

	void ddl_file::read(zone_reader& reader, DDLFile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto def = reader.read_single(asset->ddlDef, 7))
		{
			read_def(reader, def);
		}
		reader.pop_stream();
	}
}
