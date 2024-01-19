#include "std_include.hpp"
#include "xmodel.hpp"

namespace zonetool::s1
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

		// tags
		asset->boneNames = read.read_array<scr_string_t>();
		for (unsigned char i = 0; i < asset->numBones; i++)
		{
			this->add_script_string(&asset->boneNames[i], read.read_string());
		}

		// basic info
		asset->parentList = read.read_array<unsigned char>();
		asset->tagAngles = read.read_array<XModelAngle>();
		asset->tagPositions = read.read_array<XModelTagPos>();
		asset->partClassification = read.read_array<unsigned char>();
		asset->baseMat = read.read_array<DObjAnimMat>();
		asset->reactiveMotionParts = read.read_array<ReactiveMotionModelPart>();
		asset->reactiveMotionTweaks = read.read_single<ReactiveMotionModelTweaks>();
		asset->collSurfs = read.read_array<XModelCollSurf_s>();
		asset->boneInfo = read.read_array<XBoneInfo>();
		asset->invHighMipRadius = read.read_array<unsigned short>();

		// surfaces
		asset->materialHandles = read.read_array<Material*>();
		for (unsigned char i = 0; i < asset->numsurfs; i++)
		{
			asset->materialHandles[i] = read.read_asset<Material>();
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			asset->lodInfo[i].modelSurfs = read.read_asset<XModelSurfs>();
		}

		// subassets
		asset->physPreset = read.read_asset<PhysPreset>();
		asset->physCollmap = read.read_asset<PhysCollmap>();

		// weights
		asset->weightNames = read.read_array<scr_string_t>();
		for (unsigned short i = 0; i < asset->numberOfWeights; i++)
		{
			this->add_script_string(&asset->weightNames[i], read.read_string());
		}

		// blendshape
		asset->blendShapeWeightMap = read.read_array<BlendShapeWeightMap>();

		// mdao
		asset->mdaoVolumes = read.read_array<MdaoVolume>();
		for (auto i = 0; i < asset->mdaoVolumeCount; i++)
		{
			asset->mdaoVolumes[i].volumeData = read.read_asset<GfxImage>();
		}

		// extra models
		asset->compositeModels = read.read_array<XModel*>();
		for (char i = 0; i < asset->numCompositeModels; i++)
		{
			asset->compositeModels[i] = read.read_asset<XModel>();
		}

		// skeletonscript subasset
		asset->skeletonScript = read.read_asset<SkeletonScript>();

		// bone physics
		asset->bonePhysics = read.read_array<XPhysBoneInfo>();
		for (char i = 0; i < asset->numBonePhysics; i++)
		{
			asset->bonePhysics[i].physPreset = read.read_asset<PhysPreset>();
			asset->bonePhysics[i].physContraint = read.read_asset<PhysConstraint>();
			asset->bonePhysics[i].physCollmap = read.read_asset<PhysCollmap>();
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

			auto* original_scriptstrings = asset->boneNames;
			asset->boneNames = mem->allocate<scr_string_t>(asset->numBones);
			for (unsigned char i = 0; i < asset->numBones; i++)
			{
				this->add_script_string(&asset->boneNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}

			auto* original_weights = asset->weightNames;
			asset->weightNames = mem->allocate<scr_string_t>(asset->numberOfWeights);
			for (unsigned char i = 0; i < asset->numberOfWeights; i++)
			{
				this->add_script_string(&asset->weightNames[i], SL_ConvertToString(original_weights[i]));
			}
		}
	}

	void xmodel::prepare(zone_buffer* buf, zone_memory* mem)
	{
		// fixup scriptstrings
		auto* xmodel = this->asset_;

		// name bonenames
		if (xmodel->boneNames)
		{
			for (unsigned char i = 0; i < xmodel->numBones; i++)
			{
				xmodel->boneNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->boneNames[i])));
			}
		}

		// name weights
		if (xmodel->weightNames)
		{
			for (unsigned short i = 0; i < xmodel->numberOfWeights; i++)
			{
				xmodel->weightNames[i] = static_cast<scr_string_t>(
					buf->write_scriptstring(this->get_script_string(&xmodel->weightNames[i])));
			}
		}
	}

	void xmodel::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		// Materials
		for (unsigned char i = 0; i < data->numsurfs; i++)
		{
			if (data->materialHandles[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materialHandles[i]->name);
			}
		}

		// XSurfaces
		for (auto i = 0; i < 6; i++)
		{
			if (data->lodInfo[i].modelSurfs)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODELSURFS, data->lodInfo[i].modelSurfs->name);
			}
		}

		// PhysCollmap
		if (data->physCollmap)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSCOLLMAP, data->physCollmap->name);
		}

		// PhysPreset
		if (data->physPreset)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSPRESET, data->physPreset->name);
		}

		// GfxImage
		if (data->mdaoVolumes)
		{
			for (auto i = 0; i < data->mdaoVolumeCount; i++)
			{
				zone->add_asset_of_type(ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name);
			}
		}

		// Sub XModels
		if (data->compositeModels)
		{
			for (char i = 0; i < data->numCompositeModels; i++)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL, data->compositeModels[i]->name);
			}
		}

		// SkeletonScript
		if (data->skeletonScript)
		{
			zone->add_asset_of_type(ASSET_TYPE_SKELETON_SCRIPT, data->skeletonScript->name);
		}

		// Bone Physics
		if (data->bonePhysics)
		{
			for (unsigned char i = 0; i < data->numBonePhysics; i++)
			{
				if (data->bonePhysics[i].physPreset)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSPRESET, data->bonePhysics[i].physPreset->name);
				}
				if (data->bonePhysics[i].physContraint)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSCONSTRAINT, data->bonePhysics[i].physContraint->name);
				}
				if (data->bonePhysics[i].physCollmap)
				{
					zone->add_asset_of_type(ASSET_TYPE_PHYSCOLLMAP, data->bonePhysics[i].physCollmap->name);
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

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->name = buf->write_str(this->name());

		if (data->boneNames)
		{
			buf->align(3);
			buf->write(data->boneNames, data->numBones);
			buf->clear_pointer(&dest->boneNames);
		}

		if (data->parentList)
		{
			buf->align(0);
			buf->write(data->parentList, (data->numBones - data->numRootBones));
			buf->clear_pointer(&dest->parentList);
		}

		if (data->tagAngles)
		{
			buf->align(1);
			buf->write(data->tagAngles, (data->numBones - data->numRootBones));
			buf->clear_pointer(&dest->tagAngles);
		}

		if (data->tagPositions)
		{
			buf->align(3);
			buf->write(data->tagPositions, (data->numBones - data->numRootBones));
			buf->clear_pointer(&dest->tagPositions);
		}

		if (data->partClassification)
		{
			buf->align(0);
			buf->write(data->partClassification, data->numBones);
			buf->clear_pointer(&dest->partClassification);
		}

		if (data->baseMat)
		{
			buf->align(3);
			buf->write(data->baseMat, data->numBones);
			buf->clear_pointer(&dest->baseMat);
		}

		if (data->reactiveMotionParts)
		{
			buf->align(15);
			buf->write(data->reactiveMotionParts, data->numReactiveMotionParts);
			buf->clear_pointer(&dest->reactiveMotionParts);
		}

		if (data->reactiveMotionTweaks)
		{
			buf->align(15);
			buf->write(data->reactiveMotionTweaks);
			buf->clear_pointer(&dest->reactiveMotionTweaks);
		}

		buf->inc_stream(5, 4 * data->numsurfs);
		if (data->materialHandles)
		{
			buf->align(7);
			auto* dest_materials = buf->write(data->materialHandles, data->numsurfs);
			for (unsigned char i = 0; i < data->numsurfs; i++)
			{
				dest_materials[i] = reinterpret_cast<Material*>(zone->get_asset_pointer(
					ASSET_TYPE_MATERIAL, data->materialHandles[i]->name));
			}
		}

		for (auto i = 0; i < 6; i++)
		{
			if (!data->lodInfo[i].modelSurfs) continue;
			dest->lodInfo[i].modelSurfs = reinterpret_cast<XModelSurfs*>(zone->get_asset_pointer(
				ASSET_TYPE_XMODELSURFS, data->lodInfo[i].modelSurfs->name));
		}

		if (data->collSurfs)
		{
			buf->align(3);
			buf->write(data->collSurfs, data->numCollSurfs);
			buf->clear_pointer(&dest->collSurfs);
		}

		if (data->boneInfo)
		{
			buf->align(3);
			buf->write(data->boneInfo, data->numBones);
			buf->clear_pointer(&dest->boneInfo);
		}

		if (data->invHighMipRadius)
		{
			buf->align(1);
			buf->write(data->invHighMipRadius, data->numsurfs);
			buf->clear_pointer(&dest->invHighMipRadius);
		}

		if (data->weightNames)
		{
			buf->align(3);
			buf->write(data->weightNames, data->numberOfWeights);
			buf->clear_pointer(&dest->weightNames);
		}
		//dest->weightNames = nullptr;
		//dest->numberOfWeights = 0;
		//dest->targetCount = 0;

		if (data->blendShapeWeightMap)
		{
			buf->align(3);
			buf->write(data->blendShapeWeightMap, data->numberOfWeightMaps);
			buf->clear_pointer(&dest->blendShapeWeightMap);
		}
		//dest->blendShapeWeightMap = nullptr;
		//dest->numberOfWeightMaps = 0;

		if (data->physPreset)
		{
			dest->physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
				ASSET_TYPE_PHYSPRESET, data->physPreset->name));
		}

		if (data->physCollmap)
		{
			dest->physCollmap = reinterpret_cast<PhysCollmap*>(zone->get_asset_pointer(
				ASSET_TYPE_PHYSCOLLMAP, data->physCollmap->name));
		}

		if (data->mdaoVolumes)
		{
			buf->align(3);
			auto* dest_mdaoVolumes = buf->write(data->mdaoVolumes, data->mdaoVolumeCount);
			for (unsigned short i = 0; i < data->mdaoVolumeCount; i++)
			{
				dest_mdaoVolumes[i].volumeData = reinterpret_cast<GfxImage*>(zone->get_asset_pointer(
					ASSET_TYPE_IMAGE, data->mdaoVolumes[i].volumeData->name));
			}
			buf->clear_pointer(&dest->mdaoVolumes);
		}
		//dest->mdaoVolumes = nullptr;
		//dest->mdaoVolumeCount = 0;

		if (data->compositeModels)
		{
			buf->align(7);
			auto* dest_compositeModels = buf->write(data->compositeModels, data->numCompositeModels);
			for (char i = 0; i < data->numCompositeModels; i++)
			{
				dest_compositeModels[i] = reinterpret_cast<XModel*>(zone->get_asset_pointer(
					ASSET_TYPE_XMODEL, data->compositeModels[i]->name));
			}
			buf->clear_pointer(&dest->compositeModels);
		}
		//dest->compositeModels = nullptr;
		//dest->numCompositeModels = 0;

		if (data->skeletonScript)
		{
			dest->skeletonScript = reinterpret_cast<SkeletonScript*>(zone->get_asset_pointer(
				ASSET_TYPE_SKELETON_SCRIPT, data->skeletonScript->name));
		}
		//dest->skeletonScript = nullptr;

		if (data->bonePhysics)
		{
			buf->align(3);
			auto* dest_bonePhysics = buf->write(data->bonePhysics, data->numBonePhysics);
			for (unsigned char i = 0; i < data->numBonePhysics; i++)
			{
				if (data->bonePhysics[i].physPreset)
				{
					dest_bonePhysics[i].physPreset = reinterpret_cast<PhysPreset*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSPRESET, data->bonePhysics[i].physPreset->name));
				}
				if (data->bonePhysics[i].physContraint)
				{
					dest_bonePhysics[i].physContraint = reinterpret_cast<PhysConstraint*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSCONSTRAINT, data->bonePhysics[i].physContraint->name));
				}
				if (data->bonePhysics[i].physCollmap)
				{
					dest_bonePhysics[i].physCollmap = reinterpret_cast<PhysCollmap*>(zone->get_asset_pointer(
						ASSET_TYPE_PHYSCOLLMAP, data->bonePhysics[i].physCollmap->name));
				}
			}
			buf->clear_pointer(&dest->bonePhysics);
		}
		//dest->bonePhysics = nullptr;
		//dest->numBonePhysics = 0;

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

		// tags
		dump.dump_array(asset->boneNames, asset->numBones);
		for (unsigned char i = 0; i < asset->numBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
		}

		// basic info
		dump.dump_array(asset->parentList, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->tagAngles, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->tagPositions, asset->numBones - asset->numRootBones);
		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, asset->numBones);
		dump.dump_array(asset->reactiveMotionParts, asset->numReactiveMotionParts);
		dump.dump_single(asset->reactiveMotionTweaks);
		dump.dump_array(asset->collSurfs, asset->numCollSurfs);
		dump.dump_array(asset->boneInfo, asset->numBones);
		dump.dump_array(asset->invHighMipRadius, asset->numsurfs);

		// surfaces
		dump.dump_array(asset->materialHandles, asset->numsurfs);
		for (unsigned char i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_asset(asset->materialHandles[i]);
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			dump.dump_asset(asset->lodInfo[i].modelSurfs);
		}

		// physics subassets
		dump.dump_asset(asset->physPreset);
		dump.dump_asset(asset->physCollmap);

		// weights
		dump.dump_array(asset->weightNames, asset->numberOfWeights);
		for (unsigned short i = 0; i < asset->numberOfWeights; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->weightNames[i]));
		}

		// blendshapeweights
		dump.dump_array(asset->blendShapeWeightMap, asset->numberOfWeightMaps);

		// mdao
		dump.dump_array(asset->mdaoVolumes, asset->mdaoVolumeCount);
		for (auto i = 0; i < asset->mdaoVolumeCount; i++)
		{
			dump.dump_asset(asset->mdaoVolumes[i].volumeData);
		}

		// extra models
		dump.dump_array(asset->compositeModels, asset->numCompositeModels);
		for (char i = 0; i < asset->numCompositeModels; i++)
		{
			dump.dump_asset(asset->compositeModels[i]);
		}

		// skeletonscript subasset
		dump.dump_asset(asset->skeletonScript);

		// bone physics
		dump.dump_array(asset->bonePhysics, asset->numBonePhysics);
		for (char i = 0; i < asset->numBonePhysics; i++)
		{
			dump.dump_asset(asset->bonePhysics[i].physPreset);
			dump.dump_asset(asset->bonePhysics[i].physContraint);
			dump.dump_asset(asset->bonePhysics[i].physCollmap);
		}

		dump.close();
	}
}