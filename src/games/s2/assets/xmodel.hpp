#pragma once

namespace zonetool::s2
{
	class xmodel_base
	{
	public:
		static void read(zone_reader& reader, XModelBase* asset);
	};

	class xmodel
	{
	public:
		static void read(zone_reader& reader, XModel* asset);
	};
}
