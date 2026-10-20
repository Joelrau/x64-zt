#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"

// 1: switch lods at the h1 lodInfo[].dist (useLodInfoDist, read as plain world distances by sub_14074D7B0)
// 0: switch lods at the lodDistSq the s2 linker bakes from triangle sizes like stock models
#ifndef H1_S2_USE_LOD_INFO_DIST
#define H1_S2_USE_LOD_INFO_DIST 1
#endif

namespace zonetool::h1::converter::s2::xmodel
{
	namespace
	{
		// the values every stock model without its own tuning has (mp_aachen_v2, 958 xmodelbases)
		constexpr auto no_cull_lod_dist = 1000000.0f; // lodInfo[].dist of all 6 lods when useLodInfoDist is 0
		constexpr auto automatic_forced_lod = 0xFE;
		constexpr auto default_cull_size_scale = 15.0f;
		constexpr auto default_unk_0x374 = 96.0f;
		constexpr auto lightmap_uv_model_flags = 0x6000u; // stock models with lightmap uv surfs (0x6800 instead of 0x800)

		// the sway of stock s2 weeds (foliage_s2_cob_weeds_07), h1 reactive motion grass gets it with the "fwa" wind techsets
		constexpr std::array<float, 4> grass_sway_params{2000.0f, 40.0f, 0.055f, 1.5f};

		bool has_surface_flag(const XModel* asset, const unsigned int flag)
		{
			for (auto lod = 0; lod < std::min(static_cast<int>(asset->numLods), 6); lod++)
			{
				const auto* surfs = asset->lodInfo[lod].modelSurfs;
				for (auto i = 0; surfs && surfs->surfs && i < surfs->numsurfs; i++)
				{
					if (surfs->surfs[i].flags & flag)
					{
						return true;
					}
				}
			}

			return false;
		}

		bool has_lightmap_uv_surfaces(const XModel* asset)
		{
			return has_surface_flag(asset, SURF_FLAG_LIGHTMAP_COORDS);
		}

		struct converted_model
		{
			std::string base_name;
			zonetool::s2::XModelBase base;
			zonetool::s2::XModel model;
			std::vector<std::uint16_t> parent_list;
			std::vector<zonetool::s2::XModelCollSurf> coll_surfs;
			std::deque<zonetool::s2::XModelSurfs> surfs;
			std::deque<zonetool::s2::Material> materials;
			std::vector<zonetool::s2::Material*> material_handles;
			std::vector<std::uint16_t> material_indices;
		};

		void convert_bones(const XModel* asset, converted_model& result)
		{
			auto& base = result.base;
			base.numBones = asset->numBones;
			base.numRootBones = asset->numRootBones;
			base.numClientBones = 0;
			base.boneNames = reinterpret_cast<zonetool::s2::scr_string_t*>(asset->boneNames);

			const auto child_bone_count = asset->numBones - asset->numRootBones;
			result.parent_list.resize(child_bone_count);
			for (auto i = 0; asset->parentList && i < child_bone_count; i++)
			{
				result.parent_list[i] = asset->parentList[i];
			}

			base.parentList = result.parent_list.empty() ? nullptr : result.parent_list.data();
			base.tagAngles = reinterpret_cast<short*>(asset->tagAngles);
			base.tagPositions = reinterpret_cast<float*>(asset->tagPositions);
			base.partClassification = asset->partClassification;
			base.baseMat = reinterpret_cast<char*>(asset->baseMat);
			base.boneInfo = reinterpret_cast<char*>(asset->boneInfo);
		}

		void convert_lods(const XModel* asset, converted_model& result)
		{
			auto& base = result.base;
			for (auto& lod : base.lodInfo)
			{
				lod.dist = no_cull_lod_dist;
			}

			const auto lod_count = std::clamp(static_cast<int>(asset->numLods), 0, 6);
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = base.lodInfo[i];
#if H1_S2_USE_LOD_INFO_DIST
				new_lod.dist = lod.dist > 0.0f ? lod.dist : no_cull_lod_dist;
#endif
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				if (lod.modelSurfs)
				{
					auto& surfs = result.surfs.emplace_back();
					surfs.name = lod.modelSurfs->name;
					new_lod.modelSurfs = &surfs;
				}
			}

			// what the s2 linker bakes (867 of 867 multi lod mp_aachen_v2 models): the lod 0 triangle size, scaled by how many triangles each lod drops
			const auto* lod0 = lod_count ? asset->lodInfo[0].modelSurfs : nullptr;
			const auto lod0_area = xsurface::surface_area(lod0);
			const auto lod0_tris = static_cast<float>(xsurface::triangle_count(lod0));
			for (auto i = 0; i < lod_count; i++)
			{
				const auto tris = xsurface::triangle_count(asset->lodInfo[i].modelSurfs);
				base.lodDistSq[i] = tris ? lod0_area * lod0_tris / static_cast<float>(tris) : 0.0f;
			}

			base.numLods = static_cast<unsigned char>(lod_count);
			base.collLod = asset->collLod;
			base.forcedLod = automatic_forced_lod;
			base.lodRampType = asset->lodRampType;
			base.lodDistScale = 1.0f;
			base.useLodInfoDist = H1_S2_USE_LOD_INFO_DIST;
		}

		void convert_coll_surfs(const XModel* asset, converted_model& result)
		{
			auto& base = result.base;
			result.coll_surfs.resize(asset->collSurfs ? asset->numCollSurfs : 0);
			for (auto i = 0u; i < result.coll_surfs.size(); i++)
			{
				const auto& surf = asset->collSurfs[i];
				auto& new_surf = result.coll_surfs[i];
				std::memset(&new_surf, 0, sizeof(new_surf));
				new_surf.bounds = std::bit_cast<zonetool::s2::Bounds>(surf.bounds);
				new_surf.boneIdx = surf.boneIdx;
				new_surf.contents = static_cast<std::uint32_t>(surf.contents);
				new_surf.surfFlags = convert_surface_flags(static_cast<std::uint32_t>(surf.surfFlags));
			}

			base.collSurfCount = static_cast<unsigned short>(result.coll_surfs.size());
			base.collSurfs = result.coll_surfs.empty() ? nullptr : reinterpret_cast<char*>(result.coll_surfs.data());
		}

		void convert_materials(const XModel* asset, converted_model& result)
		{
			auto& model = result.model;
			result.material_handles.resize(asset->numsurfs);
			result.material_indices.assign(asset->numsurfs, std::uint16_t{0xFFFF});
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto material = asset->materialHandles ? asset->materialHandles[i] : nullptr;
				auto& stub = result.materials.emplace_back();
				stub.name = material && material->name ? material->name : "$default";
				result.material_handles[i] = &stub;
			}

			model.numMaterials = asset->numsurfs;
			model.materialHandles = result.material_handles.empty() ? nullptr : result.material_handles.data();
			model.materialIndices = result.material_indices.empty() ? nullptr : result.material_indices.data();

			model.runtimeSize = static_cast<std::uint16_t>(sizeof(zonetool::s2::XModel)
				+ asset->numsurfs * (sizeof(zonetool::s2::Material*) + sizeof(std::uint16_t)));
		}

		std::unique_ptr<converted_model> convert(const XModel* asset)
		{
			auto result = std::make_unique<converted_model>();
			result->base_name = std::string(asset->name) + "_h1base";

			auto& base = result->base;
			std::memset(&base, 0, sizeof(base));
			base.name = result->base_name.data();
			base.scale = asset->scale;
			std::ranges::fill(base.unkScale, 1.0f);
			convert_bones(asset, *result);
			convert_lods(asset, *result);
			convert_coll_surfs(asset, *result);

			base.contents = asset->contents;
			base.radius = asset->radius;
			std::memcpy(base.boundsMidPoint, asset->bounds.midPoint, sizeof(base.boundsMidPoint));
			std::memcpy(base.boundsHalfSize, asset->bounds.halfSize, sizeof(base.boundsHalfSize));
			base.boundsRadius = std::hypot(base.boundsHalfSize[0], base.boundsHalfSize[1], base.boundsHalfSize[2]);
			base.cullSizeScale = default_cull_size_scale;
			base.memUsage = static_cast<unsigned int>(asset->memUsage);
			base.unk_0x301 = -1;
			base.unk_0x372 = -1;
			base.unk_0x374 = default_unk_0x374;
			const auto sways = has_surface_flag(asset, SURF_FLAG_REACTIVE_MOTION);
			if (sways)
			{
				std::ranges::copy(grass_sway_params, base.swayParams);
			}

			auto& model = result->model;
			std::memset(&model, 0, sizeof(model));
			model.name = asset->name;
			model.base = &base;
			model.flags = zonetool::s2::XMODEL_FLAG_UNK_0x800 | (has_lightmap_uv_surfaces(asset) ? lightmap_uv_model_flags : 0u)
				| (sways ? zonetool::s2::XMODEL_FLAG_SWAY : 0u);
			convert_materials(asset, *result);

			return result;
		}
	}

	void dump(XModel* asset)
	{
		xsurface::assign_base_vert_indices(asset);
		const auto converted = convert(asset);
		zonetool::s2::xmodel_base::dump(&converted->base);
		zonetool::s2::xmodel::dump(&converted->model);
		add_csv_row("xmodel", asset->name);
	}
}
