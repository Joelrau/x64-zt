#include <std_include.hpp>
#include "s2.hpp"
#include "json_fields.hpp"

namespace zonetool::s2
{
	namespace
	{
		constexpr std::size_t world_model_count = 2;
		constexpr std::size_t view_model_count = 12;
		constexpr std::size_t reticle_view_model_count = 255;

		bool is_string_field(const AttachmentField& field)
		{
			return static_cast<std::uint8_t>(field.type - 4) > 5;
		}

		std::string json_path(const std::string& name)
		{
			return "attachments\\" + name + ".json";
		}

#define ATTACHMENT_FIELD(field) #field, a.field
		template <typename J>
		void visit_attachment_json(const J& j, WeaponAttachment& a)
		{
			j.string(ATTACHMENT_FIELD(szInternalName));
			j.string(ATTACHMENT_FIELD(szDisplayName));
			j.value(ATTACHMENT_FIELD(type));
			j.value(ATTACHMENT_FIELD(weaponType));
			j.value(ATTACHMENT_FIELD(weapClass));
			j.value(ATTACHMENT_FIELD(greebleType));
			j.value(ATTACHMENT_FIELD(unknown32));
			j.value(ATTACHMENT_FIELD(unknown36));
			j.asset_array(ATTACHMENT_FIELD(worldModels), world_model_count);
			j.asset_array(ATTACHMENT_FIELD(viewModels), view_model_count);
			j.asset_array(ATTACHMENT_FIELD(worldModelsAdditional), world_model_count);
			j.asset_array(ATTACHMENT_FIELD(viewModelsAdditional), view_model_count);
			j.asset_array(ATTACHMENT_FIELD(reticleViewModels), reticle_view_model_count);
			j.value(ATTACHMENT_FIELD(loadIndex));
			j.value(ATTACHMENT_FIELD(adsSettingsMode));
			j.value(ATTACHMENT_FIELD(adsSceneBlurStrength));
			j.script_string(ATTACHMENT_FIELD(knifeAttachTagOverride));
			j.value(ATTACHMENT_FIELD(unknown124));
		}
#undef ATTACHMENT_FIELD

		bool is_float_value(const AttachmentField& field)
		{
			return field.type == WAFIELD_TYPE_FLOAT || (field.type != WAFIELD_TYPE_BOOL && field.code != WAFIELD_CODE_SET);
		}

		ordered_json dump_field_value(const AttachmentField& field)
		{
			if (is_string_field(field))
			{
				return field.value.string ? ordered_json(field.value.string) : ordered_json(nullptr);
			}

			if (is_float_value(field))
			{
				return field.value.floatValue;
			}

			if (field.type == WAFIELD_TYPE_BOOL)
			{
				return field.value.intValue != 0;
			}

			return field.value.intValue;
		}

		void parse_field_value(const json& value, AttachmentField& field, zone_memory* mem)
		{
			if (is_string_field(field))
			{
				field.value.string = value.is_null() ? nullptr : mem->duplicate_string(value.get<std::string>());
			}
			else if (is_float_value(field))
			{
				field.value.floatValue = value.get<float>();
			}
			else if (field.type == WAFIELD_TYPE_BOOL)
			{
				field.value.intValue = value.get<bool>();
			}
			else
			{
				field.value.intValue = value.get<int>();
			}
		}

		ordered_json dump_fields(const WeaponAttachment& a)
		{
			if (!a.fields)
			{
				return nullptr;
			}

			auto out = ordered_json::array();
			for (auto i = 0; i < a.fieldCount; i++)
			{
				const auto& field = a.fields[i];
				auto entry = ordered_json::object();
				if (const auto name = weapon_field_name(a.fieldOffsets[i]))
				{
					entry["name"] = *name;
				}
				else
				{
					entry["offset"] = a.fieldOffsets[i];
				}

				entry["index"] = field.index;
				entry["type"] = field.type;
				entry["code"] = field.code;
				entry["value"] = dump_field_value(field);
				out.push_back(std::move(entry));
			}

			return out;
		}

		std::uint16_t parse_field_offset(const json& entry, const std::string& attachment)
		{
			if (entry.contains("offset"))
			{
				return entry["offset"].get<std::uint16_t>();
			}

			const auto& name = entry["name"].get_ref<const std::string&>();
			const auto offset = weapon_field_offset(name);
			if (!offset)
			{
				ZONETOOL_FATAL("Unknown weapon field \"%s\" in attachment \"%s\"", name.data(), attachment.data());
			}

			return *offset;
		}

		void parse_fields(const json& data, WeaponAttachment& a, zone_memory* mem)
		{
			const auto node = data.find("fields");
			if (node == data.end() || node->is_null())
			{
				return;
			}

			struct parsed_field
			{
				std::uint16_t offset;
				AttachmentField field;
			};

			std::vector<parsed_field> fields;
			for (const auto& entry : *node)
			{
				auto& parsed = fields.emplace_back();
				parsed.offset = parse_field_offset(entry, a.szInternalName);
				parsed.field.index = entry.value("index", std::uint8_t(0));
				parsed.field.type = entry["type"].get<std::uint8_t>();
				parsed.field.code = entry["code"].get<std::uint8_t>();
				parse_field_value(entry["value"], parsed.field, mem);
			}

			// the game expects the fields sorted by offset and index, the same as stock zones
			std::stable_sort(fields.begin(), fields.end(), [](const parsed_field& left, const parsed_field& right)
			{
				return std::tie(left.offset, left.field.index) < std::tie(right.offset, right.field.index);
			});

			a.fieldCount = static_cast<int>(fields.size());
			a.fieldOffsets = mem->allocate<unsigned short>(std::max<std::size_t>(fields.size(), 1));
			a.fields = mem->allocate<AttachmentField>(std::max<std::size_t>(fields.size(), 1));
			for (auto i = 0u; i < fields.size(); i++)
			{
				a.fieldOffsets[i] = fields[i].offset;
				a.fields[i] = fields[i].field;
			}
		}

		ordered_json dump_unk_struct80(const WeaponAttachment& a)
		{
			if (!a.unkStruct80)
			{
				return nullptr;
			}

			return std::vector<std::uint8_t>(std::begin(*a.unkStruct80), std::end(*a.unkStruct80));
		}

		void parse_unk_struct80(const json& data, WeaponAttachment& a, zone_memory* mem)
		{
			const auto node = data.find("unkStruct80");
			if (node == data.end() || node->is_null())
			{
				return;
			}

			a.unkStruct80 = mem->allocate<std::remove_pointer_t<decltype(a.unkStruct80)>>();
			for (auto i = 0u; i < std::size(*a.unkStruct80) && i < node->size(); i++)
			{
				(*a.unkStruct80)[i] = static_cast<char>((*node)[i].get<std::uint8_t>());
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
		const auto data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<WeaponAttachment>();
		visit_attachment_json(json_field_parser(data, mem), *asset);
		parse_unk_struct80(data, *asset, mem);
		parse_fields(data, *asset, mem);
		return asset;
	}

	void weapon_attachment::dump(WeaponAttachment* asset)
	{
		ordered_json data;
		visit_attachment_json(json_field_dumper(data), *asset);
		data["unkStruct80"] = dump_unk_struct80(*asset);
		data["fields"] = dump_fields(*asset);
		const auto str = data.dump(4);

		auto file = filesystem::file(json_path(asset->szInternalName));
		file.open("wb");
		file.write(str.data(), str.size(), 1);
		file.close();
	}
}
