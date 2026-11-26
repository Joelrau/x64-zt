#include <std_include.hpp>
#include "xskeleton.hpp"

namespace zonetool::t9
{
	void xskeleton::read(auto& reader, XSkeleton* asset)
	{
		const auto total_bones = asset->numBones + asset->numExtraBones;
		const auto child_bones = total_bones - asset->numRootBones;

		reader.read_array(asset->boneNames, 3, total_bones);
		reader.read_array(asset->extraBones, 3, asset->numExtraBones);
		reader.read_array(asset->parentList, 1, child_bones);
		reader.read_array(asset->quats, 7, child_bones);
		reader.read_array(asset->trans, 3, child_bones);
		reader.read_array(asset->partClassification, 0, total_bones);
		reader.read_array(asset->baseMat, 15, total_bones);
		reader.read_array(asset->extraBoneInfo, 3, asset->numExtraBones);
	}
}

namespace zonetool::t9
{
	template void xskeleton::read(zone_reader&, XSkeleton*);
	template void xskeleton::read(zone_writer&, XSkeleton*);
}
