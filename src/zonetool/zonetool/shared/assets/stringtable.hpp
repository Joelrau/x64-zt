#pragma once

namespace zonetool
{
	template <ASSET_TEMPLATE>
	class string_table : public asset_interface
	{
		typedef S StringTable;
		typedef TYPEOF_MEMBER(S, values) StringTableCell;

	private:
		std::string name_;
		S* asset_ = nullptr;

	public:
		S* parse(const std::string& name, zone_memory* mem)
		{
			auto table = csv::parser(filesystem::get_file_path(name) + name);
			auto stringtable = mem->allocate<S>();

			stringtable->name = mem->duplicate_string(name);
			stringtable->rowCount = static_cast<int>(table.get_num_rows());
			stringtable->columnCount = static_cast<int>(table.get_max_columns());
			stringtable->values = mem->allocate<StringTableCell>(stringtable->rowCount * stringtable->columnCount);

			auto rows = table.get_rows();
			for (int row = 0; row < table.get_num_rows(); row++)
			{
				for (int col = 0; col < stringtable->columnCount; col++)
				{
					int entry = (row * stringtable->columnCount) + col;
					stringtable->values[entry].string = mem->duplicate_string(col >= rows[row]->num_fields ? "" : rows[row]->fields[col]);
					stringtable->values[entry].hash = string_table_hash(stringtable->values[entry].string);
				}
			}

			return stringtable;
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
				this->asset_ = db_find_x_asset_header_safe<H, E>(this->type(), this->name().data()).stringTable;
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

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto data = this->asset_;
			auto dest = buf->write<S>(data);

			buf->push_stream(Streams::XFILE_BLOCK_VIRTUAL);

			dest->name = buf->write_str(this->name());

			if (data->values)
			{
				buf->align(game::get_mode() == game::game_mode::iw7 ? 7 : 3);
				const auto destStrings = buf->write(data->values, data->columnCount * data->rowCount);

				if (data->columnCount * data->rowCount > 0)
				{
					for (int i = 0; i < data->columnCount * data->rowCount; i++)
					{
						if (data->values[i].string)
						{
							destStrings[i].string = buf->write_str(data->values[i].string);
						}
					}
				}

				buf->clear_pointer(&dest->values);
			}

			buf->pop_stream();
		}

		template <typename S>
		static void dump(S* asset)
		{
			auto file = filesystem::file(asset->name);
			file.open("wb");

			if (!file.get_fp())
			{
				file.close();
				return;
			}

			for (auto row = 0; row < asset->rowCount; row++)
			{
				for (auto column = 0; column < asset->columnCount; column++)
				{
					const auto index = (row * asset->columnCount) + column;
					const auto string_value = asset->values[index].string;
					const auto last_char = (column == asset->columnCount - 1) ? "\n" : ",";

					if (string_value == nullptr)
					{
						std::fprintf(file.get_fp(), last_char);
					}
					else
					{
						std::string str = string_value;
						auto added_quotes = false;
						if (str.contains(','))
						{
							added_quotes = true;
							str.insert(str.begin(), '"');
							str.insert(str.end(), '"');
						}

						if (str.contains('\"') && !added_quotes)
						{
							str = std::regex_replace(str, std::regex("\""), "\\\"");

							str.insert(str.begin(), '"');
							str.insert(str.end(), '"');
						}

						str = std::regex_replace(str, std::regex("\n"), "\\n");
						std::fprintf(file.get_fp(), "%s%s", str.data(), last_char);
					}
				}
			}

			file.close();
		}
	};
}