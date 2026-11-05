#pragma once

namespace zonetool::s2
{
	class path_data : public asset_base<PathData, ASSET_TYPE_PATHDATA>
	{
	public:
		static void read(zone_reader& reader, PathData* asset);
		PathData* parse(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(PathData* asset);
	};
}
