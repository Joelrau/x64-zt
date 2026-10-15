#pragma once

namespace zonetool::s2
{
	class techset : public asset_base<MaterialTechniqueSet, ASSET_TYPE_TECHNIQUE_SET>
	{
	public:
		static void read(zone_reader& reader, MaterialTechniqueSet* asset);

		MaterialTechniqueSet* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(MaterialTechniqueSet* asset);
	};
}
