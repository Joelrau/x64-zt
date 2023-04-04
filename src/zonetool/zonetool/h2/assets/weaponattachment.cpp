#include <std_include.hpp>
#include "weaponattachment.hpp"

namespace zonetool::h2
{
	void weapon_attachment::add_script_string(scr_string_t* ptr, const char* str)
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

	const char* weapon_attachment::get_script_string(scr_string_t* ptr)
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

#define ATTACHMENT_READ_ASSET_ARR(__type__, __datafield__, __field__, __struct__, __size__) \
	if (!data[#__field__].is_null()) \
	{ \
		attachment->__field__ = mem->allocate<__struct__*>(__size__); \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			auto asset##__field__ = data[#__field__][idx##__field__].get<std::string>(); \
			if (asset##__field__.empty()) \
			{ \
				attachment->__field__[idx##__field__] = nullptr; \
			} \
			else \
			{ \
				attachment->__field__[idx##__field__] = DB_FindXAssetHeader(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
			} \
		} \
	} \
	else \
	{ \
		attachment->__field__ = nullptr; \
	}

	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = "attachments\\"s + name + ".json"s;

		auto file = filesystem::file(path);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing attachment \"%s\"...", name.data());

		// parse json file
		file.open("rb");
		auto size = file.size();
		auto bytes = file.read_bytes(size);
		file.close();
		json data = json::parse(bytes);

		auto* attachment = mem->allocate<WeaponAttachment>();

		// base asset
		auto base = data["baseAsset"].get<std::string>();
		WeaponAttachment* baseAsset = nullptr;
		if (!base.empty())
		{
			baseAsset = DB_FindXAssetHeader(ASSET_TYPE_ATTACHMENT, base.data(), 1).attachment;
			if (baseAsset == nullptr)
			{
				ZONETOOL_FATAL("Could not load base asset \"%s\" into memory...", base.data());
			}
			memcpy(attachment, baseAsset, sizeof(WeaponAttachment));
		}
		else
		{
			ZONETOOL_WARNING("No base asset is defined for attachment \"%s\", stuff might go wrong!", name.data());
		}

		if (!data["internalName"].is_null())
		{
			attachment->szInternalName = mem->duplicate_string(data["internalName"].get<std::string>());
		}

		if (!data["displayName"].is_null())
		{
			attachment->szDisplayName = mem->duplicate_string(data["displayName"].get<std::string>());
		}

		if (!data["type"].is_null())
		{
			attachment->type = data["type"].get<AttachmentType>();
		}

		if (!data["weaponType"].is_null())
		{
			attachment->weaponType = data["weaponType"].get<weapType_t>();
		}

		if (!data["weaponClass"].is_null())
		{
			attachment->weapClass = data["weaponClass"].get<weapClass_t>();
		}

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, worldModels, XModel, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, viewModels, XModel, 2);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, reticleViewModels, XModel, 64);

		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, bounceSounds, snd_alias_list_t, 53);
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_SOUND, sound, rollingSounds, snd_alias_list_t, 53);

		attachment->stringArray1 = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->stringArray1[i], mem->duplicate_string(data["stringArray1"][i].get<std::string>()));
		}

		attachment->stringArray2 = mem->allocate<scr_string_t>(4);
		for (auto i = 0; i < 4; i++)
		{
			this->add_script_string(&attachment->stringArray2[i], mem->duplicate_string(data["stringArray2"][i].get<std::string>()));
		}

		if (!data["waFields"].is_null())
		{
			attachment->waFieldsCount = static_cast<unsigned int>(data["waFields"].size());
			attachment->waFieldOffsets = mem->allocate<unsigned short>(attachment->waFieldsCount);
			attachment->waFields = mem->allocate<WAField>(attachment->waFieldsCount);
			for (unsigned int i = 0; i < attachment->waFieldsCount; i++)
			{
				attachment->waFieldOffsets[i] = data["waFields"][i]["offset"].get<unsigned short>();

				attachment->waFields[i].fieldType = data["waFields"][i]["type"].get<unsigned char>();
				attachment->waFields[i].index = data["waFields"][i]["index"].get<unsigned char>();
				attachment->waFields[i].code = data["waFields"][i]["code"].get<unsigned char>();

				auto type = attachment->waFields[i].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					attachment->waFields[i].parm.string = mem->duplicate_string(data["waFields"][i]["value"].get<std::string>());
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					attachment->waFields[i].parm.p_int = data["waFields"][i]["value"].get<int>();
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					attachment->waFields[i].parm.p_bool = data["waFields"][i]["value"].get<bool>();
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					attachment->waFields[i].parm.p_float = data["waFields"][i]["value"].get<float>();
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					attachment->waFields[i].parm.p_int = data["waFields"][i]["value"].get<int>();
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, name.data());
				}
			}
		}
		else
		{
			attachment->waFieldOffsets = nullptr;
			attachment->waFields = nullptr;
			attachment->waFieldsCount = 0;
		}

		return attachment;
	}

	void weapon_attachment::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
		}

		this->asset_ = this->parse(name, mem);
		if (!this->asset_)
		{
			this->asset_ = db_find_x_asset_header_copy<WeaponAttachment>(XAssetType(this->type()), this->name().data(), mem).attachment;

			auto* attachment = this->asset_;
			if (attachment->stringArray1)
			{
				auto* original_strings1 = attachment->stringArray1;
				attachment->stringArray1 = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->stringArray1[i], SL_ConvertToString(original_strings1[i]));
				}
			}
			if (attachment->stringArray2)
			{
				auto* original_strings1 = attachment->stringArray2;
				attachment->stringArray2 = mem->allocate<scr_string_t>(4);
				for (auto i = 0; i < 4; i++)
				{
					this->add_script_string(&attachment->stringArray2[i], SL_ConvertToString(original_strings1[i]));
				}
			}
		}
	}

#define ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(__field__,__count__) \
		if (data->__field__) \
		{ \
			for (int i = 0; i < __count__; i++) \
			{ \
				data->__field__[i] = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&data->__field__[i]))); \
			} \
		}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(stringArray1, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY_PREPARE(stringArray2, 4);
	}

#define ATTACHMENT_SUBASSET_DEPENDING(__field__,__type__/*,__struct__*/) \
		if (data->__field__) \
		{ \
			zone->add_asset_of_type(__type__, data->__field__->name); \
		}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;

		if (data->worldModels)
		{
			for (auto i = 0; i < 2; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(worldModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->viewModels)
		{
			for (auto i = 0; i < 2; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(viewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->reticleViewModels)
		{
			for (auto i = 0; i < 64; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(reticleViewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->bounceSounds)
		{
			for (auto i = 0; i < 53; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(bounceSounds[i], ASSET_TYPE_SOUND);
			}
		}

		if (data->rollingSounds)
		{
			for (auto i = 0; i < 53; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(rollingSounds[i], ASSET_TYPE_SOUND);
			}
		}
	}

	std::string weapon_attachment::name()
	{
		return this->name_;
	}

	std::int32_t weapon_attachment::type()
	{
		return ASSET_TYPE_ATTACHMENT;
	}

#define ATTACHMENT_SOUND_CUSTOM_ARRAY(__field__, __count__) \
		if (data->__field__) \
		{ \
			buf->align(7); \
			auto* dest_sounds = buf->write(data->__field__, __count__); \
			auto ptr = 0xFDFDFDFFFFFFFFFF; \
			for (auto i = 0; i < __count__; i++) \
			{ \
				buf->align(7); \
				buf->write(&ptr); \
				buf->write_str(data->__field__[i]->name); \
				zone_buffer::clear_pointer(&dest_sounds[i]); \
			} \
			zone_buffer::clear_pointer(&dest->__field__); \
		}

#define ATTACHMENT_SCRIPTSTRING_ARRAY(__field__,__count__) \
		if (data->__field__) \
		{ \
			buf->align(3); \
			buf->write(data->__field__,__count__); \
			zone_buffer::clear_pointer(&dest->__field__); \
		}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->szDisplayName)
		{
			dest->szDisplayName = buf->write_str(data->szDisplayName);
		}

		if (data->worldModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->worldModels, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->worldModels[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->worldModels);
		}

		if (data->viewModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->viewModels, 2);

			for (auto i = 0; i < 2; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->viewModels[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->viewModels);
		}

		if (data->reticleViewModels)
		{
			buf->align(7);
			auto destModels = buf->write(data->reticleViewModels, 64);

			for (auto i = 0; i < 64; i++)
			{
				if (destModels[i])
				{
					destModels[i] = reinterpret_cast<XModel*>(
						zone->get_asset_pointer(ASSET_TYPE_XMODEL, data->reticleViewModels[i]->name)
						);
				}
			}

			zone_buffer::clear_pointer(&dest->reticleViewModels);
		}

		ATTACHMENT_SOUND_CUSTOM_ARRAY(bounceSounds, 53);
		ATTACHMENT_SOUND_CUSTOM_ARRAY(rollingSounds, 53);

		if (data->chargeInfo)
		{
			buf->align(3);
			buf->write(data->chargeInfo);
			zone_buffer::clear_pointer(&dest->chargeInfo);
		}

		if (data->hybridSettings)
		{
			buf->align(3);
			buf->write(data->hybridSettings);
			zone_buffer::clear_pointer(&dest->hybridSettings);
		}

		ATTACHMENT_SCRIPTSTRING_ARRAY(stringArray1, 4);
		ATTACHMENT_SCRIPTSTRING_ARRAY(stringArray2, 4);

		if (data->waFieldOffsets)
		{
			buf->align(1);
			buf->write(data->waFieldOffsets, data->waFieldsCount);
			zone_buffer::clear_pointer(&dest->waFieldOffsets);
		}

		if (data->waFields)
		{
			buf->align(3);
			auto* dest_waFields = buf->write(data->waFields, data->waFieldsCount);
			for (unsigned int i = 0; i < data->waFieldsCount; i++)
			{
				auto type = data->waFields[i].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					if (data->waFields[i].parm.string)
					{
						dest_waFields[i].parm.string = buf->write_str(data->waFields[i].parm.string);
					}
				}
			}
			zone_buffer::clear_pointer(&dest->waFields);
		}

		buf->pop_stream();
	}

#define ATTACHMENT_DUMP_ASSET_ARR(__field__, __size__) \
	if (asset->__field__) \
	{ \
		for (auto idx##__field__ = 0; idx##__field__ < __size__; idx##__field__++) \
		{ \
			if (asset->__field__[idx##__field__]) \
			{ \
				data[#__field__][idx##__field__] = asset->__field__[idx##__field__]->name; \
			} \
			else \
			{ \
				data[#__field__][idx##__field__] = ""; \
			} \
		} \
	} \
	else \
	{ \
		data[#__field__] = nullptr; \
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		const auto path = "attachments\\"s + asset->name + ".json"s;

		ordered_json data;

		data["baseAsset"] = asset->szInternalName;

		data["internalName"] = asset->szInternalName;
		data["displayName"] = asset->szDisplayName ? asset->szDisplayName : "";

		data["type"] = asset->type;
		data["weaponType"] = asset->weaponType;
		data["weaponClass"] = asset->weapClass;

		ATTACHMENT_DUMP_ASSET_ARR(worldModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(viewModels, 2);
		ATTACHMENT_DUMP_ASSET_ARR(reticleViewModels, 64);

		ATTACHMENT_DUMP_ASSET_ARR(bounceSounds, 53);
		ATTACHMENT_DUMP_ASSET_ARR(rollingSounds, 53);

		for (auto i = 0; i < 4; i++)
		{
			if (asset->stringArray1 && asset->stringArray1[i])
			{
				data["stringArray1"][i] = SL_ConvertToString(asset->stringArray1[i]);
			}
			else
			{
				data["stringArray1"][i] = "";
			}
		}

		for (auto i = 0; i < 4; i++)
		{
			if (asset->stringArray2 && asset->stringArray2[i])
			{
				data["stringArray2"][i] = SL_ConvertToString(asset->stringArray2[i]);
			}
			else
			{
				data["stringArray2"][i] = "";
			}
		}

		if (asset->waFieldOffsets && asset->waFields)
		{
			data["waFields"] = {};
			for (unsigned int i = 0; i < asset->waFieldsCount; i++)
			{
				data["waFields"][i]["offset"] = asset->waFieldOffsets[i];

				data["waFields"][i]["type"] = asset->waFields[i].fieldType;
				data["waFields"][i]["index"] = asset->waFields[i].index;
				data["waFields"][i]["code"] = asset->waFields[i].code;

				auto type = asset->waFields[i].fieldType;
				if (type == WAFIELD_TYPE_STRING ||
					type == WAFIELD_TYPE_FX ||
					type == WAFIELD_TYPE_MODEL ||
					type == WAFIELD_TYPE_ANIM ||
					type == WAFIELD_TYPE_MATERIAL ||
					//type == 14 ||
					type == WAFIELD_TYPE_SOUND ||
					type == WAFIELD_TYPE_TRACER)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.string;
				}
				else if (type == WAFIELD_TYPE_INT)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_int;
				}
				else if (type == WAFIELD_TYPE_BOOL)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_bool;
				}
				else if (type == WAFIELD_TYPE_FLOAT)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_float;
				}
				else if (type == WAFIELD_TYPE_FLOAT32)
				{
					data["waFields"][i]["value"] = asset->waFields[i].parm.p_int;
				}
				else
				{
					ZONETOOL_FATAL("Unknown WAField type: %d, for attachment \"%s\"", type, asset->name);
				}
			}
		}
		else
		{
			data["waFields"] = nullptr;
		}

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}