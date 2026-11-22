#pragma once

namespace zonetool::iw8::converter::iw7::xmodel
{
	struct merged_bone
	{
		const XModel* model;
		unsigned int index;
		int parent;
		DObjAnimMat pose;
	};

	struct merged_skeleton
	{
		std::vector<merged_bone> bones;
		std::unordered_map<unsigned int, int> bone_by_name;
		std::unordered_map<const XModel*, std::vector<int>> bone_map;
		std::unordered_map<const XModel*, DObjAnimMat> transforms;
	};

	bool is_supported(const XModel* asset);
	bool dump(XModel* asset);
	bool dump_with_parts(XModel* asset, const std::vector<XModel*>& parts);
}
