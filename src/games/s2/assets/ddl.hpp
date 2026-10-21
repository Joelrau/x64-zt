#pragma once

namespace zonetool::s2
{
	class ddl : public asset_base<DDLRoot, ASSET_TYPE_DDL>
	{
	public:
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

		DDLRoot* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_DDL, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing ddl \"%s\"...", name.data());

			auto* asset = read.read_single<DDLRoot>();
			asset->name = read.read_string();
			if (const auto def = parse_array(read, mem, asset->ddlDef, 1))
			{
				parse_def(read, mem, def);
			}
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_def = write_array(buf, 7, data->ddlDef, 1, dest->ddlDef))
			{
				write_def(buf, data->ddlDef, dest_def);
			}

			buf->pop_stream();
		}

		static void dump(DDLRoot* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_DDL, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_single(asset->ddlDef);
			if (asset->ddlDef)
			{
				dump_def(write, asset->ddlDef);
			}
			write.close();
		}

	private:
		static void read_def(zone_reader& reader, DDLDef* def)
		{
			reader.read_string(def->name);
			if (const auto structs = reader.read_array(def->structList, 7, def->structCount))
			{
				for (auto i = 0; i < def->structCount; i++)
				{
					reader.read_string(structs[i].name);
					if (const auto members = reader.read_array(structs[i].members, 7, structs[i].memberCount))
					{
						for (auto j = 0; j < structs[i].memberCount; j++)
						{
							reader.read_string(members[j].name);
							reader.read_array(members[j].parent, 7, 1);
						}
					}
					reader.read_array(structs[i].hashTableUpper.list, 3, structs[i].hashTableUpper.count);
					reader.read_array(structs[i].hashTableLower.list, 3, structs[i].hashTableLower.count);
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
					reader.read_array(enums[i].hashTable.list, 3, enums[i].hashTable.count);
				}
			}

			if (const auto next = reader.read_single(def->next, 7))
			{
				read_def(reader, next);
			}
		}

		static void parse_def(assetmanager::reader& read, zone_memory* mem, DDLDef* def)
		{
			def->name = read.read_string();
			if (const auto structs = parse_array(read, mem, def->structList, def->structCount))
			{
				for (auto i = 0; i < def->structCount; i++)
				{
					structs[i].name = read.read_string();
					if (const auto members = parse_array(read, mem, structs[i].members, structs[i].memberCount))
					{
						for (auto j = 0; j < structs[i].memberCount; j++)
						{
							members[j].name = read.read_string();
							parse_array(read, mem, members[j].parent, 1);
						}
					}
					parse_array(read, mem, structs[i].hashTableUpper.list, structs[i].hashTableUpper.count);
					parse_array(read, mem, structs[i].hashTableLower.list, structs[i].hashTableLower.count);
				}
			}

			if (const auto enums = parse_array(read, mem, def->enumList, def->enumCount))
			{
				for (auto i = 0; i < def->enumCount; i++)
				{
					enums[i].name = read.read_string();
					if (const auto members = parse_array(read, mem, enums[i].members, enums[i].memberCount))
					{
						for (auto j = 0; j < enums[i].memberCount; j++)
						{
							members[j] = read.read_string();
						}
					}
					parse_array(read, mem, enums[i].hashTable.list, enums[i].hashTable.count);
				}
			}

			if (const auto next = parse_array(read, mem, def->next, 1))
			{
				parse_def(read, mem, next);
			}
		}

		static void write_def(zone_buffer* buf, DDLDef* def, DDLDef* dest)
		{
			if (def->name)
			{
				dest->name = buf->write_str(def->name);
			}

			if (const auto dest_structs = write_array(buf, 7, def->structList, def->structCount, dest->structList))
			{
				for (auto i = 0; i < def->structCount; i++)
				{
					auto* ddl_struct = &def->structList[i];
					write_string(buf, ddl_struct->name, dest_structs[i].name);
					if (const auto dest_members = write_array(buf, 7, ddl_struct->members, ddl_struct->memberCount, dest_structs[i].members))
					{
						for (auto j = 0; j < ddl_struct->memberCount; j++)
						{
							write_string(buf, ddl_struct->members[j].name, dest_members[j].name);
							write_array(buf, 7, ddl_struct->members[j].parent, 1, dest_members[j].parent);
						}
					}
					write_array(buf, 3, ddl_struct->hashTableUpper.list, ddl_struct->hashTableUpper.count, dest_structs[i].hashTableUpper.list);
					write_array(buf, 3, ddl_struct->hashTableLower.list, ddl_struct->hashTableLower.count, dest_structs[i].hashTableLower.list);
				}
			}

			if (const auto dest_enums = write_array(buf, 7, def->enumList, def->enumCount, dest->enumList))
			{
				for (auto i = 0; i < def->enumCount; i++)
				{
					auto* ddl_enum = &def->enumList[i];
					write_string(buf, ddl_enum->name, dest_enums[i].name);
					if (const auto dest_members = write_array(buf, 7, ddl_enum->members, ddl_enum->memberCount, dest_enums[i].members))
					{
						for (auto j = 0; j < ddl_enum->memberCount; j++)
						{
							write_string(buf, ddl_enum->members[j], dest_members[j]);
						}
					}
					write_array(buf, 3, ddl_enum->hashTable.list, ddl_enum->hashTable.count, dest_enums[i].hashTable.list);
				}
			}

			if (const auto dest_next = write_array(buf, 7, def->next, 1, dest->next))
			{
				write_def(buf, def->next, dest_next);
			}
		}

		static void dump_def(assetmanager::dumper& write, DDLDef* def)
		{
			write.dump_string(def->name);
			write.dump_array(def->structList, def->structCount);
			for (auto i = 0; def->structList && i < def->structCount; i++)
			{
				auto* ddl_struct = &def->structList[i];
				write.dump_string(ddl_struct->name);
				write.dump_array(ddl_struct->members, ddl_struct->memberCount);
				for (auto j = 0; ddl_struct->members && j < ddl_struct->memberCount; j++)
				{
					write.dump_string(ddl_struct->members[j].name);
					write.dump_single(ddl_struct->members[j].parent);
				}
				write.dump_array(ddl_struct->hashTableUpper.list, ddl_struct->hashTableUpper.count);
				write.dump_array(ddl_struct->hashTableLower.list, ddl_struct->hashTableLower.count);
			}

			write.dump_array(def->enumList, def->enumCount);
			for (auto i = 0; def->enumList && i < def->enumCount; i++)
			{
				auto* ddl_enum = &def->enumList[i];
				write.dump_string(ddl_enum->name);
				write.dump_array(ddl_enum->members, ddl_enum->memberCount);
				for (auto j = 0; ddl_enum->members && j < ddl_enum->memberCount; j++)
				{
					write.dump_string(ddl_enum->members[j]);
				}
				write.dump_array(ddl_enum->hashTable.list, ddl_enum->hashTable.count);
			}

			write.dump_single(def->next);
			if (def->next)
			{
				dump_def(write, def->next);
			}
		}
	};
}
