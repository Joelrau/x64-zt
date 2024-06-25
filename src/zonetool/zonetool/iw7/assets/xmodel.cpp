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
		for (auto i = 0; i < asset->numBones + asset->numClientBones; i++)
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
		for (unsigned int i = 0; i < asset->numsurfs; i++)
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
		asset->physicsLODData = read.read_array<char>();

		asset->physicsLODDataNames = mem->allocate<scr_string_t>(asset->physicsLODDataNameCount);
		for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
		{
			this->add_script_string(&asset->physicsLODDataNames[i], read.read_string());
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

			original_scriptstrings = asset->physicsLODDataNames;
			asset->physicsLODDataNames = mem->allocate<scr_string_t>(asset->physicsLODDataNameCount);
			for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
			{
				this->add_script_string(&asset->physicsLODDataNames[i], SL_ConvertToString(original_scriptstrings[i]));
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

			for (unsigned int i = 0; i < xmodel->physicsLODDataNameCount; i++)
			{
				xmodel->physicsLODDataNames[i] = static_cast<scr_string_t>(buf->write_scriptstring(
					this->get_script_string(&xmodel->physicsLODDataNames[i])));
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
			for (unsigned int i = 0; i < data->numsurfs; i++)
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

		if (data->physicsLODData)
		{
			buf->align(15);
			buf->write(data->physicsLODData, data->physicsLODDataSize);
			buf->clear_pointer(&dest->physicsLODData);
		}

		if (data->physicsLODDataNames)
		{
			buf->align(3);
			buf->write(data->physicsLODDataNames, data->physicsLODDataNameCount);
			buf->clear_pointer(&dest->physicsLODDataNames);
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
		dump.dump_array(asset->physicsLODData, asset->physicsLODDataSize);

		for (unsigned int i = 0; i < asset->physicsLODDataNameCount; i++)
		{
			dump.dump_string(SL_ConvertToString(asset->physicsLODDataNames[i]));
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