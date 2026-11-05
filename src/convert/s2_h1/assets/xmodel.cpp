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

		template <typename T>
		T* make_stub(const char* name, utils::memory::allocator& allocator)
		{
			const auto stub = allocator.allocate<T>();
			stub->name = name;
			return stub;
		}

		const XModel* resolve_variant(const XModel* asset)
		{
			if (!(asset->flags & XMODEL_FLAG_VARIANTS))
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
					if (lod.modelSurfs->surfs[i].flags & XSURFACE_FLAG_SKINNED)
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

			const auto coll_surfs = reinterpret_cast<const XModelCollSurf*>(base->collSurfs);
			const auto new_coll_surfs = allocator.allocate_array<zonetool::h1::XModelCollSurf_s>(base->collSurfCount);
			for (auto i = 0; i < base->collSurfCount; i++)
			{
				auto& surf = new_coll_surfs[i];
				surf.bounds = std::bit_cast<zonetool::h1::Bounds>(coll_surfs[i].bounds);
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

		std::optional<std::uint32_t> get_forced_vertex_color_bits(const Material* material)
		{
			if (!material || !material->techniqueSet || !material->techniqueSet->name)
			{
				return {};
			}

			const auto techset = material->techniqueSet->name;
			if (std::strstr(techset, "_fwa"))
			{
				return 0x00FFFFFFu;
			}

			if (std::strstr(techset, "_effect") || std::strstr(techset, "_flag"))
			{
				return 0u;
			}

			return {};
		}

		int get_lod_count(const XModel* asset, const XModelBase* base)
		{
			const auto skip_last = (asset->flags & XMODEL_FLAG_NO_LAST_LOD) && base->numLods > 1;
			return std::min(base->numLods - (skip_last ? 1 : 0), 6);
		}

		void dump_lod_surfs(const XModel* asset)
		{
			const auto source = resolve_variant(asset);
			const auto lod_count = get_lod_count(asset, source->base);
			auto vertex_lighting_index = 0u;
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = source->base->lodInfo[i];
				std::unordered_map<int, std::uint32_t> vertex_color_surfs;
				for (auto j = 0; j < lod.numsurfs; j++)
				{
					const auto forced_bits = source->materialHandles ? get_forced_vertex_color_bits(source->materialHandles[lod.surfIndex + j]) : std::nullopt;
					if (forced_bits)
					{
						vertex_color_surfs.emplace(j, *forced_bits);
					}
				}

				xsurface::dump(lod.modelSurfs, vertex_color_surfs, vertex_lighting_index);
				for (auto j = 0; j < lod.modelSurfs->numsurfs; j++)
				{
					vertex_lighting_index += lod.modelSurfs->surfs[j].vertCount;
				}
			}
		}

		float get_lod_dist(const XModelBase* base, const int lod, const int lod_count)
		{
			if (lod >= lod_count - 1)
			{
				return 1000000.0f;
			}

			return std::sqrt(std::max(0.0f, base->lodDistScale * base->lodDistSq[lod])) * 565.087f + base->radius;
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

			const auto lod_count = get_lod_count(asset, base);
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = base->lodInfo[i];
				auto& new_lod = new_asset->lodInfo[i];
				new_lod.dist = get_lod_dist(base, i, lod_count);
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

			new_asset->numLods = static_cast<char>(lod_count);
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
			dump_lod_surfs(asset);
			add_csv_row("xmodel", asset->name);
			dumped_models.insert(asset->name);
		}
	}
}
