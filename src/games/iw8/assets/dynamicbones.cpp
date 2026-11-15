#include <std_include.hpp>
#include "dynamicbones.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_noise(zone_reader& reader, XAnimDynamicBoneNoiseProperties& properties)
		{
			for (auto& axis : properties.axis)
			{
				reader.read_asset(ASSET_TYPE_XANIMCURVE, axis.curve);
			}
		}

		void read_bone(zone_reader& reader, XAnimDynamicBoneProperties& bone)
		{
			reader.read_script_string(bone.boneName);
			reader.read_string(bone.lightHitSound.name);
			reader.read_string(bone.heavyHitSound.name);
			read_noise(reader, bone.noise.translation);
			read_noise(reader, bone.noise.rotation);
		}
	}

	void dynamic_bones::read(zone_reader& reader, XAnimDynamicBones* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto bones = reader.read_array(asset->dynamicBoneProperties, 7, asset->numDynamicBones))
		{
			for (auto i = 0u; i < asset->numDynamicBones; i++)
			{
				read_bone(reader, bones[i]);
			}
		}

		if (const auto bones = reader.read_array(asset->collidableBones, 3, asset->numCollidableBones))
		{
			for (auto i = 0u; i < asset->numCollidableBones; i++)
			{
				reader.read_script_string(bones[i].boneName);
			}
		}

		reader.read_array(asset->collisionShapes, 3, asset->numCollisionShapes);
		reader.read_array(asset->customPinConstraints, 3, asset->numCustomPinConstraints);
		reader.read_script_string_array(asset->pinConstraintBoneNames, asset->numPinConstraintBoneNames);
		reader.pop_stream();
	}

	void anim_curve::read(zone_reader& reader, XAnimCurve* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->controlPoints, 3, asset->numControlPoints);
		reader.pop_stream();
	}
}
