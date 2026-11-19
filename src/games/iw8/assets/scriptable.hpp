#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class scriptable_def : public asset_interface
	{
	private:
		std::string name_;
		ScriptableDef* asset_ = nullptr;
		script_string_map script_strings_;

	public:
		ScriptableDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, ScriptableDef* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(ScriptableDef* asset);
	};

	class composite_model : public asset_interface
	{
	private:
		std::string name_;
		XCompositeModelDef* asset_ = nullptr;
		script_string_map script_strings_;

	public:
		XCompositeModelDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, XCompositeModelDef* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(XCompositeModelDef* asset);
	};
}
