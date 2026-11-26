#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "material.hpp"
#include "physics.hpp"
#include "xsurface.hpp"

#include <iw8/assets/xmodel.hpp>

namespace zonetool::s4::converter::iw8::xmodel
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::Material** convert_materials(const XModel* asset, const unsigned short count, utils::memory::allocator& allocator)
		{
			if (!asset->materialHandles)
			{
				return nullptr;
			}

			const auto materials = allocator.allocate_array<target::Material*>(count);
			for (auto i = 0; i < count; i++)
			{
				materials[i] = material::reference(asset->materialHandles[i], allocator);
			}

			return materials;
		}

		unsigned char* convert_parent_list(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto count = asset->numBones + asset->numClientBones - asset->numRootBones;
			if (!asset->parentList || count <= 0)
			{
				return nullptr;
			}

			const auto parents = allocator.allocate_array<unsigned char>(count);
			for (auto i = 0; i < count; i++)
			{
				parents[i] = static_cast<unsigned char>(asset->parentList[i]);
			}

			return parents;
		}

		void convert_lods(const XModel* asset, target::XModel* result, utils::memory::allocator& allocator)
		{
			if (asset->numLods > std::size(result->lodInfo))
			{
				ZONETOOL_WARNING("XModel \"%s\" has %d LODs, IW8 gets the first %zu with the last distance",asset->name, asset->numLods, std::size(result->lodInfo));
			}

			result->numLods = static_cast<unsigned char>(std::min<std::size_t>(asset->numLods, std::size(result->lodInfo)));
			result->numsurfs = asset->numsurfs;
			if (asset->lodInfo && result->numLods < asset->numLods)
			{
				result->numsurfs = asset->lodInfo[result->numLods].surfIndex;
			}

			for (auto i = 0; asset->lodInfo && i < result->numLods; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = result->lodInfo[i];

				new_lod.dist = lod.dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				std::memcpy(new_lod.partBits.array, lod.partBits.array, sizeof(new_lod.partBits.array));
				new_lod.modelSurfsStaging = name_reference<target::XModelSurfs>(lod.modelSurfsStaging, allocator);
			}

			if (asset->lodInfo && result->numLods < asset->numLods)
			{
				result->lodInfo[result->numLods - 1].dist = asset->lodInfo[asset->numLods - 1].dist;
			}
		}

		target::ReactiveMotionModelInfo* convert_reactive_motion(const ReactiveMotionModelInfo* info, utils::memory::allocator& allocator)
		{
			static_assert(sizeof(target::ReactiveMotionModelInfo) == sizeof(ReactiveMotionModelInfo));
			static_assert(sizeof(target::ReactiveMotionModelPartTweaks) == sizeof(ReactiveMotionModelPartTweaks));
			static_assert(sizeof(target::ReactiveMotionModelPart) == sizeof(ReactiveMotionModelPart));

			if (!info)
			{
				return nullptr;
			}

			const auto result = allocator.allocate<target::ReactiveMotionModelInfo>();
			std::memcpy(result, info, sizeof(*result));
			result->modelPartTweaks = reinterpret_cast<target::ReactiveMotionModelPartTweaks*>(info->modelPartTweaks);
			result->modelParts = reinterpret_cast<target::ReactiveMotionModelPart*>(info->modelParts);
			return result;
		}

		void set_surface_layer_masks(const XModel* asset)
		{
			for (auto i = 0; asset->lodInfo && asset->materialHandles && i < asset->numLods; i++)
			{
				const auto& lod = asset->lodInfo[i];
				if (!lod.modelSurfsStaging)
				{
					continue;
				}

				std::vector<bool> layer_masks(lod.numsurfs);
				for (auto j = 0; j < lod.numsurfs && lod.surfIndex + j < asset->numsurfs; j++)
				{
					layer_masks[j] = material::uses_vertex_layer_mask(asset->materialHandles[lod.surfIndex + j]);
				}

				xsurface::set_layer_masks(lod.modelSurfsStaging, std::move(layer_masks));
			}
		}
	}

	target::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
	{
		static_assert(sizeof(target::DObjAnimMat) == sizeof(DObjAnimMat));
		static_assert(sizeof(target::XBoneInfo) == sizeof(XBoneInfo));

		if (asset->numBones + asset->numClientBones > 0xFF)
		{
			ZONETOOL_ERROR("XModel \"%s\" has more than 255 bones", asset->name);
			return nullptr;
		}

		const auto result = allocator.allocate<target::XModel>();
		result->name = asset->name;
		result->collLod = asset->unknown[0];
		result->shadowCutoffLod = 6;
		result->numAimAssistBones = asset->numAimAssistBones;
		result->numBones = asset->numBones;
		result->numRootBones = asset->numRootBones;
		result->numClientBones = asset->numClientBones;
		result->flags = asset->flags;
		result->contents = physics::convert_contents(asset->contents);
		result->scale = asset->scale;
		result->radius = asset->radius;
		std::memcpy(&result->bounds, &asset->bounds, sizeof(result->bounds));
		result->edgeLength = asset->edgeLength;
		result->lgvData = asset->lgvData;
		std::memcpy(&result->physicsUsageCounter, &asset->physicsUsageCounter, sizeof(result->physicsUsageCounter));
		if (asset->noScalePartBits)
		{
			std::memcpy(result->noScalePartBits, asset->noScalePartBits, sizeof(result->noScalePartBits));
		}

		result->aimAssistBones = reinterpret_cast<target::scr_string_t*>(asset->aimAssistBones);
		result->boneNames = reinterpret_cast<target::scr_string_t*>(asset->boneNames);
		result->parentList = convert_parent_list(asset, allocator);
		result->quats = asset->quats;
		result->trans = asset->trans;
		result->partClassification = asset->partClassification;
		result->baseMat = reinterpret_cast<target::DObjAnimMat*>(asset->baseMat);
		result->boneInfo = reinterpret_cast<target::XBoneInfo*>(asset->boneInfo);
		result->himipRadiusInvSq = asset->himipRadiusInvSq;
		result->reactiveMotionInfo = convert_reactive_motion(asset->reactiveMotionInfo, allocator);

		convert_lods(asset, result, allocator);
		result->materialHandles = convert_materials(asset, result->numsurfs, allocator);
		if (asset->physicsType != XMODEL_PHYSICS_RAGDOLL)
		{
			result->physicsAsset = name_reference<target::PhysicsAsset>(asset->physicsAsset, allocator);
		}
		if (asset->detailCollision && asset->detailCollision->physicsLODDataNameCount > 1)
		{
			ZONETOOL_WARNING("xmodel \"%s\": detail collision has %u bodies, IW8 scriptables support 1, the physics asset is used", asset->name,
				asset->detailCollision->physicsLODDataNameCount);
		}
		else
		{
			result->detailCollision = name_reference<target::XModelDetailCollision>(asset->detailCollision, allocator);
		}

		return result;
	}

	void dump(XModel* asset)
	{
		utils::memory::allocator allocator;
		if (const auto converted = convert(asset, allocator))
		{
			set_surface_layer_masks(asset);
			target::xmodel::dump(converted);
			add_csv_row(target::type_to_string(target::ASSET_TYPE_XMODEL), asset->name);
		}
	}
}
