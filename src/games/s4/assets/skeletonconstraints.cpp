#include <std_include.hpp>
#include "skeletonconstraints.hpp"

namespace zonetool::s4
{
	void skeleton_constraints::read(zone_reader& reader, SkeletonConstraints* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto constraints = reader.read_array(asset->constraints, 3, asset->numConstraints))
		{
			for (auto i = 0u; i < asset->numConstraints; i++)
			{
				if (constraints[i].type == 0)
				{
					reader.read_script_string(constraints[i].u.unknown0.unknownString);
				}
				else if (constraints[i].type == 1)
				{
					reader.read_script_string(constraints[i].u.unknown1.unknownString);
				}
			}
		}
		reader.pop_stream();
	}
}
