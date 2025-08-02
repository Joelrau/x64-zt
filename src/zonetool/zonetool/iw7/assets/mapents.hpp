#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class map_ents : public asset_interface
	{
	private:
		std::string name_;
		MapEnts* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		static void dump_spawn_list(const std::string& name, SpawnPointRecordList* spawnList);
		static void dump_spawners(const std::string& name, SpawnerList* spawners);
		static void dump_entity_strings(const std::string& name, char* entityString, int numEntityChars);

		void parse_spawn_list(zone_memory* mem, std::string name, SpawnPointRecordList* spawnList);
		void parse_spawners(zone_memory* mem, std::string name, SpawnerList* spawners);
		void parse_entity_strings(zone_memory* mem, std::string name, char** entityStrings, int* numEntityChars);

	public:
		static void add_entity_string(const std::string& line);
		static void clear_entity_strings();

		MapEnts* parse(std::string name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		static void write_triggers(zone_base* zone, zone_buffer* buf, MapTriggers* dest);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(MapEnts* asset);
	};
}