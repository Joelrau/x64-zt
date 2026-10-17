#pragma once

namespace zonetool::s2
{
	class scriptable_def : public asset_base<ScriptableDef, ASSET_TYPE_SCRIPTABLE>
	{
	public:
		static void read(zone_reader& reader, ScriptableDef* asset);
		ScriptableDef* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(ScriptableDef* asset);
	};
}
