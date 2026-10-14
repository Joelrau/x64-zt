#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"

#include <h1/assets/xmodel.hpp>

namespace zonetool::s2::converter::h1::xmodel
{
	namespace
	{
		std::unordered_set<std::string> dumped_models;

		constexpr unsigned int s2_xmodel_flag_variants = 0x200;
		constexpr std::uint16_t s2_surf_flag_skinned = 0x4;

		struct s2_coll_surf
		{
			float midPoint[3];
			float halfSize[3];
			int boneIdx;
			int __pad0;
			std::uint64_t contents;
			std::uint32_t surfFlags;
			int __pad1;
		};
		static_assert(sizeof(s2_coll_surf) == 48);

		template <typename T>
		T* make_stub(const char* name, utils::memory::allocator& allocator)
		{
			const auto stub = allocator.allocate<T>();
			stub->name = name;
			return stub;
		}

		const XModel* resolve_variant(const XModel* asset)
		{
			if (!(asset->flags & s2_xmodel_flag_variants))
			{
				return asset;
			}

			if (!asset->variants || !asset->variantCount || !asset->variants[0].model)
			{
				return nullptr;
			}

			return resolve_variant(asset->variants[0].model);
		}

		bool is_skinned(const XModelBase* base)
		{
			for (const auto& lod : base->lodInfo)
			{
				if (!lod.modelSurfs || !lod.modelSurfs->surfs)
				{
					continue;
				}

				for (auto i = 0; i < lod.modelSurfs->numsurfs; i++)
				{
					if (lod.modelSurfs->surfs[i].flags & s2_surf_flag_skinned)
					{
						return true;
					}
				}
			}

			return false;
		}

		zonetool::h1::XModelCollSurf_s* convert_coll_surfs(const XModelBase* base, utils::memory::allocator& allocator)
		{
			if (!base->collSurfs || !base->collSurfCount)
			{
				return nullptr;
			}

			const auto coll_surfs = reinterpret_cast<const s2_coll_surf*>(base->collSurfs);
			const auto new_coll_surfs = allocator.allocate_array<zonetool::h1::XModelCollSurf_s>(base->collSurfCount);
			for (auto i = 0; i < base->collSurfCount; i++)
			{
				auto& surf = new_coll_surfs[i];
				std::memcpy(surf.bounds.midPoint, coll_surfs[i].midPoint, sizeof(surf.bounds.midPoint));
				std::memcpy(surf.bounds.halfSize, coll_surfs[i].halfSize, sizeof(surf.bounds.halfSize));
				surf.boneIdx = coll_surfs[i].boneIdx;
				surf.contents = static_cast<int>(coll_surfs[i].contents);
				surf.surfFlags = static_cast<int>(convert_surface_flags(coll_surfs[i].surfFlags));
			}

			return new_coll_surfs;
		}

		unsigned short* make_inv_high_mip_radius(const float radius, const unsigned char numsurfs, utils::memory::allocator& allocator)
		{
			const auto inv_radius = half_float::float_to_half(radius > 0.0f ? 1.0f / radius : 1.0f);
			const auto values = allocator.allocate_array<unsigned short>(numsurfs);
			std::fill_n(values, numsurfs, inv_radius);
			return values;
		}

		zonetool::h1::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto source = resolve_variant(asset);
			if (!source || !source->base)
			{
				ZONETOOL_WARNING("XModel \"%s\" has no base", asset->name);
				return nullptr;
			}

			const auto base = source->base;
			const auto bone_count = base->numBones + base->numClientBones;
			const auto child_bone_count = bone_count - base->numRootBones;
			if (bone_count > std::numeric_limits<unsigned char>::max())
			{
				ZONETOOL_WARNING("XModel \"%s\" has too many bones (%d)", asset->name, bone_count);
				return nullptr;
			}

			const auto new_asset = allocator.allocate<zonetool::h1::XModel>();
			new_asset->name = asset->name;
			new_asset->numBones = static_cast<unsigned char>(bone_count);
			new_asset->numRootBones = static_cast<unsigned char>(base->numRootBones);
			new_asset->numsurfs = source->numMaterials;
			new_asset->lodRampType = is_skinned(base) ? zonetool::h1::XMODEL_LOD_RAMP_SKINNED : zonetool::h1::XMODEL_LOD_RAMP_RIGID;
			new_asset->scale = 1.0f;

			new_asset->boneNames = reinterpret_cast<zonetool::h1::scr_string_t*>(base->boneNames);
			new_asset->parentList = allocator.allocate_array<unsigned char>(child_bone_count);
			for (auto i = 0; i < child_bone_count; i++)
			{
				new_asset->parentList[i] = static_cast<unsigned char>(base->parentList[i]);
			}

			new_asset->tagAngles = reinterpret_cast<zonetool::h1::XModelAngle*>(base->tagAngles);
			new_asset->tagPositions = reinterpret_cast<zonetool::h1::XModelTagPos*>(base->tagPositions);
			new_asset->partClassification = base->partClassification;
			new_asset->baseMat = reinterpret_cast<zonetool::h1::DObjAnimMat*>(base->baseMat);

			new_asset->materialHandles = allocator.allocate_array<zonetool::h1::Material*>(new_asset->numsurfs);
			for (auto i = 0; i < new_asset->numsurfs; i++)
			{
				const auto material = source->materialHandles ? source->materialHandles[i] : nullptr;
				new_asset->materialHandles[i] = make_stub<zonetool::h1::Material>(material ? material->name : "$default", allocator);
			}

			for (auto i = 0; i < base->numLods && i < 6; i++)
			{
				const auto& lod = base->lodInfo[i];
				auto& new_lod = new_asset->lodInfo[i];
				new_lod.dist = lod.dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				if (!lod.modelSurfs || !xsurface::is_dumped(lod.modelSurfs->name))
				{
					ZONETOOL_WARNING("XModel \"%s\" uses XModelSurfs that are not converted", asset->name);
					return nullptr;
				}

				std::memcpy(new_lod.partBits, lod.modelSurfs->partBits, sizeof(new_lod.partBits));
				new_lod.modelSurfs = make_stub<zonetool::h1::XModelSurfs>(lod.modelSurfs->name, allocator);
			}

			new_asset->numLods = static_cast<char>(base->numLods);
			new_asset->collLod = base->collLod;
			new_asset->numCollSurfs = base->collSurfCount;
			new_asset->collSurfs = convert_coll_surfs(base, allocator);
			new_asset->contents = base->contents;
			new_asset->boneInfo = reinterpret_cast<zonetool::h1::XBoneInfo*>(base->boneInfo);
			new_asset->radius = base->radius;
			std::memcpy(new_asset->bounds.midPoint, base->boundsMidPoint, sizeof(new_asset->bounds.midPoint));
			std::memcpy(new_asset->bounds.halfSize, base->boundsHalfSize, sizeof(new_asset->bounds.halfSize));
			new_asset->invHighMipRadius = make_inv_high_mip_radius(base->radius, new_asset->numsurfs, allocator);

			return new_asset;
		}
	}

	bool is_dumped(const std::string& name)
	{
		return dumped_models.contains(name);
	}

	void dump(XModel* asset)
	{
		utils::memory::allocator allocator;
		if (const auto converted = convert(asset, allocator))
		{
			zonetool::h1::xmodel::dump(converted);
			add_csv_row("xmodel", asset->name);
			dumped_models.insert(asset->name);
		}
	}
}
