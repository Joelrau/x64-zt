#include <std_include.hpp>
#include "s2.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t world_model_count = 2;
		constexpr std::size_t view_model_count = 12;
		constexpr std::size_t reticle_view_model_count = 255;

		constexpr auto attachment_fields = glz::tuple
		{
			&WeaponAttachment::szInternalName,
			&WeaponAttachment::szDisplayName,
			&WeaponAttachment::type,
			&WeaponAttachment::weaponType,
			&WeaponAttachment::weapClass,
			&WeaponAttachment::greebleType,
			&WeaponAttachment::unknown32,
			&WeaponAttachment::unknown36,
			json_reflect::counted(&WeaponAttachment::worldModels, world_model_count),
			json_reflect::counted(&WeaponAttachment::viewModels, view_model_count),
			json_reflect::counted(&WeaponAttachment::worldModelsAdditional, world_model_count),
			json_reflect::counted(&WeaponAttachment::viewModelsAdditional, view_model_count),
			json_reflect::counted(&WeaponAttachment::reticleViewModels, reticle_view_model_count),
			json_reflect::manual("unkStruct80"),
			json_reflect::manual("fields"),
			&WeaponAttachment::loadIndex,
			&WeaponAttachment::adsSettingsMode,
			&WeaponAttachment::adsSceneBlurStrength,
			json_reflect::manual("knifeAttachTagOverride"),
			&WeaponAttachment::unknown124,
			&WeaponAttachment::extendedMag,
			&WeaponAttachment::unknown134,
			&WeaponAttachment::treatAsAltWeapon,
			&WeaponAttachment::tacticalKnife,
			&WeaponAttachment::unknown138,
		};

		constexpr json_reflect::options json_options{.strings = json_reflect::null_string::null};

		std::string json_path(const std::string& name)
		{
			return "attachments\\" + name + ".json";
		}

		bool is_string_field(const AttachmentField& field)
		{
			return field.type < WAFIELD_TYPE_INT || field.type > WAFIELD_TYPE_MILLISECONDS;
		}

		bool is_float_value(const AttachmentField& field)
		{
			return field.type == WAFIELD_TYPE_FLOAT || (field.type != WAFIELD_TYPE_BOOL && field.code != WAFIELD_CODE_SET);
		}

		void read_field_value(const json& data, AttachmentField& field, zone_memory* mem)
		{
			if (is_string_field(field))
			{
				field.value.string = data.is_string() ? mem->duplicate_string(data.get<std::string>()) : nullptr;
			}
			else if (is_float_value(field))
			{
				field.value.floatValue = data.get<float>();
			}
			else if (field.type == WAFIELD_TYPE_BOOL)
			{
				field.value.intValue = data.get<bool>();
			}
			else
			{
				field.value.intValue = data.get<int>();
			}
		}

		void write_field_value(ordered_json& data, const AttachmentField& field)
		{
			if (is_string_field(field))
			{
				json_reflect::write(data, field.value.string, json_options);
			}
			else if (is_float_value(field))
			{
				data = field.value.floatValue;
			}
			else if (field.type == WAFIELD_TYPE_BOOL)
			{
				data = field.value.intValue != 0;
			}
			else
			{
				data = field.value.intValue;
			}
		}

		std::uint16_t read_field_offset(const json& data, const WeaponAttachment& attachment)
		{
			if (data.contains("offset"))
			{
				return data["offset"].get<std::uint16_t>();
			}

			const auto& name = data["name"].get_ref<const std::string&>();
			const auto offset = weapon_field_offset(name);
			if (!offset)
			{
				ZONETOOL_FATAL("Unknown weapon field \"%s\" in attachment \"%s\"", name.data(), attachment.szInternalName);
			}

			return *offset;
		}

		void read_fields(const json& data, WeaponAttachment& attachment, zone_memory* mem)
		{
			if (!data.is_array())
			{
				return;
			}

			std::vector<std::pair<std::uint16_t, AttachmentField>> fields;
			for (const auto& entry : data)
			{
				auto& [offset, field] = fields.emplace_back();
				offset = read_field_offset(entry, attachment);
				field.index = entry.value("index", std::uint8_t(0));
				field.type = entry["type"].get<std::uint8_t>();
				field.code = entry["code"].get<std::uint8_t>();
				read_field_value(entry["value"], field, mem);
			}

			// the game expects the fields sorted by offset and index, the same as stock zones
			std::ranges::stable_sort(fields, [](const auto& left, const auto& right)
			{
				return std::tie(left.first, left.second.index) < std::tie(right.first, right.second.index);
			});

			attachment.fieldCount = static_cast<int>(fields.size());
			attachment.fieldOffsets = mem->allocate<unsigned short>(std::max<std::size_t>(fields.size(), 1));
			attachment.fields = mem->allocate<AttachmentField>(std::max<std::size_t>(fields.size(), 1));
			for (auto i = 0u; i < fields.size(); i++)
			{
				attachment.fieldOffsets[i] = fields[i].first;
				attachment.fields[i] = fields[i].second;
			}
		}

		void write_fields(ordered_json& data, const WeaponAttachment& attachment)
		{
			if (!attachment.fields)
			{
				data = nullptr;
				return;
			}

			data = ordered_json::array();
			for (auto i = 0; i < attachment.fieldCount; i++)
			{
				const auto& field = attachment.fields[i];
				auto& entry = data.emplace_back();
				if (const auto name = weapon_field_name(attachment.fieldOffsets[i]))
				{
					entry["name"] = *name;
				}
				else
				{
					entry["offset"] = attachment.fieldOffsets[i];
				}

				entry["index"] = field.index;
				entry["type"] = field.type;
				entry["code"] = field.code;
				write_field_value(entry["value"], field);
			}
		}

		void read_unk_struct80(const json& data, WeaponAttachment& attachment, zone_memory* mem)
		{
			if (!data.is_array())
			{
				return;
			}

			attachment.unkStruct80 = mem->allocate<char[72]>();
			for (auto i = 0u; i < std::size(*attachment.unkStruct80) && i < data.size(); i++)
			{
				(*attachment.unkStruct80)[i] = static_cast<char>(data[i].get<std::uint8_t>());
			}
		}

		void write_unk_struct80(ordered_json& data, const WeaponAttachment& attachment)
		{
			if (!attachment.unkStruct80)
			{
				data = nullptr;
				return;
			}

			for (auto i = 0u; i < std::size(*attachment.unkStruct80); i++)
			{
				data[i] = static_cast<std::uint8_t>((*attachment.unkStruct80)[i]);
			}
		}
	}

	void weapon_attachment::read(zone_reader& reader, WeaponAttachment* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->szInternalName);
		reader.read_string(asset->szDisplayName);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModels, world_model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModels, view_model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->worldModelsAdditional, world_model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->viewModelsAdditional, view_model_count);
		reader.read_asset_array(ASSET_TYPE_XMODEL, asset->reticleViewModels, reticle_view_model_count);
		reader.read_script_string(asset->knifeAttachTagOverride);
		reader.read_array(asset->unkStruct80, 3);
		reader.read_array(asset->fieldOffsets, 1, asset->fieldCount);
		if (const auto fields = reader.read_array(asset->fields, 7, asset->fieldCount))
		{
			for (auto i = 0; i < asset->fieldCount; i++)
			{
				if (is_string_field(fields[i]))
				{
					reader.read_string(fields[i].value.string);
				}
			}
		}
		reader.pop_stream();
	}

	void weapon_attachment::prepare(zone_buffer* buf, zone_memory* mem)
	{
		script_string_preparer prepare(buf);
		prepare(this->asset_->knifeAttachTagOverride);
	}

	void weapon_attachment::load_depending(zone_base* zone)
	{
		auto* data = this->asset_;
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->worldModels, world_model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->viewModels, view_model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->worldModelsAdditional, world_model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->viewModelsAdditional, view_model_count);
		add_dependencies(zone, ASSET_TYPE_XMODEL, data->reticleViewModels, reticle_view_model_count);
	}

	void weapon_attachment::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);

		dest->szInternalName = this->write_name(buf);
		write_string(buf, data->szDisplayName, dest->szDisplayName);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->worldModels, world_model_count, dest->worldModels);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->viewModels, view_model_count, dest->viewModels);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->worldModelsAdditional, world_model_count, dest->worldModelsAdditional);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->viewModelsAdditional, view_model_count, dest->viewModelsAdditional);
		write_asset_array(zone, buf, ASSET_TYPE_XMODEL, data->reticleViewModels, reticle_view_model_count, dest->reticleViewModels);
		write_array(buf, 3, data->unkStruct80, 1, dest->unkStruct80);
		write_array(buf, 1, data->fieldOffsets, data->fieldCount, dest->fieldOffsets);
		if (auto* fields = write_array(buf, 7, data->fields, data->fieldCount, dest->fields))
		{
			for (auto i = 0; i < data->fieldCount; i++)
			{
				if (is_string_field(data->fields[i]))
				{
					write_string(buf, data->fields[i].value.string, fields[i].value.string);
				}
			}
		}

		buf->pop_stream();
	}

	WeaponAttachment* weapon_attachment::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file(json_path(name));
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing attachment \"%s\"...", name.data());

		file.open("rb");
		auto data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<WeaponAttachment>();
		json_reflect::read<attachment_fields>(data, *asset, mem, json_options);
		read_unk_struct80(data["unkStruct80"], *asset, mem);
		read_fields(data["fields"], *asset, mem);
		asset->knifeAttachTagOverride = parse_script_string(data["knifeAttachTagOverride"]);
		return asset;
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		ordered_json data;
		json_reflect::write<attachment_fields>(data, *asset, json_options);
		write_unk_struct80(data["unkStruct80"], *asset);
		write_fields(data["fields"], *asset);
		dump_script_string(data["knifeAttachTagOverride"], asset->knifeAttachTagOverride);

		const auto str = data.dump(4);
		auto file = filesystem::file(json_path(asset->szInternalName));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}
}
