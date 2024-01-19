#include <std_include.hpp>
#include "ddl.hpp"

namespace zonetool::h1
{
	namespace
	{
		int32_t Com_HashString(const char* str, int maxLength = 0)
		{
			int32_t hash = 0;

			if (str != nullptr)
			{
				char currentChar = *str;

				while (currentChar != '\0' && (maxLength == 0 || hash < maxLength))
				{
					hash = 31 * hash + currentChar;
					currentChar = *(++str);
				}
			}

			return hash;
		}

		int compare_hash(const void* a, const void* b)
		{
			const auto hash_a = reinterpret_cast<DDLHash*>(
				reinterpret_cast<size_t>(a))->hash;
			const auto hash_b = reinterpret_cast<DDLHash*>(
				reinterpret_cast<size_t>(b))->hash;

			if (hash_a < hash_b)
			{
				return -1;
			}
			else if (hash_a > hash_b)
			{
				return 1;
			}

			return 0;
		}
	}

	DDLRoot* ddl::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file("ddl\\"s + name + ".json"s);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing ddl \"%s\"...", name.data());

			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			auto asset = mem->allocate<DDLRoot>();
			asset->name = mem->duplicate_string(data["name"].get<std::string>());
			
			asset->ddlDef = mem->allocate<DDLDef>();
			auto j_defs = data["defs"];

			assert(j_defs.size());

			auto def = asset->ddlDef;
			auto def_index = 0;
			while(def)
			{
				auto j_def = j_defs[def_index];

				def->name = mem->duplicate_string(j_def["name"].get<std::string>());
				def->version = j_def["version"].get<unsigned short>();
				def->checksum = j_def["checksum"].get<unsigned int>(); // idk how to calc this
				def->flags = j_def["flags"].get<unsigned char>();
				def->bitSize = j_def["bitSize"].get<int>(); // should maybe be re-calculated?
				def->byteSize = j_def["byteSize"].get<int>(); // ^

				if (j_def["structs"].size())
				{
					def->structCount = static_cast<int>(j_def["structs"].size());
					def->structList = mem->allocate<DDLStruct>(def->structCount);
					for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
					{
						def->structList[struct_index].name = mem->duplicate_string(j_def["structs"][struct_index]["name"].get<std::string>());
						def->structList[struct_index].bitSize = j_def["structs"][struct_index]["bitSize"].get<int>(); // re-calc?
						if (j_def["structs"][struct_index]["members"].size())
						{
							def->structList[struct_index].memberCount = static_cast<int>(j_def["structs"][struct_index]["members"].size());
							def->structList[struct_index].members = mem->allocate<DDLMember>(def->structList[struct_index].memberCount);
							for (auto member_index = 0; member_index < def->structList[struct_index].memberCount; member_index++)
							{
								def->structList[struct_index].members[member_index].name = mem->duplicate_string(
									j_def["structs"][struct_index]["members"][member_index]["name"].get<std::string>());

								def->structList[struct_index].members[member_index].index = 
									j_def["structs"][struct_index]["members"][member_index]["index"].get<int>(); // not sure how index actually works

								def->structList[struct_index].members[member_index].parent = nullptr; // ??

								// bitSize = sizeof(type) * arraySize
								def->structList[struct_index].members[member_index].bitSize =
									j_def["structs"][struct_index]["members"][member_index]["bitSize"].get<int>(); // re-calc?

								def->structList[struct_index].members[member_index].limitSize =
									j_def["structs"][struct_index]["members"][member_index]["limitSize"].get<int>();

								def->structList[struct_index].members[member_index].offset =
									j_def["structs"][struct_index]["members"][member_index]["offset"].get<int>(); // re-calc?

								def->structList[struct_index].members[member_index].type =
									j_def["structs"][struct_index]["members"][member_index]["type"].get<int>(); // DDLType

								def->structList[struct_index].members[member_index].externalIndex =
									j_def["structs"][struct_index]["members"][member_index]["externalIndex"].get<int>(); // re-calc?

								def->structList[struct_index].members[member_index].rangeLimit =
									j_def["structs"][struct_index]["members"][member_index]["rangeLimit"].get<unsigned int>(); // has same val

								def->structList[struct_index].members[member_index].serverDelta =
									j_def["structs"][struct_index]["members"][member_index]["serverDelta"].get<unsigned int>(); // ^

								def->structList[struct_index].members[member_index].clientDelta =
									j_def["structs"][struct_index]["members"][member_index]["clientDelta"].get<unsigned int>(); // ^

								def->structList[struct_index].members[member_index].arraySize =
									j_def["structs"][struct_index]["members"][member_index]["arraySize"].get<int>();

								def->structList[struct_index].members[member_index].enumIndex =
									j_def["structs"][struct_index]["members"][member_index]["enumIndex"].get<int>(); // sometimes 0 or -1?

								def->structList[struct_index].members[member_index].permission =
									j_def["structs"][struct_index]["members"][member_index]["permission"].get<int>(); // maybe flags for write, read?
							}
						}

						// We can generate this ourselves.
						def->structList[struct_index].hashTableUpper.max = 1024;
						def->structList[struct_index].hashTableUpper.count = def->structList[struct_index].memberCount;
						def->structList[struct_index].hashTableUpper.list = mem->allocate<DDLHash>(def->structList[struct_index].hashTableUpper.count);
						for (auto list_index = 0; list_index < def->structList[struct_index].hashTableUpper.count; list_index++)
						{
							def->structList[struct_index].hashTableUpper.list[list_index].index = list_index;
							def->structList[struct_index].hashTableUpper.list[list_index].hash = Com_HashString(def->structList[struct_index].members[list_index].name);
						}
						std::qsort(def->structList[struct_index].hashTableUpper.list, 
							def->structList[struct_index].hashTableUpper.count, sizeof(DDLHash), compare_hash);

						// hashTableLower doesn't seem to be used.
						def->structList[struct_index].hashTableLower.max = 1024;
						def->structList[struct_index].hashTableLower.count = 0;
						def->structList[struct_index].hashTableLower.list = nullptr;
					}
				}

				if (j_def["enums"].size())
				{
					def->enumCount = static_cast<int>(j_def["enums"].size());
					def->enumList = mem->allocate<DDLEnum>(def->enumCount);
					for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
					{
						def->enumList[enum_index].name = mem->duplicate_string(j_def["enums"][enum_index]["name"].get<std::string>());
						if (j_def["enums"][enum_index]["members"].size())
						{
							def->enumList[enum_index].memberCount = static_cast<int>(j_def["enums"][enum_index]["members"].size());
							def->enumList[enum_index].members = mem->allocate<const char*>(def->enumList[enum_index].memberCount);
							for (auto member_index = 0; member_index < def->enumList[enum_index].memberCount; member_index++)
							{
								def->enumList[enum_index].members[member_index] = mem->duplicate_string(
									j_def["enums"][enum_index]["members"][member_index].get<std::string>());
							}
						}

						// We can generate this ourselves.
						def->enumList[enum_index].hashTable.max = 1024;
						def->enumList[enum_index].hashTable.count = def->enumList[enum_index].memberCount;
						def->enumList[enum_index].hashTable.list = mem->allocate<DDLHash>(def->enumList[enum_index].hashTable.count);
						for (auto list_index = 0; list_index < def->enumList[enum_index].hashTable.count; list_index++)
						{
							def->enumList[enum_index].hashTable.list[list_index].index = list_index;
							def->enumList[enum_index].hashTable.list[list_index].hash = Com_HashString(def->enumList[enum_index].members[list_index]);
						}
						std::qsort(def->enumList[enum_index].hashTable.list,
							def->enumList[enum_index].hashTable.count, sizeof(DDLHash), compare_hash);
					}
				}

				def->headerBitSize = j_def["headerBitSize"].get<int>(); // should maybe be re-calculated?
				def->headerByteSize = j_def["headerByteSize"].get<int>(); // ^
				def->reserveSize = j_def["reserveSize"].get<int>(); // ^
				def->userFlagsSize = j_def["userFlagsSize"].get<int>(); // ^
				def->paddingUsed = j_def["paddingUsed"].get<bool>();

				if (def_index == static_cast<int>(j_defs.size()) - 1)
				{
					def->next = nullptr;
					def = nullptr;
				}
				else
				{
					def->next = mem->allocate<DDLDef>();
					def = def->next;

					def_index++;
				}
			}

			return asset;
		}

		return nullptr;
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).ddlRoot;
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
			buf->align(3);

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
					buf->align(3);

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
			buf->align(3);

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
			buf->align(3);
			write_ddldef(buf, data->next);
			buf->clear_pointer(&dest->next);
		}

		return dest;
	}

	void ddl::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->ddlDef)
		{
			buf->align(3);
			write_ddldef(buf, data->ddlDef);
			buf->clear_pointer(&dest->ddlDef);
		}

		buf->pop_stream();
	}

	void ddl::dump(DDLRoot* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file("ddl\\"s + path + ".json"s);
		file.open("wb");

		ordered_json ddl;
		ddl["name"] = asset->name;
		
		assert(asset->ddlDef != nullptr);

		auto def = asset->ddlDef;
		int ddl_index = 0;

		ddl["defs"] = {};
		while (def)
		{
			ordered_json j_def;
			j_def["name"] = def->name;
			j_def["version"] = def->version;
			j_def["checksum"] = def->checksum; // checksum seems to be unused in the game
			j_def["flags"] = def->flags;
			j_def["bitSize"] = def->bitSize;
			j_def["byteSize"] = def->byteSize;

			j_def["structs"] = {};
			for (auto struct_index = 0; struct_index < def->structCount; struct_index++)
			{
				ordered_json j_struct;
				j_struct["name"] = def->structList[struct_index].name;
				j_struct["bitSize"] = def->structList[struct_index].bitSize;

				j_struct["members"] = {};
				for (auto member_index = 0; member_index < def->structList[struct_index].memberCount; member_index++)
				{
					ordered_json j_member;
					j_member["name"] = def->structList[struct_index].members[member_index].name;
					j_member["index"] = def->structList[struct_index].members[member_index].index;
					//j_member["parent"] = nullptr;
					//if (def->structList[struct_index].members[member_index].parent)
					//{
					//	j_member["parent"] = reinterpret_cast<DDLMember*>(def->structList[struct_index].members[member_index].parent)->name; // ??
					//}
					j_member["bitSize"] = def->structList[struct_index].members[member_index].bitSize;
					j_member["limitSize"] = def->structList[struct_index].members[member_index].limitSize;
					j_member["offset"] = def->structList[struct_index].members[member_index].offset;
					j_member["type"] = def->structList[struct_index].members[member_index].type;
					j_member["externalIndex"] = def->structList[struct_index].members[member_index].externalIndex;
					j_member["rangeLimit"] = def->structList[struct_index].members[member_index].rangeLimit;
					j_member["serverDelta"] = def->structList[struct_index].members[member_index].serverDelta;
					j_member["clientDelta"] = def->structList[struct_index].members[member_index].clientDelta;
					j_member["arraySize"] = def->structList[struct_index].members[member_index].arraySize;
					j_member["enumIndex"] = def->structList[struct_index].members[member_index].enumIndex;
					j_member["permission"] = def->structList[struct_index].members[member_index].permission;

					j_struct["members"][member_index] = j_member;
				}

				j_def["structs"][struct_index] = j_struct;
			}

			j_def["enums"] = {};
			for (auto enum_index = 0; enum_index < def->enumCount; enum_index++)
			{
				ordered_json j_enum;
				j_enum["name"] = def->enumList[enum_index].name;

				j_enum["members"] = {};
				for (auto member_index = 0; member_index < def->enumList[enum_index].memberCount; member_index++)
				{
					j_enum["members"][member_index] = def->enumList[enum_index].members[member_index];
				}

				j_def["enums"][enum_index] = j_enum;
			}

			j_def["headerBitSize"] = def->headerBitSize;
			j_def["headerByteSize"] = def->headerByteSize;
			j_def["reserveSize"] = def->reserveSize;
			j_def["userFlagsSize"] = def->userFlagsSize;
			j_def["paddingUsed"] = def->paddingUsed;

			ddl["defs"][ddl_index] = j_def;
			ddl_index++;

			def = def->next;
		}

		auto str = ddl.dump(4);
		file.write(str);

		file.close();
	}
}