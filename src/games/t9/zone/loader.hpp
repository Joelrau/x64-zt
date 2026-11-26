#pragma once

namespace zonetool::t9
{
	struct loaded_asset
	{
		XAssetType type;
		void* header;
	};

	struct loaded_zone
	{
		std::string name;
		xfile::fastfile file;
		std::unique_ptr<zone_reader> reader;
		XAssetList list;
		std::vector<loaded_asset> assets;
	};

	struct asset_loader
	{
		std::size_t size;
		std::size_t alignment;
		void (*read)(zone_reader& reader, void* asset);
		void (*write)(zone_writer& writer, void* asset);
		std::string (*name)(const void* asset);
		void (*dump)(void* asset);
		bool (*is_placeholder)(const void* asset);
		void (*copy_name)(void* destination, const void* source);
	};

	struct asset_file_chunk
	{
		std::uint8_t block;
		std::uint64_t offset;
		std::uint8_t source_block;
		std::uint64_t source_offset;
		std::uint64_t size;
	};

	struct asset_file_pointer
	{
		std::uint8_t block;
		std::uint64_t offset;
		std::uint8_t target_block;
		std::uint64_t target_offset;
	};

	struct asset_file
	{
		std::unique_ptr<loaded_zone> zone;
		std::string source_zone;
		std::vector<asset_file_chunk> chunks;
		std::vector<asset_file_pointer> pointers;
	};

	struct address_range
	{
		std::uint64_t end;
		std::uint64_t target;
	};

	const asset_loader& get_asset_loader(XAssetType type);
	const zone_layout& get_zone_layout();
	std::string get_asset_name(XAssetType type, const void* header);

	std::unique_ptr<loaded_zone> open_zone(const std::filesystem::path& path);
	std::unique_ptr<loaded_zone> load_zone(const std::filesystem::path& path, bool stop_at_error = false);

	void write_inline_asset(zone_writer& writer, std::int32_t type, const void* header);
	void write_asset_list(zone_writer& writer, const XAssetList& list);

	std::string get_asset_file_path(XAssetType type, const std::string& name);
	std::string write_asset_file(const loaded_zone& zone, const XAsset& asset, const std::unordered_set<const void*>& list_headers);
	asset_file load_asset_file(const std::string& name, const std::string& data);
}
