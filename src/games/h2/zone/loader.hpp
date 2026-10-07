#pragma once

namespace zonetool::h2
{
	struct loaded_asset
	{
		XAssetType type;
		void* header;
		bool referenced;
	};

	struct loaded_zone
	{
		std::string name;
		xfile::fastfile file;
		std::unique_ptr<zone_reader> reader;
		std::vector<loaded_asset> assets;
	};

	const char* get_asset_name(XAssetType type, const void* header);

	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path);
}
