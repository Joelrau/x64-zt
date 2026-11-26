#include <std_include.hpp>
#include "xmodel.hpp"

namespace zonetool::s4
{
	namespace
	{
		template <typename T>
		T* asset_pointer(zone_base* zone, const XAssetType type, const T* asset)
		{
			zone->preload_asset_pointer(type);
			return asset ? static_cast<T*>(zone->get_asset_pointer(type, asset->name)) : nullptr;
		}

		template <typename T>
		void add_dependency(zone_base* zone, const XAssetType type, const T* asset)
		{
			if (asset)
			{
				zone->add_asset_of_type(type, asset->name);
			}
		}

		template <typename T>
		void write_array(zone_buffer* buf, const T* source, T*& target, const std::size_t count, const std::uint32_t alignment)
		{
			if (source)
			{
				buf->align(alignment);
				buf->write(source, count);
				buf->clear_pointer(&target);
			}
		}

		template <typename T>
		void write_asset_array(zone_base* zone, zone_buffer* buf, const XAssetType type, T** source, T**& target, const std::size_t count)
		{
			if (source)
			{
				buf->align(7);
				auto* assets = buf->write(source, count);
				for (std::size_t i = 0; i < count; i++)
				{
					assets[i] = asset_pointer(zone, type, source[i]);
				}
				buf->clear_pointer(&target);
			}
		}

		void dump_script_strings(assetmanager::dumper& dump, const scr_string_t* strings, const std::size_t count)
		{
			dump.dump_array(strings, static_cast<std::uint32_t>(count));
			if (strings)
			{
				for (std::size_t i = 0; i < count; i++)
				{
					dump.dump_string(SL_ConvertToString(strings[i]));
				}
			}
		}

		std::size_t bone_count(const XModel* asset)
		{
			return asset->numBones + asset->numClientBones;
		}

		std::size_t child_bone_count(const XModel* asset)
		{
			return bone_count(asset) - asset->numRootBones;
		}

		void read_bones(zone_reader& reader, XModel* asset)
		{
			const auto bone_count = asset->numBones + asset->numClientBones;
			const auto child_count = bone_count - asset->numRootBones;

			reader.read_script_string_array(asset->aimAssistBones, asset->numAimAssistBones);
			reader.read_script_string_array(asset->boneNames, bone_count);
			reader.read_array(asset->parentList, 1, child_count);
			reader.read_array(asset->quats, 1, 4 * child_count);
			reader.read_array(asset->trans, 3, 3 * child_count);
			reader.read_array(asset->partClassification, 0, asset->numBones);
			reader.read_array(asset->baseMat, 15, bone_count);
			reader.read_array(asset->ikHingeAxis, 3, 8);
		}

		void read_physics(zone_reader& reader, XModel* asset)
		{
			if (asset->physicsType == XMODEL_PHYSICS_RAGDOLL)
			{
				reader.read_asset(ASSET_TYPE_RAGDOLL_ASSET, asset->ragdollAsset);
			}
			else
			{
				reader.read_asset(ASSET_TYPE_PHYSICSASSET, asset->physicsAsset);
			}
		}

		void read_reactive_motion(zone_reader& reader, ReactiveMotionModelInfo*& field)
		{
			if (const auto info = reader.read_single(field, 15))
			{
				reader.read_array(info->modelPartTweaks, 15, info->numParts);
				reader.read_array(info->modelParts, 15, info->numParts);
			}
		}

		void read_blend_shapes(zone_reader& reader, XModelBlendShapeInfo*& field)
		{
			if (const auto info = reader.read_single(field, 7))
			{
				reader.read_script_string_array(info->weightNames, info->numberOfWeights);
				reader.read_array(info->weightMaps, 3, info->numberOfWeightMaps);
			}
		}

		void read_mdao_volumes(zone_reader& reader, XModel* asset)
		{
			if (const auto volumes = reader.read_array(asset->mdaoVolumes, 7, asset->mdaoVolumeCount))
			{
				for (auto i = 0; i < asset->mdaoVolumeCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, volumes[i].volumeData);
				}
			}
		}

		void read_decal_volumes(zone_reader& reader, XModelDecalVolumesInfo*& field)
		{
			const auto info = reader.read_single(field, 7);
			if (!info)
			{
				return;
			}

			if (const auto volumes = reader.read_array(info->decalVolumes, 7, info->numDecalVolumes))
			{
				for (auto i = 0u; i < info->numDecalVolumes; i++)
				{
					reader.read_asset(ASSET_TYPE_MATERIAL, volumes[i].material);
					reader.read_asset(ASSET_TYPE_DECAL_VOLUME_MASK, volumes[i].blendMapOverride);
				}
			}
		}
	}

	void xmodel::read(zone_reader& reader, XModel* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->noScalePartBits, 3, 8);
		reader.read_asset(ASSET_TYPE_SCRIPTABLE, asset->scriptableMoverDef);
		reader.read_asset(ASSET_TYPE_XANIM_PROCEDURALBONES, asset->proceduralBones);
		reader.read_asset(ASSET_TYPE_XANIM_DYNAMICBONES, asset->dynamicBones);
		read_bones(reader, asset);
		read_reactive_motion(reader, asset->reactiveMotionInfo);
		reader.read_asset_array(ASSET_TYPE_MATERIAL, asset->materialHandles, asset->numsurfs);

		if (const auto lods = reader.read_array(asset->lodInfo, 7, asset->numLods))
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				reader.read_asset(ASSET_TYPE_XMODEL_SURFS, lods[i].modelSurfsStaging);
			}
		}

		reader.read_array(asset->unknownArray, 3, asset->unknownArrayCount);
		reader.read_array(asset->boneInfo, 3, asset->numBones + asset->numClientBones);
		reader.read_array(asset->himipRadiusInvSq, 3, asset->numsurfs);
		read_physics(reader, asset);
		reader.read_asset(ASSET_TYPE_PHYSICS_FX_SHAPE, asset->physicsFXShape);
		reader.read_asset(ASSET_TYPE_XMODEL_DETAIL_COLLISION, asset->detailCollision);
		reader.read_asset(ASSET_TYPE_PHYSICS_BONE_GRAPH, asset->physicsBoneGraph);
		reader.read_asset_array(ASSET_TYPE_CLOTH_ASSET, asset->clothAssets, asset->numClothAssets);
		read_blend_shapes(reader, asset->blendShapeInfo);
		read_mdao_volumes(reader, asset);
		read_decal_volumes(reader, asset->decalVolumesInfo);
		reader.read_script_strings(asset->unknownStrings, std::size(asset->unknownStrings));
		reader.pop_stream();
	}

	void xmodel::parse_script_strings(assetmanager::reader& read, scr_string_t*& strings, const std::size_t count)
	{
		strings = read.read_array<scr_string_t>();
		if (strings)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				this->script_strings_[&strings[i]] = read.read_string();
			}
		}
	}

	void xmodel::prepare_script_strings(zone_buffer* buf, scr_string_t* strings, const std::size_t count)
	{
		if (strings)
		{
			for (std::size_t i = 0; i < count; i++)
			{
				strings[i] = static_cast<scr_string_t>(buf->write_scriptstring(this->script_strings_[&strings[i]]));
			}
		}
	}

	XModel* xmodel::parse(const std::string& name, zone_memory* mem)
	{
		assetmanager::reader read(mem);
		if (!read.open("xmodel\\"s + name + ".xmb"))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.data());

		auto* asset = read.read_single<XModel>();
		asset->name = read.read_string();
		asset->noScalePartBits = read.read_array<unsigned int>();
		asset->scriptableMoverDef = read.read_asset<ScriptableDef>();
		asset->proceduralBones = read.read_asset<XAnimProceduralBones>();
		asset->dynamicBones = read.read_asset<XAnimDynamicBones>();

		this->parse_script_strings(read, asset->aimAssistBones, asset->numAimAssistBones);
		this->parse_script_strings(read, asset->boneNames, bone_count(asset));
		asset->parentList = read.read_array<unsigned short>();
		asset->quats = read.read_array<short>();
		asset->trans = read.read_array<float>();
		asset->partClassification = read.read_array<unsigned char>();
		asset->baseMat = read.read_array<DObjAnimMat>();
		asset->ikHingeAxis = read.read_array<vec3_t>();

		if ((asset->reactiveMotionInfo = read.read_single<ReactiveMotionModelInfo>()))
		{
			asset->reactiveMotionInfo->modelPartTweaks = read.read_array<ReactiveMotionModelPartTweaks>();
			asset->reactiveMotionInfo->modelParts = read.read_array<ReactiveMotionModelPart>();
		}

		if ((asset->materialHandles = read.read_array<Material*>()))
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				asset->materialHandles[i] = read.read_asset<Material>();
			}
		}

		if ((asset->lodInfo = read.read_array<XModelLodInfo>()))
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				asset->lodInfo[i].modelSurfsStaging = read.read_asset<XModelSurfs>();
			}
		}

		asset->unknownArray = read.read_array<unsigned long long>();
		asset->boneInfo = read.read_array<XBoneInfo>();
		asset->himipRadiusInvSq = read.read_array<float>();
		if (asset->physicsType == XMODEL_PHYSICS_RAGDOLL)
		{
			asset->ragdollAsset = read.read_asset<RagdollAsset>();
		}
		else
		{
			asset->physicsAsset = read.read_asset<PhysicsAsset>();
		}
		asset->physicsFXShape = read.read_asset<PhysicsFXShape>();
		asset->detailCollision = read.read_asset<XModelDetailCollision>();
		asset->physicsBoneGraph = read.read_asset<PhysicsBoneGraph>();

		if ((asset->clothAssets = read.read_array<ClothAsset*>()))
		{
			for (auto i = 0; i < asset->numClothAssets; i++)
			{
				asset->clothAssets[i] = read.read_asset<ClothAsset>();
			}
		}

		if ((asset->blendShapeInfo = read.read_single<XModelBlendShapeInfo>()))
		{
			this->parse_script_strings(read, asset->blendShapeInfo->weightNames, asset->blendShapeInfo->numberOfWeights);
			asset->blendShapeInfo->weightMaps = read.read_array<BlendShapeWeightMap>();
		}

		if ((asset->mdaoVolumes = read.read_array<MdaoVolume>()))
		{
			for (auto i = 0; i < asset->mdaoVolumeCount; i++)
			{
				asset->mdaoVolumes[i].volumeData = read.read_asset<GfxImage>();
			}
		}

		if ((asset->decalVolumesInfo = read.read_single<XModelDecalVolumesInfo>()))
		{
			if ((asset->decalVolumesInfo->decalVolumes = read.read_array<XModelDecalVolume>()))
			{
				for (auto i = 0u; i < asset->decalVolumesInfo->numDecalVolumes; i++)
				{
					auto& volume = asset->decalVolumesInfo->decalVolumes[i];
					volume.material = read.read_asset<Material>();
					volume.blendMapOverride = read.read_asset<GfxDecalVolumeMask>();
				}
			}
		}

		for (auto& string : asset->unknownStrings)
		{
			this->script_strings_[&string] = read.read_string();
		}

		read.close();
		return asset;
	}

	void xmodel::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<XModel>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void xmodel::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		this->prepare_script_strings(buf, data->aimAssistBones, data->numAimAssistBones);
		this->prepare_script_strings(buf, data->boneNames, bone_count(data));

		if (data->blendShapeInfo)
		{
			this->prepare_script_strings(buf, data->blendShapeInfo->weightNames, data->blendShapeInfo->numberOfWeights);
		}

		this->prepare_script_strings(buf, data->unknownStrings, std::size(data->unknownStrings));
	}

	void xmodel::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		add_dependency(zone, ASSET_TYPE_SCRIPTABLE, data->scriptableMoverDef);
		add_dependency(zone, ASSET_TYPE_XANIM_PROCEDURALBONES, data->proceduralBones);
		add_dependency(zone, ASSET_TYPE_XANIM_DYNAMICBONES, data->dynamicBones);

		if (data->materialHandles)
		{
			for (auto i = 0; i < data->numsurfs; i++)
			{
				add_dependency(zone, ASSET_TYPE_MATERIAL, data->materialHandles[i]);
			}
		}

		if (data->lodInfo)
		{
			for (auto i = 0; i < data->numLods; i++)
			{
				add_dependency(zone, ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfsStaging);
			}
		}

		if (data->physicsType == XMODEL_PHYSICS_RAGDOLL)
		{
			add_dependency(zone, ASSET_TYPE_RAGDOLL_ASSET, data->ragdollAsset);
		}
		else
		{
			add_dependency(zone, ASSET_TYPE_PHYSICSASSET, data->physicsAsset);
		}
		add_dependency(zone, ASSET_TYPE_PHYSICS_FX_SHAPE, data->physicsFXShape);
		add_dependency(zone, ASSET_TYPE_XMODEL_DETAIL_COLLISION, data->detailCollision);
		add_dependency(zone, ASSET_TYPE_PHYSICS_BONE_GRAPH, data->physicsBoneGraph);

		if (data->clothAssets)
		{
			for (auto i = 0; i < data->numClothAssets; i++)
			{
				add_dependency(zone, ASSET_TYPE_CLOTH_ASSET, data->clothAssets[i]);
			}
		}

		if (data->mdaoVolumes)
		{
			for (auto i = 0; i < data->mdaoVolumeCount; i++)
			{
				add_dependency(zone, ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData);
			}
		}

		if (data->decalVolumesInfo && data->decalVolumesInfo->decalVolumes)
		{
			for (auto i = 0u; i < data->decalVolumesInfo->numDecalVolumes; i++)
			{
				add_dependency(zone, ASSET_TYPE_MATERIAL, data->decalVolumesInfo->decalVolumes[i].material);
				add_dependency(zone, ASSET_TYPE_DECAL_VOLUME_MASK, data->decalVolumesInfo->decalVolumes[i].blendMapOverride);
			}
		}
	}

	std::string xmodel::name()
	{
		return this->name_;
	}

	std::int32_t xmodel::type()
	{
		return ASSET_TYPE_XMODEL;
	}

	void xmodel::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());
		write_array(buf, data->noScalePartBits, dest->noScalePartBits, 8, 3);
		dest->scriptableMoverDef = asset_pointer(zone, ASSET_TYPE_SCRIPTABLE, data->scriptableMoverDef);
		dest->proceduralBones = asset_pointer(zone, ASSET_TYPE_XANIM_PROCEDURALBONES, data->proceduralBones);
		dest->dynamicBones = asset_pointer(zone, ASSET_TYPE_XANIM_DYNAMICBONES, data->dynamicBones);

		write_array(buf, data->aimAssistBones, dest->aimAssistBones, data->numAimAssistBones, 3);
		write_array(buf, data->boneNames, dest->boneNames, bone_count(data), 3);
		write_array(buf, data->parentList, dest->parentList, child_bone_count(data), 1);
		write_array(buf, data->quats, dest->quats, 4 * child_bone_count(data), 1);
		write_array(buf, data->trans, dest->trans, 3 * child_bone_count(data), 3);
		write_array(buf, data->partClassification, dest->partClassification, data->numBones, 0);
		write_array(buf, data->baseMat, dest->baseMat, bone_count(data), 15);
		write_array(buf, data->ikHingeAxis, dest->ikHingeAxis, 8, 3);

		if (data->reactiveMotionInfo)
		{
			buf->align(15);
			auto* info = buf->write(data->reactiveMotionInfo);
			write_array(buf, data->reactiveMotionInfo->modelPartTweaks, info->modelPartTweaks, info->numParts, 15);
			write_array(buf, data->reactiveMotionInfo->modelParts, info->modelParts, info->numParts, 15);
			buf->clear_pointer(&dest->reactiveMotionInfo);
		}

		write_asset_array(zone, buf, ASSET_TYPE_MATERIAL, data->materialHandles, dest->materialHandles, data->numsurfs);

		if (data->lodInfo)
		{
			buf->align(7);
			auto* lods = buf->write(data->lodInfo, data->numLods);
			for (auto i = 0; i < data->numLods; i++)
			{
				lods[i].modelSurfsStaging = asset_pointer(zone, ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfsStaging);
			}
			buf->clear_pointer(&dest->lodInfo);
		}

		write_array(buf, data->unknownArray, dest->unknownArray, data->unknownArrayCount, 3);
		write_array(buf, data->boneInfo, dest->boneInfo, bone_count(data), 3);
		write_array(buf, data->himipRadiusInvSq, dest->himipRadiusInvSq, data->numsurfs, 3);
		if (data->physicsType == XMODEL_PHYSICS_RAGDOLL)
		{
			dest->ragdollAsset = asset_pointer(zone, ASSET_TYPE_RAGDOLL_ASSET, data->ragdollAsset);
		}
		else
		{
			dest->physicsAsset = asset_pointer(zone, ASSET_TYPE_PHYSICSASSET, data->physicsAsset);
		}
		dest->physicsFXShape = asset_pointer(zone, ASSET_TYPE_PHYSICS_FX_SHAPE, data->physicsFXShape);
		dest->detailCollision = asset_pointer(zone, ASSET_TYPE_XMODEL_DETAIL_COLLISION, data->detailCollision);
		dest->physicsBoneGraph = asset_pointer(zone, ASSET_TYPE_PHYSICS_BONE_GRAPH, data->physicsBoneGraph);
		write_asset_array(zone, buf, ASSET_TYPE_CLOTH_ASSET, data->clothAssets, dest->clothAssets, data->numClothAssets);

		if (data->blendShapeInfo)
		{
			buf->align(7);
			auto* info = buf->write(data->blendShapeInfo);
			write_array(buf, data->blendShapeInfo->weightNames, info->weightNames, info->numberOfWeights, 3);
			write_array(buf, data->blendShapeInfo->weightMaps, info->weightMaps, info->numberOfWeightMaps, 3);
			buf->clear_pointer(&dest->blendShapeInfo);
		}

		if (data->mdaoVolumes)
		{
			buf->align(7);
			auto* volumes = buf->write(data->mdaoVolumes, data->mdaoVolumeCount);
			for (auto i = 0; i < data->mdaoVolumeCount; i++)
			{
				volumes[i].volumeData = asset_pointer(zone, ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData);
			}
			buf->clear_pointer(&dest->mdaoVolumes);
		}

		if (data->decalVolumesInfo)
		{
			buf->align(7);
			auto* info = buf->write(data->decalVolumesInfo);
			if (data->decalVolumesInfo->decalVolumes)
			{
				buf->align(7);
				auto* volumes = buf->write(data->decalVolumesInfo->decalVolumes, info->numDecalVolumes);
				for (auto i = 0u; i < info->numDecalVolumes; i++)
				{
					const auto& volume = data->decalVolumesInfo->decalVolumes[i];
					volumes[i].material = asset_pointer(zone, ASSET_TYPE_MATERIAL, volume.material);
					volumes[i].blendMapOverride = asset_pointer(zone, ASSET_TYPE_DECAL_VOLUME_MASK, volume.blendMapOverride);
				}
				buf->clear_pointer(&info->decalVolumes);
			}
			buf->clear_pointer(&dest->decalVolumesInfo);
		}

		buf->pop_stream();
	}

	void xmodel::dump(XModel* asset)
	{
		assetmanager::dumper dump;
		if (!dump.open("xmodel\\"s + asset->name + ".xmb"))
		{
			return;
		}

		dump.dump_single(asset);
		dump.dump_string(asset->name);
		dump.dump_array(asset->noScalePartBits, 8);
		dump.dump_asset(asset->scriptableMoverDef);
		dump.dump_asset(asset->proceduralBones);
		dump.dump_asset(asset->dynamicBones);

		dump_script_strings(dump, asset->aimAssistBones, asset->numAimAssistBones);
		dump_script_strings(dump, asset->boneNames, bone_count(asset));
		dump.dump_array(asset->parentList, static_cast<std::uint32_t>(child_bone_count(asset)));
		dump.dump_array(asset->quats, static_cast<std::uint32_t>(4 * child_bone_count(asset)));
		dump.dump_array(asset->trans, static_cast<std::uint32_t>(3 * child_bone_count(asset)));
		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, static_cast<std::uint32_t>(bone_count(asset)));
		dump.dump_array(asset->ikHingeAxis, 8);

		dump.dump_single(asset->reactiveMotionInfo);
		if (asset->reactiveMotionInfo)
		{
			dump.dump_array(asset->reactiveMotionInfo->modelPartTweaks, asset->reactiveMotionInfo->numParts);
			dump.dump_array(asset->reactiveMotionInfo->modelParts, asset->reactiveMotionInfo->numParts);
		}

		dump.dump_array(asset->materialHandles, asset->numsurfs);
		if (asset->materialHandles)
		{
			for (auto i = 0; i < asset->numsurfs; i++)
			{
				dump.dump_asset(asset->materialHandles[i]);
			}
		}

		dump.dump_array(asset->lodInfo, asset->numLods);
		if (asset->lodInfo)
		{
			for (auto i = 0; i < asset->numLods; i++)
			{
				dump.dump_asset(asset->lodInfo[i].modelSurfsStaging);
			}
		}

		dump.dump_array(asset->unknownArray, asset->unknownArrayCount);
		dump.dump_array(asset->boneInfo, static_cast<std::uint32_t>(bone_count(asset)));
		dump.dump_array(asset->himipRadiusInvSq, asset->numsurfs);
		if (asset->physicsType == XMODEL_PHYSICS_RAGDOLL)
		{
			dump.dump_asset(asset->ragdollAsset);
		}
		else
		{
			dump.dump_asset(asset->physicsAsset);
		}
		dump.dump_asset(asset->physicsFXShape);
		dump.dump_asset(asset->detailCollision);
		dump.dump_asset(asset->physicsBoneGraph);

		dump.dump_array(asset->clothAssets, asset->numClothAssets);
		if (asset->clothAssets)
		{
			for (auto i = 0; i < asset->numClothAssets; i++)
			{
				dump.dump_asset(asset->clothAssets[i]);
			}
		}

		dump.dump_single(asset->blendShapeInfo);
		if (asset->blendShapeInfo)
		{
			dump_script_strings(dump, asset->blendShapeInfo->weightNames, asset->blendShapeInfo->numberOfWeights);
			dump.dump_array(asset->blendShapeInfo->weightMaps, asset->blendShapeInfo->numberOfWeightMaps);
		}

		dump.dump_array(asset->mdaoVolumes, asset->mdaoVolumeCount);
		if (asset->mdaoVolumes)
		{
			for (auto i = 0; i < asset->mdaoVolumeCount; i++)
			{
				dump.dump_asset(asset->mdaoVolumes[i].volumeData);
			}
		}

		dump.dump_single(asset->decalVolumesInfo);
		if (asset->decalVolumesInfo)
		{
			dump.dump_array(asset->decalVolumesInfo->decalVolumes, asset->decalVolumesInfo->numDecalVolumes);
			if (asset->decalVolumesInfo->decalVolumes)
			{
				for (auto i = 0u; i < asset->decalVolumesInfo->numDecalVolumes; i++)
				{
					dump.dump_asset(asset->decalVolumesInfo->decalVolumes[i].material);
					dump.dump_asset(asset->decalVolumesInfo->decalVolumes[i].blendMapOverride);
				}
			}
		}

		for (const auto string : asset->unknownStrings)
		{
			dump.dump_string(SL_ConvertToString(string));
		}

		dump.close();
	}
}
