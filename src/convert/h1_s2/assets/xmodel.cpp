#include <std_include.hpp>

#include "../include.hpp"
#include "material.hpp"
#include "physcollmap.hpp"
#include "physpreset.hpp"
#include "techset.hpp"
#include "xmodel.hpp"
#include "xsurface.hpp"

namespace zonetool::h1::converter::s2::xmodel
{
	namespace
	{
		constexpr auto no_cull_lod_dist = 1000000.0f;
		constexpr auto automatic_forced_lod = 0xFE;
		constexpr auto no_cull_size_scale = 0.0f; // h1 has no projected size cull
		constexpr auto default_unk_0x374 = 96.0f;
		constexpr auto lightmap_uv_model_flags = 0x6000u;
		constexpr auto stock_name_suffix = "_h1";
		constexpr std::array<float, 4> grass_sway_params{2000.0f, 40.0f, 0.055f, 1.5f};

		std::unordered_set<std::string> dumped_models;
		std::map<std::pair<zonetool::s2::XAssetType, std::string>, std::string> converted_names;

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

		unsigned short* convert_parent_list(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto child_bone_count = asset->numBones - asset->numRootBones;
			if (child_bone_count <= 0)
			{
				return nullptr;
			}

			const auto parent_list = allocator.allocate_array<unsigned short>(child_bone_count);
			for (auto i = 0; asset->parentList && i < child_bone_count; i++)
			{
				parent_list[i] = asset->parentList[i];
			}

			return parent_list;
		}

		void convert_bones(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			base->numBones = asset->numBones;
			base->numRootBones = asset->numRootBones;
			base->numClientBones = 0;
			base->boneNames = reinterpret_cast<zonetool::s2::scr_string_t*>(asset->boneNames);
			base->parentList = convert_parent_list(asset, allocator);
			base->tagAngles = reinterpret_cast<short*>(asset->tagAngles);
			base->tagPositions = reinterpret_cast<float*>(asset->tagPositions);
			base->partClassification = asset->partClassification;
			base->baseMat = reinterpret_cast<char*>(asset->baseMat);
			base->boneInfo = reinterpret_cast<char*>(asset->boneInfo);
		}

		void convert_lods(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			for (auto& lod : base->lodInfo)
			{
				lod.dist = no_cull_lod_dist;
			}

			const auto lod_count = std::clamp(static_cast<int>(asset->numLods), 0, 6);
			for (auto i = 0; i < lod_count; i++)
			{
				const auto& lod = asset->lodInfo[i];
				auto& new_lod = base->lodInfo[i];
				new_lod.dist = lod.dist > 0.0f ? lod.dist : no_cull_lod_dist;
				new_lod.numsurfs = lod.numsurfs;
				new_lod.surfIndex = lod.surfIndex;
				if (lod.modelSurfs)
				{
					new_lod.modelSurfs = make_stub<zonetool::s2::XModelSurfs>(
						converted_name(zonetool::s2::ASSET_TYPE_XMODEL_SURFS, lod.modelSurfs->name), allocator);
				}
			}

			// the lod distances that the s2 linker bakes into stock models
			const auto* lod0 = lod_count ? asset->lodInfo[0].modelSurfs : nullptr;
			const auto lod0_area = xsurface::surface_area(lod0);
			const auto lod0_tris = static_cast<float>(xsurface::triangle_count(lod0));
			for (auto i = 0; i < lod_count; i++)
			{
				const auto tris = xsurface::triangle_count(asset->lodInfo[i].modelSurfs);
				base->lodDistSq[i] = tris ? lod0_area * lod0_tris / static_cast<float>(tris) : 0.0f;
			}

			base->numLods = static_cast<unsigned char>(lod_count);
			base->collLod = asset->collLod;
			base->forcedLod = automatic_forced_lod;
			base->lodRampType = asset->lodRampType;
			base->lodDistScale = 1.0f;
			base->useLodInfoDist = 1;
		}

		void convert_coll_surfs(const XModel* asset, zonetool::s2::XModelBase* base, utils::memory::allocator& allocator)
		{
			const auto count = asset->collSurfs ? asset->numCollSurfs : 0;
			base->collSurfCount = static_cast<unsigned short>(count);
			if (!count)
			{
				return;
			}

			const auto coll_surfs = allocator.allocate_array<zonetool::s2::XModelCollSurf>(count);
			for (auto i = 0; i < count; i++)
			{
				const auto& surf = asset->collSurfs[i];
				auto& new_surf = coll_surfs[i];
				new_surf.bounds = std::bit_cast<zonetool::s2::Bounds>(surf.bounds);
				new_surf.boneIdx = surf.boneIdx;
				new_surf.contents = static_cast<std::uint32_t>(surf.contents);
				new_surf.surfFlags = convert_surface_flags(static_cast<std::uint32_t>(surf.surfFlags));
			}

			base->collSurfs = reinterpret_cast<char*>(coll_surfs);
		}

		void convert_materials(const XModel* asset, zonetool::s2::XModel* model, utils::memory::allocator& allocator)
		{
			model->numMaterials = asset->numsurfs;
			model->materialHandles = allocator.allocate_array<zonetool::s2::Material*>(asset->numsurfs);
			model->materialIndices = allocator.allocate_array<unsigned short>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				const auto material = asset->materialHandles ? asset->materialHandles[i] : nullptr;
				model->materialHandles[i] = make_stub<zonetool::s2::Material>(material && material->name ? material->name : "$default", allocator);
				model->materialIndices[i] = 0xFFFF;
			}

			model->runtimeSize = static_cast<std::uint16_t>(sizeof(zonetool::s2::XModel)
				+ asset->numsurfs * (sizeof(zonetool::s2::Material*) + sizeof(std::uint16_t)));
		}

		zonetool::s2::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto base = allocator.allocate<zonetool::s2::XModelBase>();
			base->name = allocator.duplicate_string(std::string(asset->name) + "_h1base");
			base->scale = asset->scale;
			std::ranges::fill(base->unkScale, 1.0f);
			convert_bones(asset, base, allocator);
			convert_lods(asset, base, allocator);
			convert_coll_surfs(asset, base, allocator);

			base->contents = asset->contents;
			base->radius = asset->radius;
			std::memcpy(base->boundsMidPoint, asset->bounds.midPoint, sizeof(base->boundsMidPoint));
			std::memcpy(base->boundsHalfSize, asset->bounds.halfSize, sizeof(base->boundsHalfSize));
			base->boundsRadius = std::hypot(base->boundsHalfSize[0], base->boundsHalfSize[1], base->boundsHalfSize[2]);
			base->cullSizeScale = no_cull_size_scale;
			base->memUsage = static_cast<unsigned int>(asset->memUsage);
			base->physPreset = asset->physPreset ? physpreset::reference(asset->physPreset, asset, allocator) : nullptr;
			base->physCollmap = physcollmap::reference(asset->physCollmap, allocator);
			base->unk_0x301 = -1;
			base->unk_0x372 = -1;
			base->unk_0x374 = default_unk_0x374;

			const auto sways = has_surface_flag(asset, SURF_FLAG_REACTIVE_MOTION);
			if (sways)
			{
				std::ranges::copy(grass_sway_params, base->swayParams);
			}

			const auto model = allocator.allocate<zonetool::s2::XModel>();
			model->name = converted_name(zonetool::s2::ASSET_TYPE_XMODEL, asset->name);
			model->base = base;
			model->flags = zonetool::s2::XMODEL_FLAG_UNK_0x800
				| (asset->flags & zonetool::s2::XMODEL_FLAG_UNK_0x8)
				| (has_surface_flag(asset, SURF_FLAG_LIGHTMAP_COORDS) ? lightmap_uv_model_flags : 0u)
				| (sways ? zonetool::s2::XMODEL_FLAG_SWAY : 0u);
			convert_materials(asset, model, allocator);
			return model;
		}

		void dump_materials(const XModel* asset)
		{
			for (auto i = 0; asset->materialHandles && i < asset->numsurfs; i++)
			{
				if (const auto material = asset->materialHandles[i]; material && material->name)
				{
					material::dump(material);
				}
			}
		}
	}

	void dump(XModel* asset)
	{
		if (!dumped_models.insert(asset->name).second)
		{
			return;
		}

		utils::memory::allocator allocator;
		xsurface::assign_base_vert_indices(asset);
		for (auto i = 0; i < asset->numLods && i < static_cast<int>(std::size(asset->lodInfo)); i++)
		{
			if (asset->lodInfo[i].modelSurfs)
			{
				xsurface::dump(asset->lodInfo[i].modelSurfs); // a map model can use surfs from a common zone
			}
		}

		const auto converted = convert(asset, allocator);
		zonetool::s2::xmodel_base::dump(converted->base);
		zonetool::s2::xmodel::dump(converted);
		add_csv_row("xmodel", converted->name);
	}

	zonetool::s2::XModel* reference(const XModel* asset, utils::memory::allocator& allocator)
	{
		if (!asset || !asset->name)
		{
			return nullptr;
		}

		dump_used(asset->name);
		return make_stub<zonetool::s2::XModel>(converted_name(zonetool::s2::ASSET_TYPE_XMODEL, asset->name), allocator);
	}

	void dump_used(const char* name)
	{
		auto* loaded = db_find_x_asset_header_safe(ASSET_TYPE_XMODEL, name).model;
		if (loaded && loaded->numLods > 0 && loaded->lodInfo[0].modelSurfs)
		{
			dump(loaded);
			dump_materials(loaded);
			return;
		}

		add_csv_row("xmodel", converted_name(zonetool::s2::ASSET_TYPE_XMODEL, name)); // the model comes from a converted transient zone
	}

	void clear()
	{
		dumped_models.clear();
		converted_names.clear();
	}

	const char* converted_name(const zonetool::s2::XAssetType type, const char* name)
	{
		const auto key = std::make_pair(type, std::string(name));
		if (const auto found = converted_names.find(key); found != converted_names.end())
		{
			return found->second.data();
		}

		auto converted = std::string(name);
		{
			db_scope _(techset::common_zone().db());
			if (zonetool::s2::db_find_x_asset_header_safe(type, converted).data)
			{
				converted += stock_name_suffix;
			}
		}

		return converted_names.emplace(key, std::move(converted)).first->second.data();
	}
}
