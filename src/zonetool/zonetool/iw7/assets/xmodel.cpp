#include "std_include.hpp"
#include "xmodel.hpp"

namespace zonetool::iw7
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
		asset->boneNames = mem->allocate<scr_string_t>(asset->numBones + asset->numClientBones);
		for (unsigned char i = 0; i < asset->numBones + asset->numClientBones; i++)
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

		// physics
		asset->physicsAsset = read.read_asset<PhysicsAsset>();
		asset->physicsFXShape = read.read_asset<PhysicsFXShape>();

		// unknown
		asset->unknown02 = read.read_array<char>();

		asset->unknownNames = mem->allocate<scr_string_t>(asset->unknownNamesCount);
		for (unsigned int i = 0; i < asset->unknownNamesCount; i++)
		{
			this->add_script_string(&asset->unknownNames[i], read.read_string());
		}

		asset->scriptableMoverDef = read.read_asset<ScriptableDef>();
		asset->proceduralBones = read.read_asset<XAnimProceduralBones>();

		asset->aimAssistBones = mem->allocate<scr_string_t>(asset->numAimAssistBones);
		for (unsigned int i = 0; i < asset->numAimAssistBones; i++)
		{
			this->add_script_string(&asset->aimAssistBones[i], read.read_string());
		}

		asset->unknown03 = read.read_array<char>();
		asset->unknownVec3 = read.read_array<vec3_t>();
		asset->unknown04 = read.read_array<unk_1453E14D8>();

		read.close();

		/*std::string name_e = asset->name;
		if (name_e.find("viewmodel_ak47") != std::string::npos)
		{
			auto asset2 = db_find_x_asset_header_copy<XModel>(XAssetType(this->type()), "weapon_ftlpistol_vm", mem).model;
			XModel iw7_asset_{};
			XModel* iw7_asset = &iw7_asset_;
			memcpy(iw7_asset, asset2, sizeof(XModel));

			iw7_asset->name = asset->name;
			iw7_asset->numBones = asset->numBones;
			iw7_asset->numRootBones = asset->numRootBones;
			iw7_asset->numsurfs = asset->numsurfs;
			iw7_asset->numReactiveMotionParts = 0;
			iw7_asset->scale = asset->scale;
			memcpy(&iw7_asset->noScalePartBits, &asset->noScalePartBits, sizeof(asset->noScalePartBits));

			iw7_asset->boneNames = asset->boneNames;

			iw7_asset->parentList = asset->parentList;
			iw7_asset->tagAngles = asset->tagAngles;
			iw7_asset->tagPositions = asset->tagPositions;
			iw7_asset->partClassification = asset->partClassification;
			iw7_asset->baseMat = asset->baseMat;
			iw7_asset->reactiveMotionParts = nullptr;

			iw7_asset->materialHandles = asset->materialHandles;

			for (auto i = 0; i < 6; i++)
			{
				iw7_asset->lodInfo[i].dist = 1000000.0f;
			}

			// level of detail data
			for (auto i = 0; i < asset->numLods; i++)
			{
				iw7_asset->lodInfo[i].dist = asset->lodInfo[i].dist;
				iw7_asset->lodInfo[i].numsurfs = asset->lodInfo[i].numsurfs;
				iw7_asset->lodInfo[i].surfIndex = asset->lodInfo[i].surfIndex;
				iw7_asset->lodInfo[i].modelSurfs = asset->lodInfo[i].modelSurfs;
				memcpy(&iw7_asset->lodInfo[i].partBits, &asset->lodInfo[i].partBits, sizeof(asset->lodInfo[i].partBits));
			}

			iw7_asset->maxLoadedLod = asset->maxLoadedLod;
			iw7_asset->numLods = asset->numLods;
			iw7_asset->collLod = asset->collLod;
			//iw7_asset->flags = asset->flags;

			iw7_asset->numCollSurfs = asset->numCollSurfs;
			iw7_asset->collSurfs = asset->collSurfs;

			iw7_asset->contents = asset->contents;

			iw7_asset->boneInfo = asset->boneInfo;

			iw7_asset->radius = asset->radius;
			memcpy(&iw7_asset->bounds, &asset->bounds, sizeof(asset->bounds));
			iw7_asset->memUsage = asset->memUsage;

			// idk
			//iw7_asset->invHighMipRadius = asset->invHighMipRadius;

			//iw7_asset->quantization = 0.0f;

			iw7_asset->unknownIndex = 0xFF;
			iw7_asset->unknownIndex2 = 0xFF;

			//iw7_asset->flags = 0x40;

			memcpy(asset, iw7_asset, sizeof(XModel));
		}*/

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
			asset->boneNames = mem->allocate<scr_string_t>(asset->numBones + asset->numClientBones);
			for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
			{
				this->add_script_string(&asset->boneNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}

			original_scriptstrings = asset->unknownNames;
			asset->unknownNames = mem->allocate<scr_string_t>(asset->unknownNamesCount);
			for (unsigned int i = 0; i < asset->unknownNamesCount; i++)
			{
				this->add_script_string(&asset->unknownNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}

			original_scriptstrings = asset->aimAssistBones;
			asset->aimAssistBones = mem->allocate<scr_string_t>(asset->numAimAssistBones);
			for (unsigned int i = 0; i < asset->numAimAssistBones; i++)
			{
				this->add_script_string(&asset->aimAssistBones[i], SL_ConvertToString(original_scriptstrings[i]));
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
			for (auto i = 0; i < xmodel->numBones + xmodel->numClientBones; i++)
			{
				xmodel->boneNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->boneNames[i])));
			}

			for (unsigned int i = 0; i < xmodel->unknownNamesCount; i++)
			{
				xmodel->unknownNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->unknownNames[i])));
			}

			for (unsigned int i = 0; i < xmodel->numAimAssistBones; i++)
			{
				xmodel->aimAssistBones[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->aimAssistBones[i])));
			}
		}
	}

	void xmodel::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		// materials
		for (unsigned char i = 0; i < data->numsurfs; i++)
		{
			if (data->materialHandles[i])
			{
				zone->add_asset_of_type(ASSET_TYPE_MATERIAL, data->materialHandles[i]->name);
			}
		}

		// surfaces
		for (auto i = 0; i < 6; i++)
		{
			if (data->lodInfo[i].modelSurfs)
			{
				zone->add_asset_of_type(ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfs->name);
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

		//
		if (data->scriptableMoverDef)
		{
			zone->add_asset_of_type(ASSET_TYPE_SCRIPTABLE, data->scriptableMoverDef->name);
		}

		if (data->proceduralBones)
		{
			zone->add_asset_of_type(ASSET_TYPE_XANIM_PROCEDURALBONES, data->proceduralBones->name);
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
			buf->write(data->boneNames, data->numBones + data->numClientBones);
			buf->clear_pointer(&dest->boneNames);
		}

		if (data->parentList)
		{
			buf->align(0);
			buf->write(data->parentList, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->parentList);
		}

		if (data->tagAngles)
		{
			buf->align(1);
			buf->write(data->tagAngles, (data->numBones + data->numClientBones - data->numRootBones));
			buf->clear_pointer(&dest->tagAngles);
		}

		if (data->tagPositions)
		{
			buf->align(3);
			buf->write(data->tagPositions, (data->numBones + data->numClientBones - data->numRootBones));
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
			buf->align(15);
			buf->write(data->baseMat, data->numBones + data->numClientBones);
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

		buf->inc_stream(XFILE_BLOCK_CALLBACK, 4 * data->numsurfs);
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
				ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfs->name));
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
			buf->write(data->boneInfo, data->numBones + data->numClientBones);
			buf->clear_pointer(&dest->boneInfo);
		}

		if (data->invHighMipRadius)
		{
			buf->align(1);
			buf->write(data->invHighMipRadius, data->numsurfs);
			buf->clear_pointer(&dest->invHighMipRadius);
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

		if (data->unknown02)
		{
			buf->align(15);
			buf->write(data->unknown02, data->unknown02Count);
			buf->clear_pointer(&dest->unknown02);
		}

		if (data->unknownNames)
		{
			buf->align(3);
			buf->write(data->unknownNames, data->unknownNamesCount);
			buf->clear_pointer(&dest->unknownNames);
		}

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

		if (data->aimAssistBones)
		{
			buf->align(3);
			buf->write(data->aimAssistBones, data->numAimAssistBones);
			buf->clear_pointer(&dest->aimAssistBones);
		}

		if (data->unknown03)
		{
			buf->align(3);
			buf->write(data->unknown03, data->unknown03Count);
			buf->clear_pointer(&dest->unknown03);
		}

		if (data->unknownVec3)
		{
			buf->align(3);
			buf->write(data->unknownVec3, data->unknownVec3Count);
			buf->clear_pointer(&dest->unknownVec3);
		}

		if (data->unknown04)
		{
			buf->align(3);
			buf->write(data->unknown04, data->unknown04Count);
			buf->clear_pointer(&dest->unknown04);
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

		// tags
		for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->boneNames[i]));
		}

		// basic info
		dump.dump_array(asset->parentList, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->tagAngles, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->tagPositions, asset->numBones + asset->numClientBones - asset->numRootBones);
		dump.dump_array(asset->partClassification, asset->numBones);
		dump.dump_array(asset->baseMat, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->reactiveMotionParts, asset->numReactiveMotionParts);
		dump.dump_single(asset->reactiveMotionTweaks);
		dump.dump_array(asset->collSurfs, asset->numCollSurfs);
		dump.dump_array(asset->boneInfo, asset->numBones + asset->numClientBones);
		dump.dump_array(asset->invHighMipRadius, asset->numsurfs);

		// surfaces
		dump.dump_array(asset->materialHandles, asset->numsurfs);
		for (unsigned int i = 0; i < asset->numsurfs; i++)
		{
			dump.dump_asset(asset->materialHandles[i]);
		}

		// lods
		for (auto i = 0; i < 6; i++)
		{
			dump.dump_asset(asset->lodInfo[i].modelSurfs);
		}

		// physics
		dump.dump_asset(asset->physicsAsset);
		dump.dump_asset(asset->physicsFXShape);

		// unknown
		dump.dump_array(asset->unknown02, asset->unknown02Count);

		for (unsigned int i = 0; i < asset->unknownNamesCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->unknownNames[i]));
		}

		dump.dump_asset(asset->scriptableMoverDef);
		dump.dump_asset(asset->proceduralBones);

		for (unsigned int i = 0; i < asset->numAimAssistBones; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->aimAssistBones[i]));
		}

		dump.dump_array(asset->unknown03, asset->unknown03Count);
		dump.dump_array(asset->unknownVec3, asset->unknownVec3Count);
		dump.dump_array(asset->unknown04, asset->unknown04Count);

		dump.close();
	}
}