#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class addon_map_ents : public asset_interface
	{
	private:
		std::string name_;
		AddonMapEnts* asset_ = nullptr;

		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entity_strings(const std::string& name, char* entityString, int numEntityChars);

		void parse_triggers(zone_memory* mem, std::string name, MapTriggers* trigger);
		void parse_entity_strings(zone_memory* mem, std::string name, char** entityStrings, int* numEntityChars);

	public:
		AddonMapEnts* parse(std::string name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		static void write_triggers(zone_buffer* buf, MapTriggers* dest);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(AddonMapEnts* asset);
	};
}