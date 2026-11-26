#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class ddl_file
	{
	public:
		static void read(zone_reader& reader, DDLFile* asset);
	};
}
