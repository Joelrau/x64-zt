#pragma once

namespace zonetool::s2
{
	class xmodel_surfs
	{
	public:
		static void read_surfaces(zone_reader& reader, XSurface* surfaces, std::size_t count);
		static void read(zone_reader& reader, XModelSurfs* asset);
	};
}
