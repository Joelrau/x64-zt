#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class weapon_def : public asset_interface
	{
	private:
		std::string name_;
		WeaponCompleteDef* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

	public:
		WeaponCompleteDef* parse(const std::string& name, zone_memory* mem);
		WeaponDef* parse_weapondef(json& data, WeaponCompleteDef* baseAsset, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void load_depending_weapondef(zone_base* zone, WeaponDef* data);

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;
		void write_weapondef(zone_base* zone, zone_buffer* buf, WeaponCompleteDef* complete, WeaponDef* data);

		static void dump(WeaponCompleteDef* asset);
		static json dump_complete(WeaponCompleteDef* asset);
		static json dump_weapondef(WeaponDef* asset);
	};
}