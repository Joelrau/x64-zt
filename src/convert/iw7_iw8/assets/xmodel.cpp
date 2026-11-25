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

	namespace
	{
		xsurface::vec3 cross(const xsurface::vec3& a, const xsurface::vec3& b)
		{
			return { a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0] };
		}

		xsurface::vec3 rotate(const float* quat, const xsurface::vec3& v)
		{
			const xsurface::vec3 axis{ quat[0], quat[1], quat[2] };
			auto inner = cross(axis, v);
			for (auto i = 0; i < 3; i++)
			{
				inner[i] += quat[3] * v[i];
			}

			const auto outer = cross(axis, inner);
			return { v[0] + 2.0f * outer[0], v[1] + 2.0f * outer[1], v[2] + 2.0f * outer[2] };
		}

		DObjAnimMat transform(const DObjAnimMat& parent, const DObjAnimMat& child)
		{
			const auto* a = parent.quat;
			const auto* b = child.quat;

			auto result = child;
			result.quat[0] = a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1];
			result.quat[1] = a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0];
			result.quat[2] = a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3];
			result.quat[3] = a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2];

			const auto trans = rotate(a, { child.trans[0], child.trans[1], child.trans[2] });
			for (auto i = 0; i < 3; i++)
			{
				result.trans[i] = trans[i] + parent.trans[i];
			}

			return result;
		}

		std::uint32_t rotate_unit_vec(const std::uint32_t packed, const float* quat)
		{
			const auto rotated = rotate(quat, xsurface::unpack_unit_vec(packed));

			auto result = packed & 0xC0000000;
			for (auto i = 0; i < 3; i++)
			{
				const auto value = (std::clamp(rotated[i], -1.0f, 1.0f) + 1.0f) * 0.5f * 1023.0f;
				result |= static_cast<std::uint32_t>(std::lround(value)) << (10 * i);
			}

			return result;
		}

		bool has_rigid_surfaces_only(const XModel* asset)
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto* surfs = asset->lodInfo[i].modelSurfs;
				if (!surfs || !surfs->surfs)
				{
					return false;
				}

				for (auto surf = 0; surf < surfs->numsurfs; surf++)
				{
					const auto& surface = surfs->surfs[surf];
					if (surface.blendVertSize || !surface.verts0.packedVerts0 || (surface.flags & (SURF_FLAG_REACTIVE_MOTION | SURF_FLAG_SUBDIV | SURF_FLAG_TENSION)))
					{
						return false;
					}
				}
			}

			return asset->numLods > 0;
		}

		std::vector<unsigned char> map_attachment_bones(const XModel* asset, const XModel* attachment)
		{
			if (asset->numBones + attachment->numBones > 255)
			{
				return {};
			}

			std::vector<unsigned char> bones(attachment->numBones);
			auto bone_count = asset->numBones;
			for (auto i = 0; i < attachment->numBones; i++)
			{
				const auto end = asset->boneNames + asset->numBones;
				const auto found = std::find(asset->boneNames, end, attachment->boneNames[i]);
				if (found != end)
				{
					bones[i] = static_cast<unsigned char>(found - asset->boneNames);
				}
				else if (i < attachment->numRootBones)
				{
					return {};
				}
				else
				{
					bones[i] = bone_count++;
				}
			}

			return bones;
		}

		void merge_bones(const XModel* asset, const XModel* attachment, const std::vector<unsigned char>& bones, XModel* result, utils::memory::allocator& allocator)
		{
			const auto bone_count = std::max<std::size_t>(asset->numBones, *std::ranges::max_element(bones) + 1);
			const auto roots = asset->numRootBones;
			const auto children = asset->numBones - roots;

			result->numBones = static_cast<unsigned char>(bone_count);
			result->boneNames = allocator.allocate_array<scr_string_t>(bone_count);
			result->parentList = allocator.allocate_array<unsigned char>(bone_count - roots);
			result->tagAngles = allocator.allocate_array<XModelAngle>(bone_count - roots);
			result->tagPositions = allocator.allocate_array<XModelTagPos>(bone_count - roots);
			result->partClassification = allocator.allocate_array<unsigned char>(bone_count);
			result->baseMat = allocator.allocate_array<DObjAnimMat>(bone_count);
			result->boneInfo = allocator.allocate_array<XBoneInfo>(bone_count);

			std::copy_n(asset->boneNames, asset->numBones, result->boneNames);
			std::copy_n(asset->parentList, children, result->parentList);
			std::copy_n(asset->tagAngles, children, result->tagAngles);
			std::copy_n(asset->tagPositions, children, result->tagPositions);
			std::copy_n(asset->partClassification, asset->numBones, result->partClassification);
			std::copy_n(asset->baseMat, asset->numBones, result->baseMat);
			std::copy_n(asset->boneInfo, asset->numBones, result->boneInfo);

			const auto& anchor = asset->baseMat[bones[0]];
			for (auto i = 0; i < attachment->numBones; i++)
			{
				const auto bone = bones[i];
				if (bone < asset->numBones)
				{
					continue;
				}

				const auto child = i - attachment->numRootBones;
				const auto parent = bones[i - attachment->parentList[child]];
				result->boneNames[bone] = attachment->boneNames[i];
				result->parentList[bone - roots] = static_cast<unsigned char>(bone - parent);
				result->tagAngles[bone - roots] = attachment->tagAngles[child];
				result->tagPositions[bone - roots] = attachment->tagPositions[child];
				result->partClassification[bone] = attachment->partClassification[i];
				result->baseMat[bone] = transform(anchor, attachment->baseMat[i]);
				result->boneInfo[bone] = attachment->boneInfo[i];
			}
		}

		XSurface merge_surface(const XSurface& surface, const DObjAnimMat& anchor, const std::vector<unsigned char>& bones, utils::memory::allocator& allocator)
		{
			auto result = surface;

			result.verts0.packedVerts0 = allocator.allocate_array<GfxPackedVertex>(surface.vertCount);
			for (auto i = 0; i < surface.vertCount; i++)
			{
				auto& vertex = result.verts0.packedVerts0[i];
				vertex = surface.verts0.packedVerts0[i];

				const auto position = rotate(anchor.quat, { vertex.xyz[0], vertex.xyz[1], vertex.xyz[2] });
				for (auto axis = 0; axis < 3; axis++)
				{
					vertex.xyz[axis] = position[axis] + anchor.trans[axis];
				}

				vertex.normal.packed = rotate_unit_vec(vertex.normal.packed, anchor.quat);
				vertex.tangent.packed = rotate_unit_vec(vertex.tangent.packed, anchor.quat);
			}

			if (surface.rigidVertLists)
			{
				result.rigidVertLists = allocator.allocate_array<XRigidVertList>(surface.rigidVertListCount);
				for (auto i = 0; i < surface.rigidVertListCount; i++)
				{
					result.rigidVertLists[i] = surface.rigidVertLists[i];
					result.rigidVertLists[i].boneOffsetIndex = bones[surface.rigidVertLists[i].boneOffsetIndex];
				}
			}

			std::memset(result.partBits, 0, sizeof(result.partBits));
			for (auto i = 0u; i < bones.size(); i++)
			{
				if (surface.partBits[i >> 5] & (0x80000000 >> (i & 31)))
				{
					result.partBits[bones[i] >> 5] |= 0x80000000 >> (bones[i] & 31);
				}
			}

			return result;
		}

		XModelSurfs* merge_surfs(const XModelSurfs* surfs, const XModelSurfs* attachment_surfs, const std::vector<int>& visible_surfaces,
			const DObjAnimMat& anchor, const std::vector<unsigned char>& bones, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<XModelSurfs>();
			result->name = allocator.duplicate_string(std::format("{}_{}", surfs->name, attachment_surfs->name));
			result->numsurfs = static_cast<unsigned short>(surfs->numsurfs + visible_surfaces.size());
			result->surfs = allocator.allocate_array<XSurface>(result->numsurfs);
			std::memcpy(result->partBits, surfs->partBits, sizeof(result->partBits));
			std::copy_n(surfs->surfs, surfs->numsurfs, result->surfs);

			for (auto i = 0u; i < visible_surfaces.size(); i++)
			{
				auto& surface = result->surfs[surfs->numsurfs + i];
				surface = merge_surface(attachment_surfs->surfs[visible_surfaces[i]], anchor, bones, allocator);
				for (auto word = 0; word < 8; word++)
				{
					result->partBits[word] |= surface.partBits[word];
				}
			}

			return result;
		}

		XModel* merge_attachment(const XModel* asset, const XModel* attachment, utils::memory::allocator& allocator)
		{
			if (!has_rigid_surfaces_only(asset) || !has_rigid_surfaces_only(attachment))
			{
				return nullptr;
			}

			const auto bones = map_attachment_bones(asset, attachment);
			if (bones.empty())
			{
				return nullptr;
			}

			const auto result = allocator.allocate<XModel>();
			*result = *asset;
			merge_bones(asset, attachment, bones, result, allocator);

			std::vector<Material*> materials;
			std::vector<unsigned short> himip_radius;
			const auto add_surface = [&](const XModel* model, const int index)
			{
				materials.emplace_back(model->materialHandles[index]);
				himip_radius.emplace_back(model->invHighMipRadius ? model->invHighMipRadius[index] : 0);
			};

			for (auto i = 0; i < asset->numLods; i++)
			{
				const auto& lod = asset->lodInfo[i];
				const auto& attachment_lod = attachment->lodInfo[std::min<int>(i, attachment->numLods - 1)];

				std::vector<int> visible_surfaces;
				for (auto surf = 0; surf < attachment_lod.numsurfs; surf++)
				{
					const auto material = attachment->materialHandles[attachment_lod.surfIndex + surf];
					if (material::is_visible(material))
					{
						visible_surfaces.emplace_back(surf);
					}
				}

				auto& new_lod = result->lodInfo[i];
				new_lod.modelSurfs = merge_surfs(lod.modelSurfs, attachment_lod.modelSurfs, visible_surfaces, asset->baseMat[bones[0]], bones, allocator);
				new_lod.surfs = new_lod.modelSurfs->surfs;
				new_lod.numsurfs = new_lod.modelSurfs->numsurfs;
				new_lod.surfIndex = static_cast<unsigned short>(materials.size());

				for (auto surf = 0; surf < lod.numsurfs; surf++)
				{
					add_surface(asset, lod.surfIndex + surf);
				}

				for (const auto surf : visible_surfaces)
				{
					add_surface(attachment, attachment_lod.surfIndex + surf);
				}
			}

			result->numsurfs = static_cast<unsigned short>(materials.size());
			result->materialHandles = allocator.allocate_array<Material*>(materials.size());
			std::ranges::copy(materials, result->materialHandles);
			if (asset->invHighMipRadius)
			{
				result->invHighMipRadius = allocator.allocate_array<unsigned short>(himip_radius.size());
				std::ranges::copy(himip_radius, result->invHighMipRadius);
			}

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

	bool dump_with_attachment(XModel* asset, XModel* attachment)
	{
		utils::memory::allocator allocator;
		const auto merged = merge_attachment(asset, attachment, allocator);
		if (!merged)
		{
			return false;
		}

		dump(merged);
		return true;
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
