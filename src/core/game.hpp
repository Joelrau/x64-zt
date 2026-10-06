#pragma once

#include "xfile/xfile.hpp"

namespace zonetool
{
	using asset_dumper = void(*)(std::int32_t type, void* header);
	using dump_command = void(*)(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path, asset_dumper converter);
	using zone_command = void(*)(const std::filesystem::path& zone_folder, const std::filesystem::path& zone_path);
	using build_command = void(*)(const std::filesystem::path& zone_folder, const std::string& name);

	struct game
	{
		std::string_view name;
		std::string_view display_name;
		std::string_view steam_folder;
		xfile::format format;
		dump_command dump_zone;
		zone_command verify_zone;
		build_command build_zone;
	};

	struct converter
	{
		std::string_view source;
		std::string_view target;
		asset_dumper dump_asset;
	};
}
