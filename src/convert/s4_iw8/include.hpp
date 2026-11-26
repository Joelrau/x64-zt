#pragma once

#include <s4/s4.hpp>
#include <iw8/iw8.hpp>

namespace zonetool::s4::converter::iw8
{
	const std::filesystem::path& iw8_folder();
	void set_iw8_folder(const std::filesystem::path& folder);
	void add_csv_row(std::string_view type, std::string_view name);
	const char* bsp_name(const char* map_name, utils::memory::allocator& allocator);
	zonetool::iw8::SpatialPartition_Tree* convert_spatial_tree(const SpatialPartition_Tree* tree, utils::memory::allocator& allocator);

	template <typename Target, typename Source>
	Target* name_reference(const Source* asset, utils::memory::allocator& allocator)
	{
		if (!asset)
		{
			return nullptr;
		}

		const auto result = allocator.allocate<Target>();
		result->name = asset->name;
		return result;
	}
}
