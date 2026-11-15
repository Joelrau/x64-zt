#include <std_include.hpp>
#include "proceduralbones.hpp"

namespace zonetool::iw8
{
	void procedural_bones::read(zone_reader& reader, XAnimProceduralBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto constraints = reader.read_array(asset->constraints, 3, asset->numConstraints))
		{
			for (auto i = 0u; i < asset->numConstraints; i++)
			{
				reader.read_script_strings(constraints[i].sourceBoneNames, std::size(constraints[i].sourceBoneNames));
			}
		}

		reader.read_script_string_array(asset->targetBoneNames, asset->numTargetBones);
		reader.pop_stream();
	}
}
