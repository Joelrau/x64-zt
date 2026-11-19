#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class ddl_file
	{
	public:
		static void read(zone_reader& reader, DDLFile* asset);
	};
}
