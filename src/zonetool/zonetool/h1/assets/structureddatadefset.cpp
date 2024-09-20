#include <std_include.hpp>
#include "structureddatadefset.hpp"

namespace zonetool::h1
{
	void structured_data_def_set::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* structured_data_def_set::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	namespace
	{
		int get_data_type_index(StructuredDataType type)
		{
			switch (type.type)
			{
			case DATA_ENUM:
				return type.u.enumIndex;
			case DATA_ENUM_ARRAY:
				return type.u.enumedArrayIndex;
			case DATA_INDEXED_ARRAY:
				return type.u.indexedArrayIndex;
			case DATA_STRUCT:
				return type.u.structIndex;
			case DATA_STRING:
				return type.u.stringDataLength;
			default:
				return type.u.index;
			}
		}
	}

	StructuredDataDefSet* structured_data_def_set::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(name);
		file.open("rb");

		if (file.get_fp())
		{
			ZONETOOL_INFO("Parsing structureddatadef \"%s\"...", name.data());

			const auto size = file.size();
			auto bytes = file.read_bytes(size);
			file.close();

			auto data = json::parse(bytes);

			auto asset = mem->allocate<StructuredDataDefSet>();
			asset->name = mem->duplicate_string(data["name"].get<std::string>());

			asset->defCount = 1;
			asset->defs = mem->allocate<StructuredDataDef>();

			auto* def = &asset->defs[0];
			def->version = data["version"].get<int>();
			def->formatChecksum = data["formatChecksum"].get<unsigned int>();

			def->enumCount = data["enumCount"].get<int>();
			def->enums = mem->allocate<StructuredDataEnum>(def->enumCount);
			for (auto i = 0; i < def->enumCount; i++)
			{
				def->enums[i].entryCount = data["enums"][i]["entryCount"].get<int>();
				def->enums[i].reservedEntryCount = data["enums"][i]["reservedEntryCount"].get<int>();
				def->enums[i].entries = mem->allocate<StructuredDataEnumEntry>(def->enums[i].entryCount);
				for (auto a = 0; a < def->enums[i].entryCount; a++)
				{
					this->add_script_string(&def->enums[i].entries[a].string, 
						mem->duplicate_string(data["enums"][i]["entries"][a]["string"].get<std::string>()));
					def->enums[i].entries[a].index = data["enums"][i]["entries"][a]["index"].get<unsigned short>();
				}
			}

			def->structCount = data["structCount"].get<int>();
			def->structs = mem->allocate<StructuredDataStruct>(def->structCount);
			for (auto i = 0; i < def->structCount; i++)
			{
				def->structs[i].propertyCount = data["structs"][i]["propertyCount"].get<int>();
				def->structs[i].properties = mem->allocate<StructuredDataStructProperty>(def->structs[i].propertyCount);
				for (auto a = 0; a < def->structs[i].propertyCount; a++)
				{
					this->add_script_string(&def->structs[i].properties[a].name,
						mem->duplicate_string(data["structs"][i]["properties"][a]["name"].get<std::string>()));
					def->structs[i].properties[a].type.type = data["structs"][i]["properties"][a]["type"].get<StructuredDataTypeCategory>();
					def->structs[i].properties[a].type.u.index = data["structs"][i]["properties"][a]["index"].get<int>();
					def->structs[i].properties[a].offset = data["structs"][i]["properties"][a]["offset"].get<unsigned int>();
					def->structs[i].properties[a].validation = data["structs"][i]["properties"][a]["validation"].get<StructuredDataValidationType>();
				}
				def->structs[i].size = data["structs"][i]["size"].get<int>();
				def->structs[i].bitOffset = data["structs"][i]["bitOffset"].get<unsigned int>();
			}

			def->indexedArrayCount = data["indexedArrayCount"].get<int>();
			def->indexedArrays = mem->allocate<StructuredDataIndexedArray>(def->indexedArrayCount);
			for (auto i = 0; i < def->indexedArrayCount; i++)
			{
				def->indexedArrays[i].arraySize = data["indexedArrays"][i]["arraySize"].get<int>();
				def->indexedArrays[i].elementType.type = data["indexedArrays"][i]["type"].get<StructuredDataTypeCategory>();
				def->indexedArrays[i].elementType.u.index = data["indexedArrays"][i]["index"].get<int>();
				def->indexedArrays[i].elementSize = data["indexedArrays"][i]["elementSize"].get<unsigned int>();
			}

			def->enumedArrayCount = data["enumedArrayCount"].get<int>();
			def->enumedArrays = mem->allocate<StructuredDataEnumedArray>(def->enumedArrayCount);
			for (auto i = 0; i < def->enumedArrayCount; i++)
			{
				def->enumedArrays[i].enumIndex = data["enumedArrays"][i]["enumIndex"].get<int>();
				def->enumedArrays[i].elementType.type = data["enumedArrays"][i]["type"].get<StructuredDataTypeCategory>();
				def->enumedArrays[i].elementType.u.index = data["enumedArrays"][i]["index"].get<int>();
				def->enumedArrays[i].elementSize = data["enumedArrays"][i]["elementSize"].get<unsigned int>();
			}

			def->rootType.type = data["type"].get<StructuredDataTypeCategory>();
			def->rootType.u.index = data["index"].get<int>();

			def->size = data["size"].get<unsigned int>();

			return asset;
		}

		return nullptr;
	}

	void structured_data_def_set::init(const std::string& name, zone_memory* mem)
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
			this->asset_ = db_find_x_asset_header_safe(XAssetType(this->type()), this->name_.data()).structuredDataDefSet;
		}
	}

	void structured_data_def_set::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;

		for (auto def_index = 0u; def_index < data->defCount; def_index++)
		{
			for (auto enum_index = 0; enum_index < data->defs[def_index].enumCount; enum_index++)
			{
				for (auto entry_index = 0; entry_index < data->defs[def_index].enums[enum_index].entryCount; entry_index++)
				{
					data->defs[def_index].enums[enum_index].entries[entry_index].string = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&data->defs[def_index].enums[enum_index].entries[entry_index].string)));
				}
			}

			for (auto struct_index = 0; struct_index < data->defs[def_index].structCount; struct_index++)
			{
				for (auto property_index = 0; property_index < data->defs[def_index].structs[struct_index].propertyCount; property_index++)
				{
					data->defs[def_index].structs[struct_index].properties[property_index].name = static_cast<scr_string_t>(buf->write_scriptstring(
						this->get_script_string(&data->defs[def_index].structs[struct_index].properties[property_index].name)));
				}
			}
		}
	}

	void structured_data_def_set::load_depending(zone_base* zone)
	{
	}

	std::string structured_data_def_set::name()
	{
		return this->name_;
	}

	std::int32_t structured_data_def_set::type()
	{
		return ASSET_TYPE_STRUCTURED_DATA_DEF;
	}

	void structured_data_def_set::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->defs)
		{
			buf->align(3);

			auto* dest_defs = buf->write(data->defs, data->defCount);
			auto* defs = data->defs;

			for (auto def_index = 0u; def_index < data->defCount; def_index++)
			{
				auto* dest_def = &dest_defs[def_index];
				auto* def = &defs[def_index];

				if (def->enums)
				{
					buf->align(3);

					auto* dest_enums = buf->write(def->enums, def->enumCount);
					auto* enums = def->enums;

					for (auto i = 0; i < def->enumCount; i++)
					{
						auto* dest_enum = &dest_enums[i];
						auto* enum_ = &enums[i];

						if (enum_->entries)
						{
							buf->align(3);
							buf->write(enum_->entries, enum_->entryCount);
							buf->clear_pointer(&dest_enum->entries);
						}
					}

					buf->clear_pointer(&dest_def->enums);
				}

				if (def->structs)
				{
					buf->align(3);

					auto* dest_structs = buf->write(def->structs, def->structCount);
					auto* structs = def->structs;

					for (auto i = 0; i < def->structCount; i++)
					{
						auto* dest_struct = &dest_structs[i];
						auto* struct_ = &structs[i];

						if (struct_->properties)
						{
							buf->align(3);
							buf->write(struct_->properties, struct_->propertyCount);
							buf->clear_pointer(&dest_struct->properties);
						}
					}

					buf->clear_pointer(&dest_def->structs);
				}

				if (def->indexedArrays)
				{
					buf->align(3);
					buf->write(def->indexedArrays, def->indexedArrayCount);
					buf->clear_pointer(&dest_def->indexedArrays);
				}

				if (def->enumedArrays)
				{
					buf->align(3);
					buf->write(def->enumedArrays, def->enumedArrayCount);
					buf->clear_pointer(&dest_def->enumedArrays);
				}
			}

			buf->clear_pointer(&dest->defs);
		}

		buf->pop_stream();
	}

	void structured_data_def_set::dump(StructuredDataDefSet* asset)
	{
		const auto path = asset->name;
		auto file = filesystem::file(path);
		file.open("wb");

		StructuredDataDef* current_def = nullptr;
		auto current_version = 0;

		for (unsigned int i = 0; i < asset->defCount; i++)
		{
			if (asset->defs[i].version > current_version)
			{
				current_version = asset->defs[i].version;
				current_def = &asset->defs[i];
			}
		}

		if (current_def)
		{
			ordered_json def;
			ordered_json temp0;
			ordered_json temp1;

			def["name"] = asset->name;
			def["version"] = current_def->version;
			def["formatChecksum"] = current_def->formatChecksum;
			def["enumCount"] = current_def->enumCount;
			def["enums"] = {};
			for (auto i = 0; i < current_def->enumCount; i++)
			{
				temp0.clear();
				temp0["entryCount"] = current_def->enums[i].entryCount;
				temp0["reservedEntryCount"] = current_def->enums[i].reservedEntryCount;
				temp0["entries"] = {};
				for (auto a = 0; a < current_def->enums[i].entryCount; a++)
				{
					temp1.clear();
					temp1["string"] = SL_ConvertToString(current_def->enums[i].entries[a].string);
					temp1["index"] = current_def->enums[i].entries[a].index;
					temp0["entries"].push_back(temp1);
				}
				def["enums"].push_back(temp0);
			}
			def["structCount"] = current_def->structCount;
			def["structs"] = {};
			for (auto i = 0; i < current_def->structCount; i++)
			{
				temp0.clear();
				temp0["propertyCount"] = current_def->structs[i].propertyCount;
				temp0["properties"] = {};
				for (auto a = 0; a < current_def->structs[i].propertyCount; a++)
				{
					temp1.clear();
					temp1["name"] = SL_ConvertToString(current_def->structs[i].properties[a].name);
					temp1["type"] = current_def->structs[i].properties[a].type.type;
					temp1["index"] = get_data_type_index(current_def->structs[i].properties[a].type);
					temp1["offset"] = current_def->structs[i].properties[a].offset;
					temp1["validation"] = current_def->structs[i].properties[a].validation;
					temp0["properties"].push_back(temp1);
				}
				temp0["size"] = current_def->structs[i].size;
				temp0["bitOffset"] = current_def->structs[i].bitOffset;
				def["structs"].push_back(temp0);
			}
			def["indexedArrayCount"] = current_def->indexedArrayCount;
			def["indexedArrays"] = {};
			for (auto i = 0; i < current_def->indexedArrayCount; i++)
			{
				temp0.clear();
				temp0["arraySize"] = current_def->indexedArrays[i].arraySize;
				temp0["type"] = current_def->indexedArrays[i].elementType.type;
				temp0["index"] = get_data_type_index(current_def->indexedArrays[i].elementType);
				temp0["elementSize"] = current_def->indexedArrays[i].elementSize;
				def["indexedArrays"].push_back(temp0);
			}
			def["enumedArrayCount"] = current_def->enumedArrayCount;
			def["enumedArrays"] = {};
			for (auto i = 0; i < current_def->enumedArrayCount; i++)
			{
				temp0.clear();
				temp0["enumIndex"] = current_def->enumedArrays[i].enumIndex;
				temp0["type"] = current_def->enumedArrays[i].elementType.type;
				temp0["index"] = get_data_type_index(current_def->enumedArrays[i].elementType);
				temp0["elementSize"] = current_def->enumedArrays[i].elementSize;
				def["enumedArrays"].push_back(temp0);
			}
			def["type"] = current_def->rootType.type;
			def["index"] = get_data_type_index(current_def->rootType);
			def["size"] = current_def->size;

			auto str = def.dump(4);

			def.clear();
			temp0.clear();
			temp1.clear();

			file.write(str);
		}

		file.close();
	}
}