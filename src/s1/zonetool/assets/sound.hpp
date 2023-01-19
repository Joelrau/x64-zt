#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	class ISound : public IAsset
	{
	private:
		std::string name_;
		snd_alias_list_t* asset_ = nullptr;

		static void write_soundfile(IZone* zone, ZoneBuffer* buf, SoundFile* dest);
		static void write_head(IZone* zone, ZoneBuffer* buf, snd_alias_t* dest);

		static void json_parse_snd_alias(snd_alias_t* asset, json snddata, ZoneMemory* mem);
		static snd_alias_list_t* json_parse(const std::string& name, ZoneMemory* mem);

		static void json_dump_snd_alias(ordered_json& sound, snd_alias_t* asset);
		static void json_dump(snd_alias_list_t* asset);

	public:
		snd_alias_list_t* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(snd_alias_list_t* asset);
	};
}