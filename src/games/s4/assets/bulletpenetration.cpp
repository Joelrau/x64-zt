#include <std_include.hpp>
#include "bulletpenetration.hpp"

namespace zonetool::s4
{
	void bullet_penetration::read(zone_reader& reader, BulletPenetration* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.pop_stream();
	}
}
