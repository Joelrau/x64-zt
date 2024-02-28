#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw7
{
	class weapon_def : public asset_interface
	{
	private:
		std::string name_;
		WeaponCompleteDef* asset_ = nullptr;

		std::vector<std::pair<scr_string_t*, const char*>> script_strings;
		void add_script_string(scr_string_t* ptr, const char* str);
		const char* get_script_string(scr_string_t* ptr);

		void write_overlay(ADSOverlay* data, ADSOverlay* dest, zone_base* zone, zone_buffer* buf);
		void write_weapon_def(WeaponCompleteDef* parent_data, zone_base* zone, zone_buffer* buf);

		void load_depending_weapondef(WeaponDef* asset, zone_base* zone);

		void prepare(AnimOverride* asset, zone_buffer* buf);
		void prepare(SFXOverride* asset, zone_buffer* buf);
		void prepare(VFXOverride* asset, zone_buffer* buf);
		void prepare_weapondef(WeaponDef* asset, zone_buffer* buf);

		void parse(AnimOverride* asset, ordered_json& data, zone_memory* mem);
		void parse(SFXOverride* asset, ordered_json& data, zone_memory* mem);
		void parse(VFXOverride* asset, ordered_json& data, zone_memory* mem);
		void parse(GrenadeRotationParams* asset, ordered_json& data, zone_memory* mem);
		void parse(ADSOverlay* asset, ordered_json& data, zone_memory* mem);
		void parse_weapondef(WeaponDef* asset, WeaponCompleteDef* parent, ordered_json& data, zone_memory* mem);
		void parse_NoteTrackToSoundEntry(NoteTrackToSoundEntry* asset, ordered_json& data, zone_memory* mem);
		void parse_NoteTrackToSuitSoundEntry(NoteTrackToSuitSoundEntry* asset, ordered_json& data, zone_memory* mem);
		void parse_complete(WeaponCompleteDef* asset, ordered_json& data, zone_memory* mem);

		static void dump(AnimOverride* asset, ordered_json& data);
		static void dump(SFXOverride* asset, ordered_json& data);
		static void dump(VFXOverride* asset, ordered_json& data);
		static void dump(GrenadeRotationParams* asset, ordered_json& data);
		static void dump(ADSOverlay* asset, ordered_json& data);
		static void dump_weapondef(WeaponDef* asset, WeaponCompleteDef* parent, ordered_json& data);
		static void dump_NoteTrackToSoundEntry(NoteTrackToSoundEntry* asset, ordered_json& data);
		static void dump_NoteTrackToSuitSoundEntry(NoteTrackToSuitSoundEntry* asset, ordered_json& data);
		static ordered_json dump_complete(WeaponCompleteDef* asset);

	public:
		WeaponCompleteDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WeaponCompleteDef* asset);
	};
}