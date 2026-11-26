#include <std_include.hpp>
#include "ddl.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_def(auto& reader, DDLDef*& field)
		{
			for (auto* next = &field; *next; next = &(*next)->next)
			{
				const auto def = reader.read_single(*next, 7);

				if (const auto structs = reader.read_array(def->structs, 7, def->structCount))
				{
					for (auto i = 0; i < def->structCount; i++)
					{
						reader.read_array(structs[i].members, 7, structs[i].memberCount);
						reader.read_array(structs[i].tableA, 7, structs[i].tableACount);
						reader.read_array(structs[i].tableB, 7, structs[i].tableBCount);
					}
				}

				if (const auto enums = reader.read_array(def->enums, 7, def->enumCount))
				{
					for (auto i = 0; i < def->enumCount; i++)
					{
						reader.read_array(enums[i].members, 7, enums[i].memberCount);
						reader.read_array(enums[i].values, 7, enums[i].valueCount);
					}
				}
			}
		}
	}

	void ddl::read(auto& reader, DDLFile* asset)
	{
		read_def(reader, asset->ddlDef);
	}
}

namespace zonetool::t9
{
	template void ddl::read(zone_reader&, DDLFile*);
	template void ddl::read(zone_writer&, DDLFile*);
}
