#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	class scriptable_def : public asset_interface
	{
	private:
		std::string name_;
		ScriptableDef* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void parse_scriptable_event_def(ScriptableEventDef* event, assetmanager::reader& read, zone_memory* mem);
		void prepare_scriptable_event_def(ScriptableEventDef* event, zone_buffer* buf, zone_memory* mem);
		void load_depending_scriptable_event_def(zone_base* zone, ScriptableEventDef* event);
		void write_scriptable_event_def(zone_base* zone, zone_buffer* buf, ScriptableEventDef* data, ScriptableEventDef* dest);
		static void dump_scriptable_event_def(ScriptableEventDef* event, assetmanager::dumper& dump);

	public:
		ScriptableDef* parse(std::string name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		void write(zone_base* zone, zone_buffer* buf) override;

		static void dump(ScriptableDef* asset);
	};
}