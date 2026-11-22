#include <std_include.hpp>

#include "../include.hpp"
#include "vectorfield.hpp"

#include <h1/assets/vectorfield.hpp>

namespace zonetool::s2::converter::h1::vectorfield
{
	namespace
	{
		std::unordered_map<std::string, bool> dumped_fields;

		zonetool::h1::VectorField* convert(const VectorField* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::h1::VectorField>();
			new_asset->name = asset->name;
			std::memcpy(&new_asset->bounds, &asset->worldBounds, sizeof(new_asset->bounds));
			new_asset->fieldsCount = static_cast<int>(asset->numSubFields);
			new_asset->fields = allocator.allocate_array<zonetool::h1::VectorFieldSubField>(asset->numSubFields);
			for (auto i = 0u; i < asset->numSubFields; i++)
			{
				const auto& field = asset->subFields[i];
				auto& new_field = new_asset->fields[i];
				new_field.linearData = reinterpret_cast<zonetool::h1::vec4_t*>(field.linearData);
				std::memcpy(&new_field.worldBounds, &field.worldBounds, sizeof(new_field.worldBounds));
				std::memcpy(new_field.ds, field.ds, sizeof(new_field.ds));
				std::memcpy(new_field.numEntries, field.numEntries, sizeof(new_field.numEntries));
				new_field.flags = field.flags;
				new_field.type = static_cast<zonetool::h1::VectorFieldType>(field.type);
			}

			return new_asset;
		}
	}

	bool dump_used(const std::string& name)
	{
		const auto asset_name = "vectorfields/" + name;
		if (const auto entry = dumped_fields.find(asset_name); entry != dumped_fields.end())
		{
			return entry->second;
		}

		const auto* asset = db_find_x_asset_header_safe(ASSET_TYPE_VECTORFIELD, asset_name).vectorField;
		const auto found = asset && asset->subFields;
		if (found)
		{
			utils::memory::allocator allocator;
			zonetool::h1::vector_field::dump(convert(asset, allocator));
			add_csv_row(zonetool::h1::type_to_string(zonetool::h1::ASSET_TYPE_VECTORFIELD), asset_name);
		}
		else
		{
			ZONETOOL_WARNING("vectorfield \"%s\" is not loaded", asset_name.data());
		}

		dumped_fields.emplace(asset_name, found);
		return found;
	}
}
