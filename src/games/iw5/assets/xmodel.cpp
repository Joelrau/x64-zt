#include <std_include.hpp>
#include "xmodel.hpp"

namespace zonetool::iw5
{
	void xmodel::read(zone_reader32& reader, XModel* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		const auto bone_count = asset->numBones;
		const auto child_count = asset->numBones - asset->numRootBones;

		if (const auto names = reader.read_array(asset->boneNames, 1, bone_count))
		{
			reader.read_script_strings(names, bone_count);
		}

		reader.read_array(asset->parentList, 0, child_count);
		reader.read_array(asset->quats, 1, child_count);
		reader.read_array(asset->trans, 3, child_count);
		reader.read_array(asset->partClassification, 0, bone_count);
		reader.read_array(asset->baseMat, 3, bone_count);
		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materialHandles, asset->numsurfs);

		for (auto& lod : asset->lodInfo)
		{
			reader.read_asset(ASSET_TYPE_XMODEL_SURFS, lod.modelSurfs);
		}

		if (const auto surfaces = reader.read_array(asset->collSurfs, 3, asset->numCollSurfs))
		{
			for (auto i = 0; i < asset->numCollSurfs; i++)
			{
				reader.read_array(surfaces[i].collTris, 3, surfaces[i].numCollTris);
			}
		}

		reader.read_array(asset->boneInfo, 3, bone_count);
		reader.read_asset(ASSET_TYPE_PHYSPRESET, asset->physPreset);
		reader.read_asset(ASSET_TYPE_PHYSCOLLMAP, asset->physCollmap);
		reader.pop_stream();
	}
}
