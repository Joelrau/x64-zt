#pragma once

namespace zonetool::s4
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

	struct asset_loader
	{
		std::size_t size;
		void (*read)(zone_reader& reader, void* asset);
		const char** (*name)(void* asset);
	};

	const asset_loader& get_asset_loader(XAssetType type);
	const char* get_asset_name(XAssetType type, const void* header);
	std::size_t asset_alignment(XAssetType type);
	std::string decrypt_script_string(const char* string);

	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path);
}
