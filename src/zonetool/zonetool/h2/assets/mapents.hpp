#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class IMapEnts : public IAsset
	{
	private:
		std::string name_;
		MapEnts* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		static void dump_splineList(const std::string& name, SplineRecordList* splineList);
		static void dump_spawnList(const std::string& name, SpawnPointRecordList* spawnList);
		static void dump_clientBlendTriggers(const std::string& name, ClientTriggerBlend* clientTriggerBlend);
		static void dump_clientTriggers(const std::string& name, ClientTriggers* clientTrigger);
		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars);

		void parse_splineList(ZoneMemory* mem, std::string name, SplineRecordList* splineList);
		void parse_spawnList(ZoneMemory* mem, std::string name, SpawnPointRecordList* spawnList);
		void parse_clientBlendTriggers(ZoneMemory* mem, std::string name, ClientTriggerBlend* clientTriggerBlend);
		void parse_clientTriggers(ZoneMemory* mem, std::string name, ClientTriggers* clientTrigger);
		void parse_triggers(ZoneMemory* mem, std::string name, MapTriggers* trigger);
		void parse_entityStrings(ZoneMemory* mem, std::string name, char** entityStrings, int* numEntityChars);

	public:
		MapEnts* parse(std::string name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		static void write_triggers(ZoneBuffer* buf, MapTriggers* dest);
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(MapEnts* asset);
	};
}