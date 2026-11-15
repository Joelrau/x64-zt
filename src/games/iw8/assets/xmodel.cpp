#include <std_include.hpp>
#include "xmodel.hpp"

namespace zonetool::iw8
{
	namespace
	{
		void read_bones(zone_reader& reader, XModel* asset)
		{
			const auto bone_count = asset->numBones + asset->numClientBones;
			const auto child_count = bone_count - asset->numRootBones;

			reader.read_script_string_array(asset->aimAssistBones, asset->numAimAssistBones);
			reader.read_script_string_array(asset->boneNames, bone_count);
			reader.read_array(asset->parentList, 0, child_count);
			reader.read_array(asset->quats, 1, 4 * child_count);
			reader.read_array(asset->trans, 3, 3 * child_count);
			reader.read_array(asset->partClassification, 0, asset->numBones);
			reader.read_array(asset->baseMat, 15, bone_count);
			reader.read_array(asset->ikHingeAxis, 3, 4);
		}

		void read_reactive_motion(zone_reader& reader, ReactiveMotionModelInfo*& field)
		{
			if (const auto info = reader.read_single(field, 15))
			{
				reader.read_array(info->modelPartTweaks, 15, info->numParts);
				reader.read_array(info->modelParts, 15, info->numParts);
			}
		}

		void read_blend_shapes(zone_reader& reader, XModelBlendShapeInfo*& field)
		{
			if (const auto info = reader.read_single(field, 7))
			{
				reader.read_script_string_array(info->weightNames, info->numberOfWeights);
				reader.read_array(info->weightMaps, 3, info->numberOfWeightMaps);
			}
		}

		void read_mdao_volumes(zone_reader& reader, XModel* asset)
		{
			if (const auto volumes = reader.read_array(asset->mdaoVolumes, 7, asset->mdaoVolumeCount))
			{
				for (auto i = 0; i < asset->mdaoVolumeCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, volumes[i].volumeData);
				}
			}
		}

		void read_decal_volumes(zone_reader& reader, XModelDecalVolumesInfo*& field)
		{
			const auto info = reader.read_single(field, 7);
			if (!info)
			{
				return;
			}

			if (const auto volumes = reader.read_array(info->decalVolumes, 7, info->numDecalVolumes))
			{
				for (auto i = 0u; i < info->numDecalVolumes; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, volumes[i].material);
					reader.read_asset(ASSET_TYPE_DECAL_VOLUME_MASK, volumes[i].blendMapOverride);
				}
			}
		}
	}

	void xmodel::read(zone_reader& reader, XModel* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptableMoverDef);
		reader.read_asset(ASSET_TYPE_XANIM_PROCEDURALBONES, asset->proceduralBones);
		reader.read_asset(ASSET_TYPE_XANIM_DYNAMICBONES, asset->dynamicBones);
		read_bones(reader, asset);
		read_reactive_motion(reader, asset->reactiveMotionInfo);
		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materialHandles, asset->numsurfs);

		for (auto& lod : asset->lodInfo)
		{
			reader.read_asset(ASSET_TYPE_XMODEL_SURFS, lod.modelSurfsStaging);
		}

		reader.read_array(asset->boneInfo, 3, asset->numBones + asset->numClientBones);
		reader.read_array(asset->himipRadiusInvSq, 3, asset->numsurfs);
		reader.read_asset(ASSET_TYPE_PHYSICSASSET, asset->physicsAsset);
		reader.read_asset(ASSET_TYPE_PHYSICS_FX_SHAPE, asset->physicsFXShape);
		reader.read_asset(ASSET_TYPE_XMODEL_DETAIL_COLLISION, asset->detailCollision);
		reader.read_asset_array(ASSET_TYPE_CLOTH_ASSET, asset->clothAssets, asset->numClothAssets);
		read_blend_shapes(reader, asset->blendShapeInfo);
		read_mdao_volumes(reader, asset);
		read_decal_volumes(reader, asset->decalVolumesInfo);
		reader.pop_stream();
	}
}
