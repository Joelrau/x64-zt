#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class scriptable_def : public asset_interface
	{
	private:
		std::string name_;
		ScriptableDef* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void parse_scriptable_event(assetmanager::reader& read, ScriptableEventDef* data);
		void parse_state_base(assetmanager::reader& read, ScriptableStateBaseDef* data);
		void parse_state(assetmanager::reader& read, ScriptableStateDef* data);
		void parse_part(assetmanager::reader& read, ScriptablePartDef* data);

		void prepare_scriptable_event(zone_buffer* buf, ScriptableEventDef* data);
		void prepare_state_base(zone_buffer* buf, ScriptableStateBaseDef* data);
		void prepare_state(zone_buffer* buf, ScriptableStateDef* data);
		void prepare_part(zone_buffer* buf, ScriptablePartDef* data);

		void load_depending_scriptable_event(zone_base* zone, ScriptableEventDef* data);
		void load_depending_state_base(zone_base* zone, ScriptableStateBaseDef* data);
		void load_depending_state(zone_base* zone, ScriptableStateDef* data);
		void load_depending_part(zone_base* zone, ScriptablePartDef* data);

		static void dump_scriptable_event(assetmanager::dumper& dump, ScriptableEventDef* data);
		static void dump_state_base(assetmanager::dumper& dump, ScriptableStateBaseDef* data);
		static void dump_state(assetmanager::dumper& dump, ScriptableStateDef* data);
		static void dump_part(assetmanager::dumper& dump, ScriptablePartDef* data);

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