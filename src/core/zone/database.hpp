#pragma once

#include "../xfile/structs.hpp"

namespace zonetool
{
	void* db_find_asset(std::int32_t type, const std::string& name);
	void db_add_asset(std::int32_t type, const std::string& name, void* header);
	void db_remove_asset(std::int32_t type, const std::string& name);
	void db_clear();

	template <typename H>
	H db_find_x_asset_header(const std::int32_t type, const char* name, [[maybe_unused]] const int create_default)
	{
		return H{.data = db_find_asset(type, name)};
	}

	template <typename H, typename E>
	H db_find_x_asset_header_safe(const std::int32_t type, const std::string& name)
	{
		return H{.data = db_find_asset(type, name)};
	}

	inline bool DB_IsXAssetDefault([[maybe_unused]] const std::int32_t type, [[maybe_unused]] const char* name)
	{
		return false;
	}

	void* get_x_gfx_globals_for_zone(int zone);
	void insert_x_gfx_globals_for_zone(int zone, void* globals);

	template <typename T>
	T* get_x_gfx_globals_for_zone(const int zone)
	{
		return static_cast<T*>(get_x_gfx_globals_for_zone(zone));
	}

	std::span<const XStreamFile> get_image_stream_files(const void* image);
	void set_image_stream_files(const void* image, std::span<const XStreamFile> files);
}
