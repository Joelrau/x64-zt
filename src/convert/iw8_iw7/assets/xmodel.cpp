#include <std_include.hpp>

#include "../include.hpp"
#include "xmodel.hpp"
#include "material.hpp"
#include "xsurface.hpp"

#include <iw7/assets/xmodel.hpp>
#include <iw7/assets/xsurface.hpp>

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

		int convert_flags(const unsigned int flags)
		{
			return static_cast<int>(flags & ~(target::XMODEL_FLAG_REACTIVE_MOTION | target::XMODEL_FLAG_HAS_DEFAULT_MODEL
				| target::XMODEL_FLAG_HAS_ANY_DEFAULT_SURFS | target::XMODEL_FLAG_HAS_ALL_DEFAULT_SURFS
				| XMODEL_FLAG_SKINNED | XMODEL_FLAG_STREAMED_IMAGES));
		}

		target::XModel* convert(const XModel* asset, utils::memory::allocator& allocator)
		{
			const auto result = allocator.allocate<target::XModel>();

			const auto has_surfaces = has_loaded_surfaces(asset);
			result->name = asset->name;
			result->hasLods = has_surfaces && asset->numLods != 0;
			result->numLods = has_surfaces ? asset->numLods : 0;
			result->collLod = asset->collLod;
			result->shadowCutoffLod = asset->shadowCutoffLod;
			result->characterCollBoundsType = asset->characterCollBoundsType;
			result->impactType = asset->impactType;
			result->flags = convert_flags(asset->flags);

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

		std::array<float, 4> multiply_quat(const float* a, const float* b)
		{
			return
			{
				a[3] * b[0] + a[0] * b[3] + a[1] * b[2] - a[2] * b[1],
				a[3] * b[1] - a[0] * b[2] + a[1] * b[3] + a[2] * b[0],
				a[3] * b[2] + a[0] * b[1] - a[1] * b[0] + a[2] * b[3],
				a[3] * b[3] - a[0] * b[0] - a[1] * b[1] - a[2] * b[2],
			};
		}

		std::array<float, 3> rotate_vector(const float* quat, const float* vector)
		{
			const std::array<float, 3> twice_cross
			{
				2.0f * (quat[1] * vector[2] - quat[2] * vector[1]),
				2.0f * (quat[2] * vector[0] - quat[0] * vector[2]),
				2.0f * (quat[0] * vector[1] - quat[1] * vector[0]),
			};

			return
			{
				vector[0] + quat[3] * twice_cross[0] + quat[1] * twice_cross[2] - quat[2] * twice_cross[1],
				vector[1] + quat[3] * twice_cross[1] + quat[2] * twice_cross[0] - quat[0] * twice_cross[2],
				vector[2] + quat[3] * twice_cross[2] + quat[0] * twice_cross[1] - quat[1] * twice_cross[0],
			};
		}

		std::array<float, 3> transform_point(const DObjAnimMat& pose, const float* point)
		{
			const auto rotated = rotate_vector(pose.quat, point);
			return { rotated[0] + pose.trans[0], rotated[1] + pose.trans[1], rotated[2] + pose.trans[2] };
		}

		DObjAnimMat make_pose(const std::array<float, 4>& quat, const std::array<float, 3>& trans)
		{
			DObjAnimMat pose{};
			std::memcpy(pose.quat, quat.data(), sizeof(pose.quat));
			std::memcpy(pose.trans, trans.data(), sizeof(pose.trans));

			const auto length_squared = quat[0] * quat[0] + quat[1] * quat[1] + quat[2] * quat[2] + quat[3] * quat[3];
			pose.transWeight = length_squared > 0.0f ? 2.0f / length_squared : 0.0f;
			return pose;
		}

		DObjAnimMat compose_pose(const DObjAnimMat& outer, const DObjAnimMat& inner)
		{
			return make_pose(multiply_quat(outer.quat, inner.quat), transform_point(outer, inner.trans));
		}

		DObjAnimMat invert_pose(const DObjAnimMat& pose)
		{
			const std::array<float, 4> conjugate{ -pose.quat[0], -pose.quat[1], -pose.quat[2], pose.quat[3] };
			const auto trans = rotate_vector(conjugate.data(), pose.trans);
			return make_pose(conjugate, { -trans[0], -trans[1], -trans[2] });
		}

		int append_bone(merged_skeleton& skeleton, const XModel* model, const unsigned int index, const int parent, const DObjAnimMat& pose)
		{
			const auto bone = static_cast<int>(skeleton.bones.size());
			skeleton.bone_by_name.emplace(static_cast<unsigned int>(model->boneNames[index]), bone);
			skeleton.bones.emplace_back(merged_bone{ model, index, parent, pose });
			return bone;
		}

		bool add_model(merged_skeleton& skeleton, const XModel* model)
		{
			const auto bone_count = static_cast<unsigned int>(model->numBones + model->numClientBones);
			if (!model->numRootBones || !model->boneNames || !model->baseMat
				|| (bone_count > model->numRootBones && (!model->parentList || !model->quats || !model->trans)))
			{
				return false;
			}

			const auto is_base = skeleton.bones.empty();
			std::vector<int> map(bone_count, -1);
			for (auto i = 0u; i < model->numRootBones; i++)
			{
				if (is_base)
				{
					map[i] = append_bone(skeleton, model, i, -1, model->baseMat[i]);
					continue;
				}

				const auto found = skeleton.bone_by_name.find(static_cast<unsigned int>(model->boneNames[i]));
				if (found == skeleton.bone_by_name.end())
				{
					return false;
				}

				map[i] = found->second;
			}

			const auto transform = is_base ? make_pose({ 0.0f, 0.0f, 0.0f, 1.0f }, {})
				: compose_pose(skeleton.bones[map[0]].pose, invert_pose(model->baseMat[0]));

			for (auto i = static_cast<unsigned int>(model->numRootBones); i < bone_count; i++)
			{
				const auto found = skeleton.bone_by_name.find(static_cast<unsigned int>(model->boneNames[i]));
				if (found == skeleton.bone_by_name.end())
				{
					const auto parent = i - model->parentList[i - model->numRootBones];
					map[i] = append_bone(skeleton, model, i, map[parent], compose_pose(transform, model->baseMat[i]));
					continue;
				}

				auto& bone = skeleton.bones[found->second];
				if (bone.index >= bone.model->numBones && i < model->numBones)
				{
					bone.model = model;
					bone.index = i;
				}

				map[i] = found->second;
			}

			skeleton.bone_map[model] = std::move(map);
			skeleton.transforms[model] = transform;
			return true;
		}

		int merged_index(const merged_skeleton& skeleton, const XModel* model, const unsigned int index)
		{
			const auto& map = skeleton.bone_map.at(model);
			return index < map.size() ? map[index] : -1;
		}

		template <typename D>
		void set_part_bit(D* bits, const unsigned int bit)
		{
			bits[bit >> 5] |= static_cast<D>(0x80000000u >> (bit & 31));
		}

		template <typename S, typename D>
		void remap_part_bits(const S* bits, const XModel* model, const merged_skeleton& skeleton, D* result)
		{
			for (auto bit = 0u; bit < 256; bit++)
			{
				if (!(static_cast<unsigned int>(bits[bit >> 5]) & (0x80000000u >> (bit & 31))))
				{
					continue;
				}

				const auto merged_bit = merged_index(skeleton, model, bit);
				if (merged_bit >= 0)
				{
					set_part_bit(result, static_cast<unsigned int>(merged_bit));
				}
			}
		}

		void write_bones(const merged_skeleton& skeleton, target::XModel* result, utils::memory::allocator& allocator)
		{
			const auto bone_count = skeleton.bones.size();
			const auto child_count = bone_count - result->numRootBones;
			result->boneNames = allocator.allocate_array<target::scr_string_t>(bone_count);
			result->parentList = allocator.allocate_array<unsigned char>(child_count);
			result->tagAngles = allocator.allocate_array<target::XModelAngle>(child_count);
			result->tagPositions = allocator.allocate_array<target::XModelTagPos>(child_count);
			result->partClassification = allocator.allocate_array<unsigned char>(result->numBones);
			result->baseMat = allocator.allocate_array<target::DObjAnimMat>(bone_count);
			result->boneInfo = allocator.allocate_array<target::XBoneInfo>(bone_count);

			for (auto index = 0u; index < bone_count; index++)
			{
				const auto& bone = skeleton.bones[index];
				const auto model = bone.model;

				result->boneNames[index] = static_cast<target::scr_string_t>(model->boneNames[bone.index]);
				std::memcpy(&result->baseMat[index], &bone.pose, sizeof(target::DObjAnimMat));
				if (model->boneInfo)
				{
					std::memcpy(&result->boneInfo[index], &model->boneInfo[bone.index], sizeof(target::XBoneInfo));
				}

				if (bone.index < model->numBones && model->partClassification)
				{
					result->partClassification[index] = model->partClassification[bone.index];
				}

				if (bone.parent < 0)
				{
					continue;
				}

				const auto child = index - result->numRootBones;
				const auto source_child = bone.index - model->numRootBones;
				result->parentList[child] = static_cast<unsigned char>(index - bone.parent);
				std::memcpy(&result->tagAngles[child], &model->quats[4 * source_child], sizeof(target::XModelAngle));
				std::memcpy(&result->tagPositions[child], &model->trans[3 * source_child], sizeof(target::XModelTagPos));
			}
		}

		std::uint32_t rotate_unit_vec(const DObjAnimMat& transform, const std::uint32_t packed)
		{
			const auto value = xsurface::unpack_unit_vec(packed);
			return xsurface::pack_unit_vec(rotate_vector(transform.quat, value.data()), packed >> 30);
		}

		void transform_surface(target::XSurface& surface, const DObjAnimMat& transform)
		{
			for (auto i = 0; i < surface.vertCount; i++)
			{
				auto& vertex = surface.verts0.packedVerts0[i];
				const auto position = transform_point(transform, vertex.xyz);
				std::memcpy(vertex.xyz, position.data(), sizeof(vertex.xyz));
				vertex.normal.packed = rotate_unit_vec(transform, vertex.normal.packed);
				vertex.tangent.packed = rotate_unit_vec(transform, vertex.tangent.packed);
			}
		}

		bool remap_surface_bones(target::XSurface& surface, const XModel* part, const merged_skeleton& skeleton)
		{
			int part_bits[8]{};
			const auto remap_bone = [&](unsigned short& bone)
			{
				const auto index = merged_index(skeleton, part, bone);
				if (index < 0)
				{
					return false;
				}

				bone = static_cast<unsigned short>(index);
				set_part_bit(part_bits, static_cast<unsigned int>(index));
				return true;
			};

			for (auto i = 0; surface.rigidVertLists && i < surface.rigidVertListCount; i++)
			{
				if (!remap_bone(surface.rigidVertLists[i].boneOffsetIndex))
				{
					return false;
				}
			}

			const auto blend_size = surface.blendVerts ? surface.blendVertSize / sizeof(target::XBlendInfo) : 0;
			std::size_t offset = 0;
			for (auto bone_count = 1u; bone_count <= 8; bone_count++)
			{
				for (auto i = 0; i < surface.blendVertCounts[bone_count - 1]; i++, offset += bone_count * 2)
				{
					const auto blend = surface.blendVerts + offset;
					if (offset + bone_count * 2 > blend_size || !remap_bone(blend[0]))
					{
						return false;
					}

					for (auto bone = 1u; bone < bone_count; bone++)
					{
						if (!remap_bone(blend[bone * 2 - 1]))
						{
							return false;
						}
					}
				}
			}

			remap_part_bits(surface.partBits, part, skeleton, part_bits);
			std::memcpy(surface.partBits, part_bits, sizeof(surface.partBits));
			return true;
		}

		target::XModelSurfs* merge_surfaces(const XModel* asset, const std::vector<const XModel*>& parts, const merged_skeleton& skeleton,
			utils::memory::allocator& allocator)
		{
			std::vector<target::XSurface> surfaces;
			for (const auto part : parts)
			{
				const auto& lod = part->lodInfo[0];
				const auto converted = xsurface::convert(find_loaded_surfaces(lod.modelSurfsStaging), allocator);
				if (!converted || converted->numsurfs != lod.numsurfs)
				{
					return nullptr;
				}

				for (auto i = 0; i < converted->numsurfs; i++)
				{
					auto& surface = converted->surfs[i];
					transform_surface(surface, skeleton.transforms.at(part));
					if (!remap_surface_bones(surface, part, skeleton))
					{
						ZONETOOL_ERROR("Surface %d of xmodel \"%s\" uses a bone that is not in the skeleton of \"%s\"", i, part->name, asset->name);
						return nullptr;
					}

					surfaces.emplace_back(surface);
				}
			}

			const auto result = allocator.allocate<target::XModelSurfs>();
			result->name = allocator.duplicate_string(std::format("{}_lod0", asset->name));
			result->numsurfs = static_cast<unsigned short>(surfaces.size());
			result->surfs = allocator.allocate_array<target::XSurface>(surfaces.size());
			for (auto i = 0u; i < surfaces.size(); i++)
			{
				result->surfs[i] = surfaces[i];
				for (auto word = 0; word < 8; word++)
				{
					result->partBits[word] |= surfaces[i].partBits[word];
				}
			}

			return result;
		}

		void merge_materials(const std::vector<const XModel*>& parts, target::XModel* result, utils::memory::allocator& allocator)
		{
			const auto has_radius = std::ranges::all_of(parts, [](const XModel* part) { return part->himipRadiusInvSq != nullptr; });
			result->materialHandles = allocator.allocate_array<target::Material*>(result->numsurfs);
			result->invHighMipRadius = has_radius ? allocator.allocate_array<unsigned short>(result->numsurfs) : nullptr;

			std::unordered_map<std::string_view, target::Material*> stubs;
			auto surface = 0;
			for (const auto part : parts)
			{
				const auto& lod = part->lodInfo[0];
				for (auto i = 0; i < lod.numsurfs; i++, surface++)
				{
					const auto source = lod.surfIndex + i;
					if (const auto material = part->materialHandles[source])
					{
						auto& stub = stubs[material->name];
						if (!stub)
						{
							stub = allocator.allocate<target::Material>();
							stub->name = material->name;
						}

						result->materialHandles[surface] = stub;
					}

					if (has_radius)
					{
						result->invHighMipRadius[surface] = half_float::float_to_half(std::sqrt(part->himipRadiusInvSq[source]));
					}
				}
			}
		}

		void merge_bounds(const std::vector<const XModel*>& parts, const merged_skeleton& skeleton, target::XModel* result)
		{
			std::array<float, 3> mins{ FLT_MAX, FLT_MAX, FLT_MAX };
			std::array<float, 3> maxs{ -FLT_MAX, -FLT_MAX, -FLT_MAX };
			result->radius = 0.0f;

			for (const auto part : parts)
			{
				const auto& transform = skeleton.transforms.at(part);
				for (auto corner = 0; corner < 8; corner++)
				{
					float point[3]{};
					for (auto axis = 0; axis < 3; axis++)
					{
						const auto sign = (corner >> axis) & 1 ? 1.0f : -1.0f;
						point[axis] = part->bounds.midPoint[axis] + sign * part->bounds.halfSize[axis];
					}

					const auto merged = transform_point(transform, point);
					for (auto axis = 0; axis < 3; axis++)
					{
						mins[axis] = std::min(mins[axis], merged[axis]);
						maxs[axis] = std::max(maxs[axis], merged[axis]);
					}
				}

				const auto offset = std::sqrt(transform.trans[0] * transform.trans[0] + transform.trans[1] * transform.trans[1]
					+ transform.trans[2] * transform.trans[2]);
				result->radius = std::max(result->radius, offset + part->radius);
			}

			for (auto axis = 0; axis < 3; axis++)
			{
				result->bounds.midPoint[axis] = (mins[axis] + maxs[axis]) * 0.5f;
				result->bounds.halfSize[axis] = (maxs[axis] - mins[axis]) * 0.5f;
			}
		}

		std::vector<const XModel*> attach_parts(merged_skeleton& skeleton, const XModel* asset, const std::vector<XModel*>& parts)
		{
			std::vector<const XModel*> pending;
			for (const auto part : parts)
			{
				if (part->numLods && has_loaded_surfaces(part))
				{
					pending.emplace_back(part);
				}
				else
				{
					ZONETOOL_WARNING("Xmodel \"%s\" has no loaded surfaces", part->name);
				}
			}

			std::vector<const XModel*> attached;
			if (!add_model(skeleton, asset))
			{
				return attached;
			}

			for (auto progress = true; progress;)
			{
				progress = false;
				for (auto part = pending.begin(); part != pending.end();)
				{
					if (add_model(skeleton, *part))
					{
						attached.emplace_back(*part);
						part = pending.erase(part);
						progress = true;
					}
					else
					{
						++part;
					}
				}
			}

			for (const auto part : pending)
			{
				const auto root = part->boneNames ? SL_ConvertToString(part->boneNames[0]) : nullptr;
				ZONETOOL_WARNING("Xmodel \"%s\" has no bone \"%s\" to attach xmodel \"%s\"", asset->name, root ? root : "", part->name);
			}

			return attached;
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

	bool dump(XModel* asset)
	{
		utils::memory::allocator allocator;
		zonetool::iw7::xmodel::dump(convert(asset, allocator));
		if (!has_loaded_surfaces(asset))
		{
			ZONETOOL_WARNING("A LOD of xmodel \"%s\" has no loaded surfaces", asset->name);
			add_csv_row("xmodel", asset->name);
			return true;
		}

		if (!dump_materials(asset))
		{
			return false;
		}

		dump_surfaces(asset);
		add_csv_row("xmodel", asset->name);
		return true;
	}

	bool dump_with_parts(XModel* asset, const std::vector<XModel*>& parts)
	{
		merged_skeleton skeleton{};
		const auto attached = attach_parts(skeleton, asset, parts);
		if (attached.empty())
		{
			return dump(asset);
		}

		if (skeleton.bones.size() > 0xFF)
		{
			ZONETOOL_ERROR("The merged skeleton of xmodel \"%s\" does not fit the bone limits", asset->name);
			return dump(asset);
		}

		utils::memory::allocator allocator;
		const auto surfaces = merge_surfaces(asset, attached, skeleton, allocator);
		if (!surfaces)
		{
			return dump(asset);
		}

		const auto result = convert(asset, allocator);
		result->hasLods = true;
		result->numLods = 1;
		result->numBones = static_cast<unsigned char>(skeleton.bones.size());
		result->numClientBones = 0;
		result->numsurfs = surfaces->numsurfs;
		write_bones(skeleton, result, allocator);
		merge_materials(attached, result, allocator);
		merge_bounds(attached, skeleton, result);

		std::memset(result->noScalePartBits, 0, sizeof(result->noScalePartBits));
		remap_part_bits(asset->noScalePartBits, asset, skeleton, result->noScalePartBits);

		std::memset(result->lodInfo, 0, sizeof(result->lodInfo));
		auto& lod = result->lodInfo[0];
		lod.numsurfs = surfaces->numsurfs;
		lod.modelSurfs = surfaces;
		lod.flags = attached.front()->lodInfo[0].flags;
		std::memcpy(lod.partBits, surfaces->partBits, sizeof(lod.partBits));

		for (const auto part : attached)
		{
			result->flags |= convert_flags(part->flags);
			lod.dist = std::max(lod.dist, part->lodInfo[0].dist);
			remap_part_bits(part->noScalePartBits, part, skeleton, result->noScalePartBits);
		}

		zonetool::iw7::xmodel::dump(result);

		auto materials_dumped = true;
		for (const auto part : attached)
		{
			materials_dumped = dump_materials(part) && materials_dumped;
		}

		if (!materials_dumped)
		{
			return false;
		}

		zonetool::iw7::xsurface::dump(surfaces);
		add_csv_row("xmodel", asset->name);
		return true;
	}
}
