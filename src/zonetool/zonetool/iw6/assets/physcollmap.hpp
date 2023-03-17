#pragma once
#include "../zonetool.hpp"

namespace zonetool::iw6
{
	class IPhysCollmap : public IAsset
	{
	private:
		std::string name_;
		PhysCollmap* asset_ = nullptr;

	public:
		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(PhysCollmap* asset);
	};
}