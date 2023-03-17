#include "std_include.hpp"
#include "xmodel.hpp"

namespace zonetool::iw6
{
	void IXModel::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* IXModel::get_script_string(scr_string_t* ptr)
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

	XModel* IXModel::parse(std::string name, ZoneMemory* mem)
	{
		const auto path = "xmodel\\"s + name + ".xmodel_export";

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

		read.close();

		return asset;
	}

	void IXModel::init(const std::string& name, ZoneMemory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->Alloc<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->StrDup(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = DB_FindXAssetHeader_Copy<XModel>(XAssetType(this->type()), this->name_.data(), mem).model;

			auto* asset = this->asset_;

			auto* original_scriptstrings = asset->boneNames;
			asset->boneNames = mem->Alloc<scr_string_t>(asset->numBones);
			for (unsigned char i = 0; i < asset->numBones; i++)
			{
				this->add_script_string(&asset->boneNames[i], SL_ConvertToString(original_scriptstrings[i]));
			}
		}
	}

	void IXModel::prepare(ZoneBuffer* buf, ZoneMemory* mem)
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
	}

	void IXModel::load_depending(IZone* zone)
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
				zone->add_asset_of_type(ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfs->name);
			}
		}

		// PhysPreset
		if (data->physPreset)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSPRESET, data->physPreset->name);
		}

		// PhysCollmap
		if (data->physCollmap)
		{
			zone->add_asset_of_type(ASSET_TYPE_PHYSCOLLMAP, data->physCollmap->name);
		}
	}

	std::string IXModel::name()
	{
		return this->name_;
	}

	std::int32_t IXModel::type()
	{
		return ASSET_TYPE_XMODEL;
	}

	void IXModel::write(IZone* zone, ZoneBuffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->boneNames)
		{
			buf->align(3);
			buf->write(data->boneNames, data->numBones);
			ZoneBuffer::clear_pointer(&dest->boneNames);
		}

		if (data->parentList)
		{
			buf->align(0);
			buf->write(data->parentList, (data->numBones - data->numRootBones));
			ZoneBuffer::clear_pointer(&dest->parentList);
		}

		if (data->tagAngles)
		{
			buf->align(1);
			buf->write(data->tagAngles, (data->numBones - data->numRootBones));
			ZoneBuffer::clear_pointer(&dest->tagAngles);
		}

		if (data->tagPositions)
		{
			buf->align(3);
			buf->write(data->tagPositions, (data->numBones - data->numRootBones));
			ZoneBuffer::clear_pointer(&dest->tagPositions);
		}

		if (data->partClassification)
		{
			buf->align(0);
			buf->write(data->partClassification, data->numBones);
			ZoneBuffer::clear_pointer(&dest->partClassification);
		}

		if (data->baseMat)
		{
			buf->align(3);
			buf->write(data->baseMat, data->numBones);
			ZoneBuffer::clear_pointer(&dest->baseMat);
		}

		if (data->reactiveMotionParts)
		{
			buf->align(15);
			buf->write(data->reactiveMotionParts, data->numReactiveMotionParts);
			ZoneBuffer::clear_pointer(&dest->reactiveMotionParts);
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
				ASSET_TYPE_XMODEL_SURFS, data->lodInfo[i].modelSurfs->name));
		}

		if (data->collSurfs)
		{
			buf->align(3);
			buf->write(data->collSurfs, data->numCollSurfs);
			ZoneBuffer::clear_pointer(&dest->collSurfs);
		}

		if (data->boneInfo)
		{
			buf->align(3);
			buf->write(data->boneInfo, data->numBones);
			ZoneBuffer::clear_pointer(&dest->boneInfo);
		}

		if (data->invHighMipRadius)
		{
			buf->align(1);
			buf->write(data->invHighMipRadius, data->numsurfs);
			ZoneBuffer::clear_pointer(&dest->invHighMipRadius);
		}

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

		buf->pop_stream();
	}

	void IXModel::dump(XModel* asset)
	{
		const auto path = "xmodel\\"s + asset->name + ".xmodel_export";

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

		dump.close();
	}
}