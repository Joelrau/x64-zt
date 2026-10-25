#include <std_include.hpp>

#include "../include.hpp"
#include "xanim.hpp"

namespace zonetool::h1::converter::s2::xanim
{
	namespace
	{
		std::unordered_set<std::string> dumped_anims;

		// same 10 part types, delta parts and frame data as h1, s2 widens the bone counts and flags to u16
		zonetool::s2::XAnimParts* convert(XAnimParts* asset, utils::memory::allocator& allocator)
		{
			const auto new_asset = allocator.allocate<zonetool::s2::XAnimParts>();
			new_asset->name = asset->name;
			new_asset->dataByteCount = asset->dataByteCount;
			new_asset->dataShortCount = asset->dataShortCount;
			new_asset->dataIntCount = asset->dataIntCount;
			new_asset->numframes = asset->numframes;
			new_asset->flags = asset->flags;
			for (auto i = 0; i <= zonetool::s2::PART_TYPE_ALL; i++)
			{
				new_asset->boneCount[i] = asset->boneCount[i];
			}

			new_asset->notifyCount = asset->notifyCount;
			new_asset->assetType = asset->assetType;
			new_asset->ikType = asset->ikType;
			new_asset->randomDataByteCount = asset->randomDataByteCount;
			new_asset->randomDataShortCount = asset->randomDataShortCount;
			new_asset->randomDataIntCount = asset->randomDataIntCount;
			new_asset->indexCount = asset->indexCount;
			new_asset->framerate = asset->framerate;
			new_asset->frequency = asset->frequency;
			new_asset->names = retype<zonetool::s2::scr_string_t>(asset->names);
			new_asset->dataByte = asset->dataByte;
			new_asset->dataShort = asset->dataShort;
			new_asset->dataInt = asset->dataInt;
			new_asset->randomDataShort = asset->randomDataShort;
			new_asset->randomDataByte = asset->randomDataByte;
			new_asset->randomDataInt = asset->randomDataInt;
			new_asset->indices.data = asset->indices.data;
			new_asset->notify = retype<zonetool::s2::XAnimNotifyInfo>(asset->notify);
			new_asset->deltaPart = retype<zonetool::s2::XAnimDeltaPart>(asset->deltaPart);
			new_asset->secondaryName = asset->secondaryName;

			// h1 stores compressed blend shape keys, s2 a weight per frame
			if (asset->blendShapeCount)
			{
				ZONETOOL_WARNING("xanim \"%s\": dropping %u blend shapes, h1 blend shape keys are not converted", asset->name, asset->blendShapeCount);
			}

			return new_asset;
		}
	}

	void dump(XAnimParts* asset)
	{
		if (!dumped_anims.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::s2::xanim_parts::dump(convert(asset, allocator));
		add_csv_row(zonetool::s2::type_to_string(zonetool::s2::ASSET_TYPE_XANIM), asset->name);
	}

	bool dump_used(const std::string& name)
	{
		if (dumped_anims.contains(name))
		{
			return true;
		}

		const auto asset = db_find_x_asset_header_safe(ASSET_TYPE_XANIMPARTS, name).parts;
		if (!asset)
		{
			return false;
		}

		dump(asset);
		return true;
	}

	void clear()
	{
		dumped_anims.clear();
	}
}
