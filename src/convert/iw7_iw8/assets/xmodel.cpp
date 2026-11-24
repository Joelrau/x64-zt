#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"
#include "material.hpp"
#include "havok.hpp"

#include <iw8/assets/xmodel.hpp>
#include <iw8/assets/physics.hpp>

#include <shared.hpp>

namespace zonetool::iw7::converter::iw8::xmodel
{
	namespace
	{
		namespace target = zonetool::iw8;

		target::Material** convert_materials(const XModel* asset, utils::memory::allocator& allocator)
		{
			std::unordered_map<std::string_view, target::Material*> stubs;
			const auto materials = allocator.allocate_array<target::Material*>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (!asset->materialHandles[i])
				{
					continue;
				}

				auto& stub = stubs[asset->materialHandles[i]->name];
				if (!stub)
				{
					stub = allocator.allocate<target::Material>();
					stub->name = asset->materialHandles[i]->name;
				}

				materials[i] = stub;
			}

			return materials;
		}

		target::Material** convert_static_materials(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto materials = allocator.allocate_array<target::Material*>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto name = material::dump_lit(asset->materialHandles[i], techset::GENERATED_TECHSET_STATIC_MODEL);
				if (!name.empty())
				{
					materials[i] = allocator.allocate<target::Material>();
					materials[i]->name = allocator.duplicate_string(name);
				}
			}

			return materials;
		}

		float* convert_himip_radius(const XModel* asset, utils::memory::allocator& allocator)
		{
			if (!asset->invHighMipRadius)
			{
				return nullptr;
			}

			const auto values = allocator.allocate_array<float>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto value = half_float::half_to_float(asset->invHighMipRadius[i]);
				values[i] = value * value;
			}

			return values;
		}

		void convert_lods(const XModel* asset, target::XModel* result, utils::memory::allocator& allocator)
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = result->lodInfo[i];

				new_lod.dist = lod.dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				new_lod.subdivLodValidMask = lod.subdivLodValidMask;
				new_lod.flags = lod.flags;

				if (lod.modelSurfs)
				{
					std::memcpy(new_lod.partBits.array, lod.modelSurfs->partBits, sizeof(new_lod.partBits.array)); // iw7 fills lod partBits at load (0x1403BA0A0)
					new_lod.modelSurfsStaging = allocator.allocate<target::XModelSurfs>();
					new_lod.modelSurfsStaging->name = lod.modelSurfs->name;
				}
			}
		}

		std::uint32_t get_surface_flags(const XModel* asset)
		{
			std::uint32_t flags = xsurface::is_skinned(asset) ? target::XMODEL_FLAG_SKINNED : 0;
			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto* surfs = asset->lodInfo[i].modelSurfs;
				if (!surfs || !surfs->surfs)
				{
					continue;
				}

				for (auto surf = 0; surf < surfs->numsurfs; surf++)
				{
					if (surfs->surfs[surf].rigidVertListCount > 1)
					{
						flags |= target::XMODEL_FLAG_RIGID_VERT_LIST_CHILDREN;
					}
				}
			}

			return flags;
		}

		target::PhysicsAsset* convert_physics_asset(const PhysicsAsset* asset, utils::memory::allocator& allocator)
		{
			if (!asset || !asset->havokData || !asset->havokDataSize)
			{
				return nullptr;
			}

			const auto converted = havok::convert_physics_asset({asset->havokData, asset->havokDataSize}, asset->name);
			if (!converted)
			{
				return nullptr;
			}

			const auto body_count = converted->body_contents.size();
			const auto result = allocator.allocate<target::PhysicsAsset>();
			result->name = asset->name;
			result->havokDataSize = static_cast<unsigned int>(converted->havok_data.size());
			result->havokData = allocator.allocate_array<char>(converted->havok_data.size());
			std::memcpy(result->havokData, converted->havok_data.data(), converted->havok_data.size());
			result->useCategory = converted->dynamic ? target::PhysicsAssetUseCategory_Dynent : target::PhysicsAssetUseCategory_Custom;
			result->numRigidBodies = static_cast<int>(body_count);
			result->numConstraints = 0;
			result->simulationCategories = allocator.allocate_array<target::PhysicsAssetSimulationCategory>(body_count);
			result->bodyContents = allocator.allocate_array<unsigned int>(body_count);
			for (auto i = 0u; i < body_count; i++)
			{
				result->simulationCategories[i] = converted->dynamic
					? target::PhysicsAssetSimulationCategory_DynamicEntity
					: target::PhysicsAssetSimulationCategory_Custom;
				result->bodyContents[i] = converted->body_contents[i];
			}

			result->numSFXEventAssets = static_cast<int>(body_count);
			result->numVFXEventAssets = static_cast<int>(body_count);
			result->sfxEventAssets = allocator.allocate_array<target::PhysicsSFXEventAsset*>(body_count);
			result->vfxEventAssets = allocator.allocate_array<target::PhysicsVFXEventAsset*>(body_count);
			std::memcpy(&result->usageCounter, &asset->physicsUsageCounter, sizeof(result->usageCounter));
			result->containsDynamicBodies = converted->dynamic;
			return result;
		}

		target::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XModel>();

			result->name = asset->name;
			result->numsurfs = asset->numsurfs;
			result->numLods = asset->numLods;
			result->collLod = asset->collLod;
			result->shadowCutoffLod = asset->shadowCutoffLod;
			result->characterCollBoundsType = asset->characterCollBoundsType;
			result->numAimAssistBones = asset->numAimAssistBones;
			result->impactType = asset->impactType;
			result->numBones = asset->numBones;
			result->numRootBones = asset->numRootBones;
			result->numClientBones = asset->numClientBones;
			result->flags = asset->flags & ~(XMODEL_FLAG_REACTIVE_MOTION | XMODEL_FLAG_UNK_0x100 | XMODEL_FLAG_UNK_0x200 // stock iw8 never sets 0x100 or 0x200
				| XMODEL_FLAG_HAS_DEFAULT_MODEL | XMODEL_FLAG_HAS_ANY_DEFAULT_SURFS | XMODEL_FLAG_HAS_ALL_DEFAULT_SURFS);
			result->flags |= get_surface_flags(asset); // TODO: XMODEL_FLAG_STREAMED_IMAGES rule is not known
			result->contents = asset->contents;
			result->scale = asset->scale;
			result->radius = asset->radius;
			std::memcpy(&result->bounds, &asset->bounds, sizeof(result->bounds));
			if (const auto skinned_bounds = xsurface::get_skinned_bounds(asset))
			{
				result->bounds = *skinned_bounds;
			}

			result->edgeLength = asset->edgeLength;
			std::memcpy(&result->physicsUsageCounter, &asset->physicsUsageCounter, sizeof(result->physicsUsageCounter));
			std::memcpy(result->noScalePartBits, asset->noScalePartBits, sizeof(result->noScalePartBits));

			result->aimAssistBones = reinterpret_cast<target::scr_string_t*>(asset->aimAssistBones);
			result->boneNames = reinterpret_cast<target::scr_string_t*>(asset->boneNames);
			result->parentList = asset->parentList;
			result->quats = reinterpret_cast<short*>(asset->tagAngles);
			result->trans = reinterpret_cast<float*>(asset->tagPositions);
			result->partClassification = asset->partClassification;
			result->baseMat = reinterpret_cast<target::DObjAnimMat*>(asset->baseMat);
			result->boneInfo = reinterpret_cast<target::XBoneInfo*>(asset->boneInfo);

			result->materialHandles = convert_materials(asset, allocator);
			convert_lods(asset, result, allocator);
			result->himipRadiusInvSq = convert_himip_radius(asset, allocator);
			return result;
		}
	}

	std::size_t dynamic_body_count(const XModel* asset)
	{
		const auto* physics = asset ? asset->physicsAsset : nullptr;
		if (!physics || !physics->havokData || !physics->havokDataSize)
		{
			return 0;
		}

		const auto converted = havok::convert_physics_asset({physics->havokData, physics->havokDataSize}, physics->name);
		return converted && converted->dynamic ? converted->body_contents.size() : 0;
	}

	target::PhysicsAsset* dump_physics_asset(const PhysicsAsset* asset, utils::memory::allocator& allocator)
	{
		const auto result = convert_physics_asset(asset, allocator);
		if (result && mark_dumped(ASSET_TYPE_PHYSICSASSET, result->name))
		{
			zonetool::iw8::physics_asset::dump(result);
			add_csv_row("physicsasset", result->name);
		}

		return result;
	}

	void dump(XModel* asset)
	{
		if (!mark_dumped(ASSET_TYPE_XMODEL, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		const auto result = convert(asset, allocator);
		result->physicsAsset = dump_physics_asset(asset->physicsAsset, allocator);

		zonetool::iw8::xmodel::dump(result);
		add_csv_row("xmodel", asset->name);

		for (auto i = 0; i < asset->numsurfs; i++)
		{
			material::dump(asset->materialHandles[i]);
		}

		for (auto i = 0; i < asset->numLods; i++)
		{
			xsurface::dump(asset, asset->lodInfo[i].modelSurfs);
		}
	}

	std::string dump_static(XModel* asset)
	{
		const auto name = "tm/"s + asset->name;
		if (!mark_dumped(ASSET_TYPE_XMODEL, name))
		{
			return name;
		}

		utils::memory::allocator allocator;
		const auto result = convert(asset, allocator);
		result->name = name.data();
		result->materialHandles = convert_static_materials(asset, allocator);
		zonetool::iw8::xmodel::dump(result);
		add_csv_row("xmodel", name);

		for (auto i = 0; i < asset->numLods; i++)
		{
			xsurface::dump(asset, asset->lodInfo[i].modelSurfs);
		}

		return name;
	}
}
