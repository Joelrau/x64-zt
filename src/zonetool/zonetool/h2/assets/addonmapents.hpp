#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class IAddonMapEnts : public IAsset
	{
	private:
		std::string name_;
		AddonMapEnts* asset_ = nullptr;

		static void dump_triggers(const std::string& name, MapTriggers* trigger);
		static void dump_entityStrings(const std::string& name, char* entityString, int numEntityChars);

		void parse_triggers(ZoneMemory* mem, std::string name, MapTriggers* trigger);
		void parse_entityStrings(ZoneMemory* mem, std::string name, char** entityStrings, int* numEntityChars);

	public:
		AddonMapEnts* parse(std::string name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;

		static void write_triggers(ZoneBuffer* buf, MapTriggers* dest);
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(AddonMapEnts* asset);
	};
}