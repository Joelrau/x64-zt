#pragma once
#include "../zonetool.hpp"

namespace zonetool::s1
{
	class IFontDef : public IAsset
	{
	private:
		std::string name_;
		Font_s* asset_ = nullptr;

	public:
		Font_s* parse(const std::string& name, ZoneMemory* mem);

		void init(const std::string& name, ZoneMemory* mem) override;
		void prepare(ZoneBuffer* buf, ZoneMemory* mem) override;
		void load_depending(IZone* zone) override;

		std::string name() override;
		std::int32_t type() override;
		void write(IZone* zone, ZoneBuffer* buffer) override;

		static void dump(Font_s* asset);
	};
}