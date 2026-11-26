#include <std_include.hpp>

#include "../include.hpp"
#include "xanim.hpp"

namespace zonetool::s4::converter::iw8::xanim
{
	namespace target = zonetool::iw8;

	target::XAnimParts* convert(const XAnimParts* asset, utils::memory::allocator& allocator)
	{
		static_assert(sizeof(target::XAnimIndices) == sizeof(XAnimIndices));
		static_assert(sizeof(target::XAnimDeltaPart) == sizeof(XAnimDeltaPart));

		const auto result = allocator.allocate<target::XAnimParts>();
		result->name = asset->name;
		result->names = asset->names;
		result->dataByte = asset->dataByte;
		result->dataShort = asset->dataShort;
		result->dataInt = asset->dataInt;
		result->randomDataShort = asset->randomDataShort;
		result->randomDataByte = asset->randomDataByte;
		result->randomDataInt = asset->randomDataInt;
		std::memcpy(&result->indices, &asset->indices, sizeof(result->indices));
		result->deltaPart = reinterpret_cast<target::XAnimDeltaPart*>(asset->deltaPart);
		result->randomDataShortCount = asset->randomDataShortCount;
		result->randomDataByteCount = asset->randomDataByteCount;
		result->indexCount = asset->indexCount;
		result->framerate = asset->framerate;
		result->frequency = asset->frequency;
		result->dataByteCount = asset->dataByteCount;
		result->dataShortCount = asset->dataShortCount;
		result->dataIntCount = asset->dataIntCount;
		result->randomDataIntCount = asset->randomDataIntCount;
		result->numframes = asset->numframes;
		result->flags = asset->flags;
		std::memcpy(result->boneCount, asset->boneCount, sizeof(result->boneCount));
		result->notifyCount = asset->notifyCount;
		result->assetType = asset->assetType;
		result->ikType = asset->ikType;
		result->fingerPoseType = asset->fingerPoseType;
		result->blendShapeWeightCount = asset->blendShapeWeightCount;
		result->blendShapeWeightNames = asset->blendShapeWeightNames;
		result->blendShapeWeights = asset->blendShapeWeights;

		if (asset->notify)
		{
			result->notify = allocator.allocate_array<target::XAnimNotifyInfo>(asset->notifyCount);
			for (auto i = 0u; i < asset->notifyCount; i++)
			{
				result->notify[i].name = asset->notify[i].name;
				result->notify[i].time = asset->notify[i].time;
			}
		}

		if (asset->extraChannels && asset->extraChannels->nameCount)
		{
			ZONETOOL_WARNING("Anim \"%s\" drops %d extra channels, IW8 has none", asset->name, asset->extraChannels->nameCount);
		}

		return result;
	}

	void dump(XAnimParts* asset)
	{
		utils::memory::allocator allocator;
		target::xanim_parts::dump(convert(asset, allocator));
		add_csv_row(target::type_to_string(target::ASSET_TYPE_XANIMPARTS), asset->name);
	}
}
