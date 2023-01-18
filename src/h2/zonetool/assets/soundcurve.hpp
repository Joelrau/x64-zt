#pragma once
#include "../zonetool.hpp"

namespace zonetool::h2
{
	class ISoundCurve : public IAsset
	{
	private:
		std::string name_;
		SndCurve* asset_ = nullptr;

	public:
		static SndCurve* parse(const std::string& name, const std::string& type, ZoneMemory* mem);
		SndCurve* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(SndCurve* asset, const std::string& type);
		static void dump(SndCurve* asset);
	};
}