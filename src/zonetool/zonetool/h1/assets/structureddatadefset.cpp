#include <std_include.hpp>
#include "structureddatadefset.hpp"

namespace zonetool::h1
{
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

	void IStructuredDataDefSet::dump(StructuredDataDefSet* asset)
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