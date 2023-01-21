#pragma once
#include "../zonetool.hpp"

namespace zonetool::h1
{
	class ILocalize : public IAsset
	{
	private:
		std::string name_;
		LocalizeEntry* asset_ = nullptr;

	public:
		static bool parse_localizedstrings_json(IZone* zone, const std::string& file_name);
		static bool parse_localizedstrings_file(IZone* zone, const std::string& file_name);
		LocalizeEntry* parse(const std::string& name, ZoneMemory* mem);

		void init(void* asset, ZoneMemory* mem) override;
		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(LocalizeEntry* asset);
	};
}