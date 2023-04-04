#pragma once

namespace zonetool::iw6
{
	class asset_interface;

	class zone_base
	{
	public:
		virtual asset_interface* find_asset(std::int32_t type, const std::string& name)
		{
			return nullptr;
		}

		virtual void* get_asset_pointer(std::int32_t type, const std::string& name) = 0;

		virtual void add_asset_of_type_by_pointer(std::int32_t type, void* pointer) = 0;

		virtual void add_asset_of_type(const std::string& type, const std::string& name) = 0;
		virtual void add_asset_of_type(std::int32_t type, const std::string& name) = 0;
		virtual std::int32_t get_type_by_name(const std::string& type) = 0;

		virtual void build(zone_buffer* buf) = 0;
	};
}
