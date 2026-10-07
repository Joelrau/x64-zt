#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class proto : public asset_interface
	{
	private:
		typedef TYPEOF_MEMBER(S, messages) message_t;
		typedef TYPEOF_MEMBER(message_t, fields) field_t;
		typedef TYPEOF_MEMBER(S, enums) enum_t;
		typedef TYPEOF_MEMBER(enum_t, values) enum_value_t;
		typedef TYPEOF_MEMBER(S, message_index) index_t;
		typedef TYPEOF_MEMBER(index_t, entries) index_entry_t;

		static constexpr int message_field_index_capacity = 128;
		static constexpr int root_message_index_capacity = 64;
		static constexpr int root_enum_index_capacity = 8;
		static constexpr int max_message_nesting = 32;

		struct field_type_name
		{
			int type;
			const char* name;
		};

		static constexpr field_type_name field_type_names[] =
		{
			{1, "float"},
			{5, "varint"},
			{7, "zigzag"},
			{12, "bool"},
			{13, "string"},
			{14, "bytes"},
			{15, "message"},
		};

		std::string name_;
		S* asset_ = nullptr;

		// Java String.hashCode
		static unsigned int name_hash(const char* name)
		{
			unsigned int hash = 0;
			for (; *name; name++)
			{
				hash = hash * 31u + static_cast<unsigned int>(static_cast<signed char>(*name));
			}
			return hash;
		}

		static void build_index(index_t& index, const std::vector<const char*>& names, const int capacity, zone_memory* mem)
		{
			index.count = static_cast<int>(names.size());
			index.capacity = capacity;
			index.entries = mem->allocate<index_entry_t>(names.size());

			for (auto i = 0; i < index.count; i++)
			{
				index.entries[i].name_hash = name_hash(names[i]);
				index.entries[i].index = i;
			}

			std::stable_sort(index.entries, index.entries + index.count, [](const index_entry_t& a, const index_entry_t& b)
			{
				return a.name_hash < b.name_hash;
			});
		}

		static std::string field_type_to_string(const int type)
		{
			for (const auto& entry : field_type_names)
			{
				if (entry.type == type)
				{
					return entry.name;
				}
			}

			return std::to_string(type);
		}

		static int field_type_from_json(const ordered_json& value)
		{
			if (value.is_number_integer())
			{
				return value.get<int>();
			}

			const auto name = value.get<std::string>();
			for (const auto& entry : field_type_names)
			{
				if (name == entry.name)
				{
					return entry.type;
				}
			}

			ZONETOOL_FATAL("Unknown proto field type \"%s\"", name.data());
		}

		static const ordered_json& find_named(const ordered_json& list, const std::string& name, const char* kind)
		{
			for (const auto& item : list)
			{
				if (item["name"].get<std::string>() == name)
				{
					return item;
				}
			}

			ZONETOOL_FATAL("Proto %s \"%s\" is not defined", kind, name.data());
		}

		static void parse_enum(const ordered_json& source, enum_t* dest, zone_memory* mem)
		{
			dest->name = mem->duplicate_string(source["name"].get<std::string>());
			dest->value_count = static_cast<int>(source["values"].size());
			dest->values = mem->allocate<enum_value_t>(dest->value_count);

			for (auto i = 0; i < dest->value_count; i++)
			{
				const auto& value = source["values"][i];
				dest->values[i].name = mem->duplicate_string(value["name"].get<std::string>());
				dest->values[i].__pad0[0] = value["number"].get<int>();
				dest->values[i].__pad0[1] = value.value("unk", 0);
			}
		}

		static void parse_message(const ordered_json& source, const ordered_json& root, message_t* dest,
			zone_memory* mem, const int depth)
		{
			if (depth > max_message_nesting)
			{
				ZONETOOL_FATAL("Proto message \"%s\" nests too deep", source["name"].get<std::string>().data());
			}

			dest->name = mem->duplicate_string(source["name"].get<std::string>());
			dest->field_count = static_cast<int>(source["fields"].size());
			dest->fields = mem->allocate<field_t>(dest->field_count);

			std::vector<const char*> field_names;

			for (auto i = 0; i < dest->field_count; i++)
			{
				const auto& json_field = source["fields"][i];
				auto* field = &dest->fields[i];

				field->name = mem->duplicate_string(json_field["name"].get<std::string>());
				field->type = field_type_from_json(json_field["type"]);
				field->number = json_field["number"].get<int>();
				field->repeated = json_field.value("repeated", false) ? 1 : 0;
				field_names.push_back(field->name);

				if (json_field.contains("message"))
				{
					const auto& message = find_named(root["messages"], json_field["message"].get<std::string>(), "message");
					field->message = mem->allocate<message_t>();
					parse_message(message, root, field->message, mem, depth + 1);
				}

				if (json_field.contains("enum"))
				{
					const auto& enum_type = find_named(root["enums"], json_field["enum"].get<std::string>(), "enum");
					field->enum_type = mem->allocate<enum_t>();
					parse_enum(enum_type, field->enum_type, mem);
				}
			}

			build_index(dest->field_index, field_names, message_field_index_capacity, mem);
		}

		static ordered_json dump_enum(const enum_t* source)
		{
			ordered_json json_enum;
			json_enum["name"] = source->name;
			json_enum["values"] = ordered_json::array();

			for (auto i = 0; i < source->value_count; i++)
			{
				ordered_json value;
				value["name"] = source->values[i].name;
				value["number"] = source->values[i].__pad0[0];
				if (source->values[i].__pad0[1])
				{
					value["unk"] = source->values[i].__pad0[1];
				}
				json_enum["values"].push_back(value);
			}

			return json_enum;
		}

		static ordered_json dump_message(const message_t* source)
		{
			ordered_json json_message;
			json_message["name"] = source->name;
			json_message["fields"] = ordered_json::array();

			for (auto i = 0; i < source->field_count; i++)
			{
				const auto* field = &source->fields[i];

				ordered_json json_field;
				json_field["name"] = field->name;
				json_field["type"] = field_type_to_string(field->type);
				json_field["number"] = field->number;
				json_field["repeated"] = field->repeated != 0;

				if (field->message)
				{
					json_field["message"] = field->message->name;
				}

				if (field->enum_type)
				{
					json_field["enum"] = field->enum_type->name;
				}

				json_message["fields"].push_back(json_field);
			}

			return json_message;
		}

		static void read_index(zone_reader& reader, index_t& index)
		{
			reader.read_array(index.entries, 3, index.count);
		}

		static void read_enum(zone_reader& reader, enum_t* value)
		{
			reader.read_string(value->name);

			if (const auto values = reader.read_array(value->values, 3, value->value_count))
			{
				for (auto i = 0; i < value->value_count; i++)
				{
					reader.read_string(values[i].name);
				}
			}
		}

		static void read_message(zone_reader& reader, message_t* message)
		{
			reader.read_string(message->name);

			if (const auto fields = reader.read_array(message->fields, 3, message->field_count))
			{
				for (auto i = 0; i < message->field_count; i++)
				{
					reader.read_string(fields[i].name);

					if (const auto nested = reader.read_single(fields[i].message, 3))
					{
						read_message(reader, nested);
					}

					if (const auto nested = reader.read_single(fields[i].enum_type, 3))
					{
						read_enum(reader, nested);
					}
				}
			}

			read_index(reader, message->field_index);
		}

		static void write_index(zone_buffer* buf, const index_t* source, index_t* dest)
		{
			if (source->entries)
			{
				buf->align(3);
				buf->write(source->entries, source->count);
				buf->clear_pointer(&dest->entries);
			}
		}

		static void write_enum(zone_buffer* buf, const enum_t* source, enum_t* dest)
		{
			dest->name = buf->write_str(source->name);

			if (source->values)
			{
				buf->align(3);
				auto* values = buf->write(source->values, source->value_count);

				for (auto i = 0; i < source->value_count; i++)
				{
					values[i].name = buf->write_str(source->values[i].name);
				}

				buf->clear_pointer(&dest->values);
			}
		}

		static void write_message(zone_buffer* buf, const message_t* source, message_t* dest)
		{
			dest->name = buf->write_str(source->name);

			if (source->fields)
			{
				buf->align(3);
				auto* fields = buf->write(source->fields, source->field_count);

				for (auto i = 0; i < source->field_count; i++)
				{
					fields[i].name = buf->write_str(source->fields[i].name);

					if (source->fields[i].message)
					{
						buf->align(3);
						auto* nested = buf->write(source->fields[i].message);
						buf->clear_pointer(&fields[i].message);
						write_message(buf, source->fields[i].message, nested);
					}

					if (source->fields[i].enum_type)
					{
						buf->align(3);
						auto* nested = buf->write(source->fields[i].enum_type);
						buf->clear_pointer(&fields[i].enum_type);
						write_enum(buf, source->fields[i].enum_type, nested);
					}
				}

				buf->clear_pointer(&dest->fields);
			}

			write_index(buf, &source->field_index, &dest->field_index);
		}

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			const auto path = "proto\\"s + name + ".json"s;
			auto file = filesystem::file(path);
			file.open("rb");

			if (!file.get_fp())
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing proto \"%s\"...", name.data());

			const auto bytes = file.read_bytes(file.size());
			file.close();

			const auto data = ordered_json::parse(bytes);

			auto* asset = mem->allocate<S>();
			asset->name = mem->duplicate_string(data["name"].get<std::string>());

			if (data.contains("checksum"))
			{
				asset->checksum = mem->duplicate_string(data["checksum"].get<std::string>());
			}

			std::vector<const char*> message_names;
			asset->message_count = static_cast<int>(data["messages"].size());
			asset->messages = mem->allocate<message_t>(asset->message_count);

			for (auto i = 0; i < asset->message_count; i++)
			{
				parse_message(data["messages"][i], data, &asset->messages[i], mem, 0);
				message_names.push_back(asset->messages[i].name);
			}

			build_index(asset->message_index, message_names, root_message_index_capacity, mem);

			std::vector<const char*> enum_names;
			asset->enum_count = static_cast<int>(data["enums"].size());
			asset->enums = mem->allocate<enum_t>(asset->enum_count);

			for (auto i = 0; i < asset->enum_count; i++)
			{
				parse_enum(data["enums"][i], &asset->enums[i], mem);
				enum_names.push_back(asset->enums[i].name);
			}

			build_index(asset->enum_index, enum_names, root_enum_index_capacity, mem);

			return asset;
		}

		void init(const std::string& name, zone_memory* mem) override
		{
			this->name_ = name;

			if (this->referenced())
			{
				this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
				this->asset_->name = mem->duplicate_string(name);
				return;
			}

			this->asset_ = parse(name, mem);
			if (!this->asset_)
			{
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).proto;
			}
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
		}

		void load_depending(zone_base* zone) override
		{
		}

		void* pointer() override { return asset_; }

		bool referenced() override { return name_.starts_with(","); }

		std::string name() override
		{
			return this->name_;
		}

		std::int32_t type() override
		{
			return Type;
		}

		static void read(zone_reader& reader, S* asset)
		{
			reader.push_stream(Streams::XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->checksum);

			if (const auto messages = reader.read_array(asset->messages, 3, asset->message_count))
			{
				for (auto i = 0; i < asset->message_count; i++)
				{
					read_message(reader, &messages[i]);
				}
			}

			read_index(reader, asset->message_index);

			if (const auto enums = reader.read_array(asset->enums, 3, asset->enum_count))
			{
				for (auto i = 0; i < asset->enum_count; i++)
				{
					read_enum(reader, &enums[i]);
				}
			}

			read_index(reader, asset->enum_index);
			reader.pop_stream();
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write<S>(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			if (data->checksum)
			{
				dest->checksum = buf->write_str(data->checksum);
			}

			if (data->messages)
			{
				buf->align(3);
				auto* messages = buf->write(data->messages, data->message_count);

				for (auto i = 0; i < data->message_count; i++)
				{
					write_message(buf, &data->messages[i], &messages[i]);
				}

				buf->clear_pointer(&dest->messages);
			}

			write_index(buf, &data->message_index, &dest->message_index);

			if (data->enums)
			{
				buf->align(3);
				auto* enums = buf->write(data->enums, data->enum_count);

				for (auto i = 0; i < data->enum_count; i++)
				{
					write_enum(buf, &data->enums[i], &enums[i]);
				}

				buf->clear_pointer(&dest->enums);
			}

			write_index(buf, &data->enum_index, &dest->enum_index);

			buf->pop_stream();
		}

		static void dump(S* asset)
		{
			ordered_json data;
			data["name"] = asset->name;
			data["checksum"] = asset->checksum ? asset->checksum : "";
			data["messages"] = ordered_json::array();
			data["enums"] = ordered_json::array();

			for (auto i = 0; i < asset->message_count; i++)
			{
				data["messages"].push_back(dump_message(&asset->messages[i]));
			}

			for (auto i = 0; i < asset->enum_count; i++)
			{
				data["enums"].push_back(dump_enum(&asset->enums[i]));
			}

			const auto path = "proto\\"s + asset->name + ".json"s;
			auto file = filesystem::file(path);
			file.open("wb");
			file.write(data.dump(4));
			file.close();
		}
	};
}
