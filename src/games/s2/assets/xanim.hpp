#pragma once

namespace zonetool::s2
{
	class xanim_parts : public asset_base<XAnimParts, ASSET_TYPE_XANIM>
	{
	public:
		static void read(zone_reader& reader, XAnimParts* asset);

		XAnimParts* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(XAnimParts* asset);
	};
}
