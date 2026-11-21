#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"
#include "material.hpp"

#include <iw8/assets/xmodel.hpp>

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

	void dump(XModel* asset)
	{
		if (!mark_dumped(ASSET_TYPE_XMODEL, asset->name))
		{
			return;
		}

		utils::memory::allocator allocator;
		zonetool::iw8::xmodel::dump(convert(asset, allocator));
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
