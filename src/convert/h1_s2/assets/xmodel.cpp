#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "techset.hpp"

namespace zonetool::h1::converter::s2::xmodel
{
	namespace
	{
		constexpr auto no_cull_lod_dist = 1000000.0f; // TODO

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

		float lod_dist_sq(const float dist, const float radius)
		{
			const auto view_dist = dist <= 0.0f || dist >= no_cull_lod_dist ? no_cull_lod_dist : dist;
			const auto scaled = std::max(0.0f, view_dist - radius) / 565.087f;
			return scaled * scaled;
		}

		void clear_donor_data(zonetool::s2::XModelBase& base)
		{
			base.extraBoneNameCount = 0;
			base.extraDataCount = 0;
			base.extraBoneNames = nullptr;
			base.extraData = nullptr;
			base.physPreset = nullptr;
			base.physCollmap = nullptr;
			base.physRigCount = 0;
			base.physRigNames = nullptr;
			base.physRigs = nullptr;
			base.physClothTuning = nullptr;
			base.cloth = nullptr;
			base.rig = nullptr;
			base.imageRefCount = 0;
			base.imageRefs = nullptr;
			std::fill(std::begin(base.skeletonScripts), std::end(base.skeletonScripts), nullptr);
		}

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
			std::memset(base.lodInfo, 0, sizeof(base.lodInfo));
			std::fill(std::begin(base.lodDistSq), std::end(base.lodDistSq), 0.0f);

			const auto lod_count = std::clamp(static_cast<int>(asset->numLods), 0, 6);
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = base.lodInfo[i];
				new_lod.dist = no_cull_lod_dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				if (lod.modelSurfs)
				{
					auto& surfs = result.surfs.emplace_back();
					surfs.name = lod.modelSurfs->name;
					new_lod.modelSurfs = &surfs;
				}

				const auto is_last = i == lod_count - 1;
				base.lodDistSq[i] = lod_dist_sq(is_last ? no_cull_lod_dist : lod.dist, asset->radius);
			}

			base.numLods = static_cast<unsigned char>(lod_count);
			base.collLod = asset->collLod;
			base.lodDistScale = 1.0f;
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

		std::unique_ptr<converted_model> convert(const XModel* asset, const zonetool::s2::XModelBase* donor)
		{
			auto result = std::make_unique<converted_model>();
			result->base_name = std::string(asset->name) + "_h1base";

			auto& base = result->base;
			base = *donor;
			base.name = result->base_name.data();
			clear_donor_data(base);
			convert_bones(asset, *result);
			convert_lods(asset, *result);
			convert_coll_surfs(asset, *result);

			base.contents = asset->contents;
			base.radius = asset->radius;
			std::memcpy(base.boundsMidPoint, asset->bounds.midPoint, sizeof(base.boundsMidPoint));
			std::memcpy(base.boundsHalfSize, asset->bounds.halfSize, sizeof(base.boundsHalfSize));

			auto& model = result->model;
			std::memset(&model, 0, sizeof(model));
			model.name = asset->name;
			model.base = &base;
			model.flags = zonetool::s2::XMODEL_FLAG_UNK_0x800;
			convert_materials(asset, *result);

			return result;
		}
	}

	void dump(XModel* asset)
	{
		const auto donor = techset::donor_zone().model_base_donor();
		if (!donor)
		{
			ZONETOOL_WARNING("No S2 xmodelbase donor for XModel \"%s\"", asset->name);
			return;
		}

		const auto converted = convert(asset, donor);
		zonetool::s2::xmodel_base::dump(&converted->base);
		zonetool::s2::xmodel::dump(&converted->model);
		add_csv_row("xmodel", asset->name);
	}
}
