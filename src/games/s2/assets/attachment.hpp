#pragma once

namespace zonetool::s2
{
	class weapon_attachment : public asset_base<WeaponAttachment, ASSET_TYPE_ATTACHMENT>
	{
	public:
		static void read(zone_reader& reader, WeaponAttachment* asset);

		WeaponAttachment* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(WeaponAttachment* asset);
	};
}
