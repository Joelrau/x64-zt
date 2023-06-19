#include <std_include.hpp>
#include "weaponattachment.hpp"

namespace zonetool::s1
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
				attachment->__field__[idx##__field__] = db_find_x_asset_header(XAssetType::__type__, asset##__field__.data(), 1).__datafield__; \
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
			baseAsset = db_find_x_asset_header(ASSET_TYPE_ATTACHMENT, base.data(), 0).attachment;
			if (baseAsset == nullptr || DB_IsXAssetDefault(ASSET_TYPE_ATTACHMENT, base.data()))
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
		ATTACHMENT_READ_ASSET_ARR(ASSET_TYPE_XMODEL, model, reticleViewModels, XModel, 32);

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
					attachment->waFields[i].parm.p_float = static_cast<float>(data["waFields"][i]["value"].get<int>());
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
					attachment->waFields[i].parm.p_float = static_cast<float>(data["waFields"][i]["value"].get<int>() / 1000.0f);
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

		this->add_script_string(&attachment->unkScriptString, mem->duplicate_string(data["unkScriptString"].get<std::string>()));

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
			this->add_script_string(&attachment->unkScriptString, SL_ConvertToString(attachment->unkScriptString));
		}
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		auto* data = this->asset_;
		data->unkScriptString = static_cast<scr_string_t>(buf->write_scriptstring(this->get_script_string(&data->unkScriptString)));
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
			for (auto i = 0; i < 32; i++)
			{
				ATTACHMENT_SUBASSET_DEPENDING(reticleViewModels[i], ASSET_TYPE_XMODEL);
			}
		}

		if (data->waFields)
		{
			for (auto i = 0u; i < data->waFieldsCount; i++)
			{
#define SUBASSET_CASE(__type__, __asset_type__) \
				case __type__: \
					if (field->parm.string) \
					{ \
						zone->add_asset_of_type(__asset_type__, field->parm.string); \
					} \
					break; \

				const auto field = &data->waFields[i];
				switch (field->fieldType)
				{
					SUBASSET_CASE(WAFIELD_TYPE_FX, ASSET_TYPE_FX);
					SUBASSET_CASE(WAFIELD_TYPE_MATERIAL, ASSET_TYPE_MATERIAL);
					SUBASSET_CASE(WAFIELD_TYPE_MODEL, ASSET_TYPE_XMODEL);
					SUBASSET_CASE(WAFIELD_TYPE_SOUND, ASSET_TYPE_SOUND);
					SUBASSET_CASE(WAFIELD_TYPE_TRACER, ASSET_TYPE_TRACER);
				}
#undef SUBASSET_CASE
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

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

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
			auto destModels = buf->write(data->reticleViewModels, 32);

			for (auto i = 0; i < 32; i++)
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
		ATTACHMENT_DUMP_ASSET_ARR(reticleViewModels, 32);

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
					data["waFields"][i]["value"] = static_cast<int>(asset->waFields[i].parm.p_float);
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
					data["waFields"][i]["value"] = static_cast<int>(asset->waFields[i].parm.p_float * 1000.0f);
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

		auto* str = SL_ConvertToString(asset->unkScriptString);
		data["unkScriptString"] = str ? str : "";

		std::string json = data.dump(4);

		auto file = filesystem::file(path);
		file.open("wb");
		file.write(json.data(), json.size(), 1);
		file.close();
	}
}