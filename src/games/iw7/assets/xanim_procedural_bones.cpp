#include <std_include.hpp>
#include "xanim_procedural_bones.hpp"

namespace zonetool::iw7
{
	void xanim_procedural_bones::read(zone_reader& reader, XAnimProceduralBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto constraints = reader.read_array(asset->constraints, 3, asset->numConstraints))
		{
			for (auto i = 0u; i < asset->numConstraints; i++)
			{
				reader.read_script_strings(constraints[i].sourceBoneNames, 2);
			}
		}

		reader.read_script_string_array(asset->targetBoneNames, asset->numTargetBones);

		if (const auto entries = reader.read_array(asset->unk01, 7, asset->unk01_count))
		{
			for (auto i = 0u; i < asset->unk01_count; i++)
			{
				reader.read_script_string(entries[i].unk01);
				reader.read_string(entries[i].unk02);
				reader.read_string(entries[i].unk03);
			}
		}

		if (const auto entries = reader.read_array(asset->unk02, 3, asset->unk02_count))
		{
			for (auto i = 0u; i < asset->unk02_count; i++)
			{
				reader.read_script_string(entries[i].unk01);
			}
		}

		reader.read_array(asset->unk03, 3, asset->unk03_count);
		reader.pop_stream();
	}
}
