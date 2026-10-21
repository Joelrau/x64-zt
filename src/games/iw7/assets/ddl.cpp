#include <std_include.hpp>
#include "ddl.hpp"

#include <formats/ddl.hpp>

namespace zonetool::iw7
{
	DDLFile* ddl::parse(const std::string& name, zone_memory* mem)
	{
		return zonetool::ddl::parse(name, mem);
	}

	void ddl::dump(DDLFile* asset)
	{
		zonetool::ddl::dump(asset);
	}

	void ddl::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).ddlFile;
		}
	}

	void ddl::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void ddl::load_depending(zone_base* zone)
	{
	}

	std::string ddl::name()
	{
		return this->name_;
	}

	std::int32_t ddl::type()
	{
		return ASSET_TYPE_DDL;
	}

	DDLDef* write_ddldef(zone_buffer* buf, DDLDef* data)
	{
		auto dest = buf->write(data);

		if (data->name)
		{
			dest->name = buf->write_str(data->name);
		}

		if (data->structList)
		{
			buf->align(7);

			auto* dest_struct_list = buf->write(data->structList, data->structCount);
			auto* data_struct_list = data->structList;
			for (auto struct_index = 0; struct_index < data->structCount; struct_index++)
			{
				if (data_struct_list[struct_index].name)
				{
					dest_struct_list[struct_index].name = buf->write_str(data_struct_list[struct_index].name);
				}

				if (data_struct_list[struct_index].members)
				{
					buf->align(7);

					auto* dest_members = buf->write(data_struct_list[struct_index].members, data_struct_list[struct_index].memberCount);
					auto* data_members = data_struct_list[struct_index].members;
					for (auto member_index = 0; member_index < data_struct_list[struct_index].memberCount; member_index++)
					{
						if (data_members[member_index].name)
						{
							dest_members[member_index].name = buf->write_str(data_members[member_index].name);
						}

						if (data_members[member_index].parent)
						{
							dest_members[member_index].parent = nullptr; // ??
						}
					}

					buf->clear_pointer(&dest_struct_list[struct_index].members);
				}

				if (data_struct_list[struct_index].hashTableUpper.list)
				{
					buf->align(3);
					buf->write(data_struct_list[struct_index].hashTableUpper.list, data_struct_list[struct_index].hashTableUpper.count);
					buf->clear_pointer(&dest_struct_list[struct_index].hashTableUpper.list);
				}

				if (data_struct_list[struct_index].hashTableLower.list)
				{
					buf->align(3);
					buf->write(data_struct_list[struct_index].hashTableLower.list, data_struct_list[struct_index].hashTableLower.count);
					buf->clear_pointer(&dest_struct_list[struct_index].hashTableLower.list);
				}
			}

			buf->clear_pointer(&dest->structList);
		}

		if (data->enumList)
		{
			buf->align(7);

			auto* dest_enum_list = buf->write(data->enumList, data->enumCount);
			auto* data_enum_list = data->enumList;
			for (auto enum_index = 0; enum_index < data->enumCount; enum_index++)
			{
				if (data_enum_list[enum_index].name)
				{
					dest_enum_list[enum_index].name = buf->write_str(data_enum_list[enum_index].name);
				}

				if (data_enum_list[enum_index].members)
				{
					buf->align(7);
					auto* strings = buf->write(data_enum_list[enum_index].members, data_enum_list[enum_index].memberCount);
					for (auto member_index = 0; member_index < data_enum_list[enum_index].memberCount; member_index++)
					{
						if (data_enum_list[enum_index].members[member_index])
						{
							strings[member_index] = buf->write_str(data_enum_list[enum_index].members[member_index]);
						}
					}
					buf->clear_pointer(&dest_enum_list[enum_index].members);
				}

				if (data_enum_list[enum_index].hashTable.list)
				{
					buf->align(3);
					buf->write(data_enum_list[enum_index].hashTable.list, data_enum_list[enum_index].hashTable.count);
					buf->clear_pointer(&dest_enum_list[enum_index].hashTable.list);
				}
			}

			buf->clear_pointer(&dest->enumList);
		}

		if (data->next)
		{
			buf->align(7);
			write_ddldef(buf, data->next);
			buf->clear_pointer(&dest->next);
		}

		return dest;
	}

	namespace
	{
		void read_ddl_def(zone_reader& reader, DDLDef* def)
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
							reader.read_array(reinterpret_cast<std::uint8_t*&>(members[j].parent), 0, 1);
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
				read_ddl_def(reader, next);
			}
		}
	}

	void ddl::read(zone_reader& reader, DDLFile* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto def = reader.read_single(asset->ddlDef, 7))
		{
			read_ddl_def(reader, def);
		}
		reader.pop_stream();
	}

	void ddl::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->ddlDef)
		{
			buf->align(7);
			write_ddldef(buf, data->ddlDef);
			buf->clear_pointer(&dest->ddlDef);
		}

		buf->pop_stream();
	}
}
