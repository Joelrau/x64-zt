#pragma once

namespace zonetool::s2
{
	class xmodel_surfs : public asset_base<XModelSurfs, ASSET_TYPE_XMODEL_SURFS>
	{
	public:
		static void read_surfaces(zone_reader& reader, XSurface* surfaces, std::size_t count);
		static void read(zone_reader& reader, XModelSurfs* asset);

		static void write_surfaces(zone_base* zone, zone_buffer* buf, XSurface* surfaces, XSurface* dest, std::size_t count);
		static void dump_surfaces(assetmanager::dumper& write, XSurface* surfaces, std::size_t count);
		static void parse_surfaces(assetmanager::reader& read, XSurface* surfaces, std::size_t count);
		static void add_shared_dependencies(zone_base* zone, XSurface* surfaces, std::size_t count);

		XModelSurfs* parse(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(XModelSurfs* asset);
	};
}
