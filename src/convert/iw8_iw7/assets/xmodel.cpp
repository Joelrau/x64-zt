#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "material.hpp"
#include "xsurface.hpp"

#include <iw7/assets/xmodel.hpp>

#include <shared.hpp>

namespace zonetool::iw8::converter::iw7::xmodel
{
	namespace
	{
		namespace target = zonetool::iw7;

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

		unsigned short* convert_inv_high_mip_radius(const XModel* asset, utils::memory::allocator& allocator)
		{
			if (!asset->himipRadiusInvSq)
			{
				return nullptr;
			}

			const auto values = allocator.allocate_array<unsigned short>(asset->numsurfs);
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				values[i] = half_float::float_to_half(std::sqrt(asset->himipRadiusInvSq[i]));
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
				std::memcpy(new_lod.partBits, lod.partBits.array, sizeof(new_lod.partBits));
				new_lod.subdivLodValidMask = lod.subdivLodValidMask;
				new_lod.flags = lod.flags;

				if (lod.modelSurfsStaging)
				{
					new_lod.modelSurfs = allocator.allocate<target::XModelSurfs>();
					new_lod.modelSurfs->name = lod.modelSurfsStaging->name;
				}
			}
		}

		Material* find_loaded_material(const Material* handle)
		{
			const auto material = static_cast<Material*>(db_find_asset(ASSET_TYPE_MATERIAL, handle->name));
			return material && material->techniqueSet ? material : nullptr;
		}

		XModelSurfs* find_loaded_surfaces(const XModelSurfs* handle)
		{
			const auto surfaces = static_cast<XModelSurfs*>(db_find_asset(ASSET_TYPE_XMODEL_SURFS, handle->name));
			return surfaces && surfaces->surfs ? surfaces : nullptr;
		}

		bool has_loaded_surfaces(const XModel* asset)
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto surfaces = asset->lodInfo[i].modelSurfsStaging;
				if (!surfaces || !find_loaded_surfaces(surfaces))
				{
					return false;
				}
			}

			return true;
		}

		void dump_surfaces(const XModel* asset)
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				xsurface::dump(find_loaded_surfaces(asset->lodInfo[i].modelSurfsStaging));
			}
		}

		bool dump_materials(const XModel* asset)
		{
			auto converted = true;
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				if (!asset->materialHandles[i])
				{
					continue;
				}

				const auto material = find_loaded_material(asset->materialHandles[i]);
				if (!material)
				{
					ZONETOOL_WARNING("Material \"%s\" of xmodel \"%s\" is not loaded", asset->materialHandles[i]->name, asset->name);
					converted = false;
				}
				else if (!material::dump(material))
				{
					ZONETOOL_WARNING("Material \"%s\" of xmodel \"%s\" has the unsupported type 0x%X", material->name, asset->name, material->materialType);
					converted = false;
				}
			}

			return converted;
		}

		target::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XModel>();

			result->name = asset->name;
			result->hasLods = asset->numLods != 0;
			result->numLods = asset->numLods;
			result->collLod = asset->collLod;
			result->shadowCutoffLod = asset->shadowCutoffLod;
			result->characterCollBoundsType = asset->characterCollBoundsType;
			result->flags = asset->flags & ~(target::XMODEL_FLAG_REACTIVE_MOTION | target::XMODEL_FLAG_HAS_DEFAULT_MODEL
				| target::XMODEL_FLAG_HAS_ANY_DEFAULT_SURFS | target::XMODEL_FLAG_HAS_ALL_DEFAULT_SURFS);

			result->numBones = asset->numBones;
			result->numRootBones = asset->numRootBones;
			result->numClientBones = asset->numClientBones;
			result->numsurfs = asset->numsurfs;
			result->scale = asset->scale;
			result->contents = asset->contents;
			result->radius = asset->radius;
			std::memcpy(&result->bounds, &asset->bounds, sizeof(result->bounds));
			result->edgeLength = asset->edgeLength;
			std::memcpy(&result->physicsUsageCounter, &asset->physicsUsageCounter, sizeof(result->physicsUsageCounter));
			std::memcpy(result->noScalePartBits, asset->noScalePartBits, sizeof(result->noScalePartBits));

			result->numAimAssistBones = asset->numAimAssistBones;
			result->aimAssistBones = reinterpret_cast<target::scr_string_t*>(asset->aimAssistBones);
			result->boneNames = reinterpret_cast<target::scr_string_t*>(asset->boneNames);
			result->parentList = asset->parentList;
			result->tagAngles = reinterpret_cast<target::XModelAngle*>(asset->quats);
			result->tagPositions = reinterpret_cast<target::XModelTagPos*>(asset->trans);
			result->partClassification = asset->partClassification;
			result->baseMat = reinterpret_cast<target::DObjAnimMat*>(asset->baseMat);
			result->boneInfo = reinterpret_cast<target::XBoneInfo*>(asset->boneInfo);

			result->materialHandles = convert_materials(asset, allocator);
			convert_lods(asset, result, allocator);
			result->invHighMipRadius = convert_inv_high_mip_radius(asset, allocator);

			result->unknownIndex = 0xFF;
			result->unknownIndex2 = 0xFF;
			return result;
		}
	}

	bool is_supported(const XModel* asset)
	{
		if (!has_loaded_surfaces(asset))
		{
			return false;
		}

		for (auto i = 0; i < asset->numsurfs; i++)
		{
			if (!asset->materialHandles[i])
			{
				continue;
			}

			const auto material = find_loaded_material(asset->materialHandles[i]);
			if (!material || !material::is_supported(material))
			{
				return false;
			}
		}

		return true;
	}

	void dump(XModel* asset)
	{
		utils::memory::allocator allocator;
		zonetool::iw7::xmodel::dump(convert(asset, allocator));
		if (!has_loaded_surfaces(asset))
		{
			ZONETOOL_WARNING("A LOD of xmodel \"%s\" has no loaded surfaces", asset->name);
		}
		else if (dump_materials(asset))
		{
			dump_surfaces(asset);
			add_csv_row("xmodel", asset->name);
		}
	}
}
