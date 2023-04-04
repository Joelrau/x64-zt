#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class sound : public asset_interface
	{
	private:
		std::string name_;
		snd_alias_list_t* asset_ = nullptr;

		static void write_soundfile(zone_base* zone, zone_buffer* buf, SoundFile* dest);
		static void write_head(zone_base* zone, zone_buffer* buf, snd_alias_t* dest);

		static void json_parse_snd_alias(snd_alias_t* asset, json snddata, zone_memory* mem);
		static snd_alias_list_t* json_parse(const std::string& name, zone_memory* mem);
		static void json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset);
		static void json_dump(snd_alias_list_t* asset);

	public:
		snd_alias_list_t* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(snd_alias_list_t* asset);
	};
}