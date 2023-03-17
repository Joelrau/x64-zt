#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class IScriptableDef : public IAsset
	{
	private:
		std::string name_;
		ScriptableDef* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void parse_scriptable_event_def(ScriptableEventDef* event, assetmanager::reader& read, ZoneMemory* mem);
		void prepare_scriptable_event_def(ScriptableEventDef* event, ZoneBuffer* buf, ZoneMemory* mem);
		void load_depending_scriptable_event_def(IZone* zone, ScriptableEventDef* event);
		void write_scriptable_event_def(IZone* zone, ZoneBuffer* buf, ScriptableEventDef* data, ScriptableEventDef* dest);
		static void dump_scriptable_event_def(ScriptableEventDef* event, assetmanager::dumper& dump);

	public:
		ScriptableDef* parse(std::string name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		void write(IZone* zone, ZoneBuffer* buf) override;

		static void dump(ScriptableDef* asset);
	};
}