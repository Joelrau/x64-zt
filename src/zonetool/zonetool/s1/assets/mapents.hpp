#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	class map_ents : public asset_interface
	{
	private:
		std::string name_;
		MapEnts* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		static void dump_spline_list(const std::string& name, SplineRecordList* splineList);
		static void dump_spawn_list(const std::string& name, SpawnPointRecordList* spawnList);
		static void dump_client_blend_triggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend);
		static void dump_client_triggers(const std::string& name, ClientTriggers* clientTrigger);
		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entity_strings(const std::string& name, char* entityString, int numEntityChars);

		void parse_spline_list(zone_memory* mem, std::string name, SplineRecordList* splineList);
		void parse_spawn_list(zone_memory* mem, std::string name, SpawnPointRecordList* spawnList);
		void parse_client_blend_triggers(zone_memory* mem, std::string name, ClientTriggerBlend* clientTriggerBlend);
		void parse_client_triggers(zone_memory* mem, std::string name, ClientTriggers* clientTrigger);
		void parse_triggers(zone_memory* mem, std::string name, MapTriggers* trigger);
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

		static void write_triggers(zone_buffer* buf, MapTriggers* dest);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(MapEnts* asset);
	};
}