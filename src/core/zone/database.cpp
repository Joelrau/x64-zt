#include <std_include.hpp>
#include "database.hpp"

namespace zonetool
{
	struct database
	{
		std::unordered_map<std::string, void*> assets[256];
		std::unordered_map<int, void*> gfx_globals;
		std::unordered_map<const void*, std::span<const XStreamFile>> image_stream_files;
		std::unordered_map<const void*, std::span<const XStreamFileKey>> image_stream_keys;
	};

	namespace
	{
		database*& active_database()
		{
			static database db;
			static database* active = &db;
			return active;
		}

		database& get_database()
		{
			return *active_database();
		}
	}

	std::shared_ptr<database> db_create()
	{
		return std::make_shared<database>();
	}

	db_scope::db_scope(database& db)
		: previous_(active_database())
	{
		active_database() = &db;
	}

	db_scope::~db_scope()
	{
		active_database() = this->previous_;
	}

	void* db_find_asset(const std::int32_t type, const std::string& name)
	{
		auto& assets = get_database().assets[type & 0xFF];
		const auto entry = assets.find(name);
		return entry != assets.end() ? entry->second : nullptr;
	}

	void db_add_asset(const std::int32_t type, const std::string& name, void* header)
	{
		get_database().assets[type & 0xFF][name] = header;
	}

	void db_remove_asset(const std::int32_t type, const std::string& name)
	{
		get_database().assets[type & 0xFF].erase(name);
	}

	void db_enum_assets(const std::int32_t type, const std::function<void(void*)>& callback)
	{
		for (const auto& [name, header] : get_database().assets[type & 0xFF])
		{
			callback(header);
		}
	}

	void db_clear()
	{
		auto& db = get_database();
		for (auto& assets : db.assets)
		{
			assets.clear();
		}

		db.gfx_globals.clear();
		db.image_stream_files.clear();
		db.image_stream_keys.clear();
	}

	void* get_x_gfx_globals_for_zone(const int zone)
	{
		const auto& gfx_globals = get_database().gfx_globals;
		const auto entry = gfx_globals.find(zone);
		return entry != gfx_globals.end() ? entry->second : nullptr;
	}

	void insert_x_gfx_globals_for_zone(const int zone, void* globals)
	{
		get_database().gfx_globals[zone] = globals;
	}

	std::span<const XStreamFile> get_image_stream_files(const void* image)
	{
		const auto& files = get_database().image_stream_files;
		const auto entry = files.find(image);
		return entry != files.end() ? entry->second : std::span<const XStreamFile>{};
	}

	void set_image_stream_files(const void* image, const std::span<const XStreamFile> files)
	{
		get_database().image_stream_files[image] = files;
	}

	std::span<const XStreamFileKey> get_image_stream_keys(const void* image)
	{
		const auto& keys = get_database().image_stream_keys;
		const auto entry = keys.find(image);
		return entry != keys.end() ? entry->second : std::span<const XStreamFileKey>{};
	}

	void set_image_stream_keys(const void* image, const std::span<const XStreamFileKey> keys)
	{
		get_database().image_stream_keys[image] = keys;
	}
}
