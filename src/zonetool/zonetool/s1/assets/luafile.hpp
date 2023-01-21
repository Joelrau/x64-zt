#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	class ILuaFile : public IAsset
	{
	private:
		std::string name_;
		LuaFile* asset_ = nullptr;

	public:
		LuaFile* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(LuaFile* asset);
	};
}