#include "std_include.hpp"
#include "xmodel.hpp"

namespace zonetool::iw8
{
	void xmodel::add_script_string(scr_string_t* ptr, const char* str)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return;
			}
		}
		this->script_strings.push_back(std::pair<scr_string_t*, const char*>(ptr, str));
	}

	const char* xmodel::get_script_string(scr_string_t* ptr)
	{
		for (std::uint32_t i = 0; i < this->script_strings.size(); i++)
		{
			if (this->script_strings[i].first == ptr)
			{
				return this->script_strings[i].second;
			}
		}
		return nullptr;
	}

	XModel* xmodel::parse(std::string name, zone_memory* mem)
	{
		const auto path = "xmodel\\"s + name + ".xmb";

		assetmanager::reader read(mem);
		if (!read.open(path))
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing xmodel \"%s\"...", name.data());

		// asset
		auto* asset = read.read_single<XModel>();
		asset->name = read.read_string();
		
		asset->scriptableMoverDef = read.read_asset<ScriptableDef>();
		asset->proceduralBones = read.read_asset<XAnimProceduralBones>();
		asset->dynamicBones = read.read_asset<XAnimDynamicBones>();

		// tags
		asset->aimAssistBones = mem->allocate<scr_string_t>(asset->numAimAssistBones);
		for (auto i = 0; i < asset->numAimAssistBones; i++)
		{
			this->add_script_string(&asset->aimAssistBones[i], read.read_string());
		}

		asset->boneNames = mem->allocate<scr_string_t>(asset->numBones + asset->numClientBones);
		for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
		{
			this->add_script_string(&asset->boneNames[i], read.read_string());
		}

		// basic info
		asset->parentList = read.read_array<char>();
		asset->quats = read.read_array<__int16>();
		asset->trans = read.read_array<float>();

		asset->partClassification = read.read_array<char>();
		asset->baseMat = read.read_array<DObjAnimMat>();
		asset->ikHingeAxis = read.read_array<vec3_t>();

		if (asset->reactiveMotionInfo)
		{
			asset->reactiveMotionInfo = read.read_single<ReactiveMotionModelInfo>();
			asset->reactiveMotionInfo->modelPartTweaks = read.read_array<ReactiveMotionModelPartTweaks>();
			asset->reactiveMotionInfo->modelParts = read.read_array<ReactiveMotionModelPart>();
		}

		// surfaces
		asset->materialHandles = read.read_array<Material*>();
		for (unsigned int i = 0; i < asset->numsurfs; i++)
		{
			asset->materialHandles[i] = read.read_asset<Material>();
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			asset->lodInfo[i].modelSurfsStaging = read.read_asset<XModelSurfs>();
		}

		asset->boneInfo = read.read_array<XBoneInfo>();
		asset->himipRadiusInvSq = read.read_array<float>();

		// physics
		asset->physicsAsset = read.read_asset<PhysicsAsset>();
		asset->physicsFXShape = read.read_asset<PhysicsFXShape>();

		if (asset->detailCollision)
		{
			asset->detailCollision = read.read_single<XModelDetailCollision>();
			asset->detailCollision->physicsLODData = read.read_array<char>();
			for (auto i = 0u; i < asset->detailCollision->physicsLODDataNameCount; i++)
			{
				this->add_script_string(&asset->detailCollision->physicsLODDataNames[i], read.read_string());
			}
		}

		if (asset->clothAssets)
		{
			asset->clothAssets = read.read_array<ClothAsset*>();
			for (auto i = 0; i < asset->numClothAssets; i++)
			{
				asset->clothAssets[i] = read.read_asset<ClothAsset>();
			}
		}

		if (asset->blendShapeInfo)
		{
			asset->blendShapeInfo = read.read_single<XModelBlendShapeInfo>();
			for (auto i = 0; i < asset->blendShapeInfo->numberOfWeights; i++)
			{
				this->add_script_string(&asset->blendShapeInfo->weightNames[i], read.read_string());
			}
			asset->blendShapeInfo->weightMaps = read.read_array<BlendShapeWeightMap>();
		}

		if (asset->mdaoVolumes)
		{
			asset->mdaoVolumes = read.read_array<MdaoVolume>();
			for (auto i = 0; i < asset->mdaoVolumeCount; i++)
			{
				asset->mdaoVolumes[i].volumeData = read.read_asset<GfxImage>();
			}
		}

		if (asset->decalVolumesInfo)
		{
			asset->decalVolumesInfo = read.read_single<XModelDecalVolumesInfo>();
			asset->decalVolumesInfo->decalVolumes = read.read_array<XModelDecalVolume>();
			for (auto i = 0u; i < asset->decalVolumesInfo->numDecalVolumes; i++)
			{
				if (asset->decalVolumesInfo->decalVolumes[i].material)
				{
					asset->decalVolumesInfo->decalVolumes[i].material = read.read_asset<Material>();
				}
				if (asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->name)
				{
					asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->name = read.read_string();
				}
				if (asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->blendMap)
				{
					asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->blendMap = read.read_asset<GfxImage>();
				}
			}
		}

		read.close();

		return asset;
	}

	void xmodel::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_copy<XModel>(XAssetType(this->type()), this->name_.data(), mem).model;

			auto* asset = this->asset_;

			auto* original_scriptstrings = asset->aimAssistBones;
			asset->aimAssistBones = mem->allocate<scr_string_t>(asset->numAimAssistBones);
			for (char i = 0; i < asset->numAimAssistBones; i++)
			{
				this->add_script_string(&asset->aimAssistBones[i], SL_ConvertToString(original_scriptstrings[i]));
			}

			original_scriptstrings = asset->boneNames;
			asset->boneNames = mem->allocate<scr_string_t>(asset->numBones + asset->numClientBones);
			for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
			{
				this->add_script_string(&asset->boneNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}

			original_scriptstrings = asset->blendShapeInfo->weightNames;
			asset->blendShapeInfo->weightNames = mem->allocate<scr_string_t>(asset->blendShapeInfo->numberOfWeights);
			for (auto i = 0; i < asset->blendShapeInfo->numberOfWeights; i++)
			{
				this->add_script_string(&asset->blendShapeInfo->weightNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}
		}
	}

	void xmodel::prepare(zone_buffer* buf, zone_memory* mem)
	{
		// fixup scriptstrings
		auto* xmodel = this->asset_;

		if (xmodel->aimAssistBones)
		{
			for (auto i = 0; i < xmodel->numAimAssistBones; i++)
			{
				xmodel->aimAssistBones[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->aimAssistBones[i])));
			}
		}

		if (xmodel->boneNames)
		{
			for (auto i = 0; i < xmodel->numBones + xmodel->numClientBones; i++)
			{
				xmodel->boneNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->boneNames[i])));
			}
		}

		if (xmodel->blendShapeInfo->weightNames)
		{
			for (auto i = 0; i < xmodel->blendShapeInfo->numberOfWeights; i++)
			{
				xmodel->blendShapeInfo->weightNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->blendShapeInfo->weightNames[i])));
			}
		}
	}

	void xmodel::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		//
		if (data->scriptableMoverDef)
		{
			zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMoverDef->name);
		}

		if (data->proceduralBones)
		{
			zone->add_asset_of_type(ASSET_TYPE_XANIM_PROCEDURALBONES, data->proceduralBones->name);
		}

		if (data->dynamicBones)
		{
			zone->add_asset_of_type(ASSET_TYPE_XANIM_DYNAMICBONES, data->dynamicBones->name);
		}

		// materials
		for (unsigned int i = 0; i < data->numsurfs; i++)
		{
			if (data->materialHandles[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materialHandles[i]->name);
			}
		}

		// surfaces
		for (auto i = 0; i < 6; i++)
		{
			if (data->lodInfo[i].modelSurfsStaging)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfsStaging->name);
			}
		}

		// physics
		if (data->physicsAsset)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSICSASSET, data->physicsAsset->name);
		}
		if (data->physicsFXShape)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSICS_FX_SHAPE, data->physicsFXShape->name);
		}

		if (data->clothAssets)
		{
			for (auto i = 0; i < data->numClothAssets; i++)
			{
				if (data->clothAssets[i])
				{
					zone->add_asset_of_type(ASSET_TYPE_CLOTH_ASSET, data->clothAssets[i]->name);
				}
			}
		}

		if (data->mdaoVolumes)
		{
			for (auto i = 0; i < data->mdaoVolumeCount; i++)
			{
				if (data->mdaoVolumes[i].volumeData)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name);
				}
			}
		}

		if (data->decalVolumesInfo)
		{
			for (auto i = 0u; i < data->decalVolumesInfo->numDecalVolumes; i++)
			{
				if (data->decalVolumesInfo->decalVolumes[i].material)
				{
					zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->decalVolumesInfo->decalVolumes[i].material->name);
				}
				if (data->decalVolumesInfo->decalVolumes[i].blendMapOverride->blendMap)
				{
					zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->decalVolumesInfo->decalVolumes[i].blendMapOverride->blendMap->name);
				}
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
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(5);

		dest->name = buf->write_str(this->name());

		if (data->scriptableMoverDef)
		{
			dest->scriptableMoverDef = reinterpret_cast<ScriptableDef*>(zone->get_asset_pointer(
				ASSET_TYPE_SCRIPTABLE, data->scriptableMoverDef->name));
		}

		if (data->proceduralBones)
		{
			dest->proceduralBones = reinterpret_cast<XAnimProceduralBones*>(zone->get_asset_pointer(
				ASSET_TYPE_XANIM_PROCEDURALBONES, data->proceduralBones->name));
		}

		if (data->dynamicBones)
		{
			dest->dynamicBones = reinterpret_cast<XAnimDynamicBones*>(zone->get_asset_pointer(
				ASSET_TYPE_XANIM_DYNAMICBONES, data->dynamicBones->name));
		}

		if (data->aimAssistBones)
		{
			buf->align(3);
			buf->write(data->aimAssistBones, data->numAimAssistBones);
			buf->clear_pointer(&dest->aimAssistBones);
		}

		if (data->boneNames)
		{
			buf->align(3);
			buf->write(data->boneNames, data->numBones + data->numClientBones);
			buf->clear_pointer(&dest->boneNames);
		}

		if (data->parentList)
		{
			buf->align(0);
			buf->write(data->parentList, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->parentList);
		}

		if (data->quats)
		{
			buf->align(1);
			buf->write(data->quats, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->quats);
		}

		if (data->trans)
		{
			buf->align(3);
			buf->write(data->trans, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->trans);
		}

		if (data->trans)
		{
			buf->align(3);
			buf->write(data->trans, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->trans);
		}

		if (data->partClassification)
		{
			buf->align(0);
			buf->write(data->partClassification, data->numBones);
			buf->clear_pointer(&dest->partClassification);
		}

		if (data->baseMat)
		{
			buf->align(15);
			buf->write(data->baseMat, data->numBones + data->numClientBones);
			buf->clear_pointer(&dest->baseMat);
		}

		if (data->ikHingeAxis)
		{
			buf->align(3);
			buf->write(data->ikHingeAxis, 4);
			buf->clear_pointer(&dest->ikHingeAxis);
		}

		if (data->reactiveMotionInfo)
		{
			buf->align(15);
			buf->write(data->reactiveMotionInfo);
			buf->clear_pointer(&dest->reactiveMotionInfo);
		}

		//buf->inc_stream(XFILE_BLOCK_CALLBACK, 4 * data->numsurfs); // DB_CreateMaterialStream doesnt exist in IW8
		if (data->materialHandles)
		{
			buf->align(7);
			auto* dest_materials = buf->write(data->materialHandles, data->numsurfs);
			for (unsigned int i = 0; i < data->numsurfs; i++)
			{
				dest_materials[i] = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, data->materialHandles[i]->name));
			}
		}

		for (auto i = 0; i < 6; i++)
		{
			if (!data->lodInfo[i].modelSurfsStaging) continue;
			dest->lodInfo[i].modelSurfsStaging = reinterpret_cast<XModelSurfs*>(zone->get_asset_pointer(
				ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfsStaging->name));
		}

		if (data->boneInfo)
		{
			buf->align(3);
			buf->write(data->boneInfo, data->numBones + data->numClientBones);
			buf->clear_pointer(&dest->boneInfo);
		}

		if (data->himipRadiusInvSq)
		{
			buf->align(3);
			buf->write(data->himipRadiusInvSq, data->numsurfs);
			buf->clear_pointer(&dest->himipRadiusInvSq);
		}

		if (data->physicsAsset)
		{
			dest->physicsAsset = reinterpret_cast<PhysicsAsset*>(zone->get_asset_pointer(
				ASSET_TYPE_PHYSICSASSET, data->physicsAsset->name));
		}

		if (data->physicsFXShape)
		{
			dest->physicsFXShape = reinterpret_cast<PhysicsFXShape*>(zone->get_asset_pointer(
				ASSET_TYPE_PHYSICS_FX_SHAPE, data->physicsFXShape->name));
		}

		if (data->detailCollision)
		{
			dest->detailCollision = reinterpret_cast<XModelDetailCollision*>(zone->get_asset_pointer(
				ASSET_TYPE_XMODEL_DETAIL_COLLISION, data->detailCollision->name));
		}

		if (data->clothAssets)
		{
			buf->align(7);
			buf->write(data->clothAssets, data->numClothAssets);
			buf->clear_pointer(&dest->clothAssets);
		}

		if (data->blendShapeInfo)
		{
			buf->align(7);
			buf->write(data->blendShapeInfo);
			buf->clear_pointer(&dest->blendShapeInfo);
		}

		if (data->mdaoVolumes)
		{
			buf->align(7);
			auto* dest_mdaoVolumes = buf->write(data->mdaoVolumes, data->mdaoVolumeCount);
			for (unsigned short i = 0; i < data->mdaoVolumeCount; i++)
			{
				dest_mdaoVolumes[i].volumeData = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name));
			}
			buf->clear_pointer(&dest->mdaoVolumes);
		}

		if (data->decalVolumesInfo)
		{
			buf->align(7);
			buf->write(data->decalVolumesInfo);
			buf->clear_pointer(&dest->decalVolumesInfo);
		}

		buf->pop_stream();
	}

	void xmodel::dump(XModel* asset)
	{
		const auto path = "xmodel\\"s + asset->name + ".xmb";

		assetmanager::dumper dump;
		if (!dump.open(path))
		{
			return;
		}

		// asset
		dump.dump_single(asset);
		dump.dump_string(asset->name);

		dump.dump_asset(asset->scriptableMoverDef);
		dump.dump_asset(asset->proceduralBones);
		dump.dump_asset(asset->dynamicBones);

		// tags
		for (auto i = 0; i < asset->numAimAssistBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->aimAssistBones[i]));
		}

		for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
		}

		dump.dump_array(asset->parentList, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->quats, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->trans, asset->numBones + asset->numClientBones - asset->numRootBones);

		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->ikHingeAxis, 4); // Load_vec3_tArray(0, 4LL);

		if (asset->reactiveMotionInfo)
		{
			dump.dump_single(asset->reactiveMotionInfo);
			dump.dump_single(asset->reactiveMotionInfo->modelPartTweaks);
			dump.dump_single(asset->reactiveMotionInfo->modelParts);
		}

		// surfaces
		dump.dump_array(asset->materialHandles, asset->numsurfs);
		for (unsigned int i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_asset(asset->materialHandles[i]);
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			dump.dump_asset(asset->lodInfo[i].modelSurfsStaging);
			//dump.dump_asset(asset->lodInfo[i].surfs);
		}

		dump.dump_array(asset->boneInfo, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->himipRadiusInvSq, asset->numsurfs);

		// physics
		dump.dump_asset(asset->physicsAsset);
		dump.dump_asset(asset->physicsFXShape);

		if (asset->detailCollision)
		{
			dump.dump_single(asset->detailCollision); // Load_XModelDetailCollisionPtr
			dump.dump_array(asset->detailCollision->physicsLODData, asset->detailCollision->physicsLODDataSize);
			for (unsigned int i = 0; i < asset->detailCollision->physicsLODDataNameCount; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->detailCollision->physicsLODDataNames[i]));
			}
		}

		if (asset->clothAssets)
		{
			dump.dump_array(asset->clothAssets, asset->numClothAssets);
			for (auto i = 0; i < asset->numClothAssets; i++)
			{
				dump.dump_asset(asset->clothAssets[i]);
			}
		}

		if (asset->blendShapeInfo)
		{
			dump.dump_single(asset->blendShapeInfo);
			for (auto i = 0; i < asset->blendShapeInfo->numberOfWeights; i++)
			{
				dump.dump_string(SL_ConvertToString(asset->blendShapeInfo->weightNames[i]));
			}
			dump.dump_array(asset->blendShapeInfo->weightMaps, asset->blendShapeInfo->numberOfWeightMaps);
		}

		if (asset->mdaoVolumes)
		{
			dump.dump_array(asset->mdaoVolumes, asset->mdaoVolumeCount);
			for (auto i = 0; i < asset->mdaoVolumeCount; i++)
			{
				dump.dump_asset(asset->mdaoVolumes[i].volumeData);
			}
		}

		if (asset->decalVolumesInfo)
		{
			dump.dump_single(asset->decalVolumesInfo);
			dump.dump_array(asset->decalVolumesInfo->decalVolumes, asset->decalVolumesInfo->numDecalVolumes);
			for (auto i = 0u; i < asset->decalVolumesInfo->numDecalVolumes; i++)
			{
				dump.dump_asset(asset->decalVolumesInfo->decalVolumes[i].material);
				dump.dump_string(asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->name);
				dump.dump_asset(asset->decalVolumesInfo->decalVolumes[i].blendMapOverride->blendMap);
			}
		}

		dump.close();
	}
}
