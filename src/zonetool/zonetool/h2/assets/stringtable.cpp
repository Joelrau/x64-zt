#include <std_include.hpp>
#include "stringtable.hpp"

namespace zonetool::h2
{
	namespace
	{
		std::vector<std::string> csv_split(const std::string& src)
		{
			std::vector<std::string> res;
			std::string buffer;
			auto in_quote = false;

			for (auto i = 0; i < src.size(); i++)
			{
				const auto c = src[i];
				if (c == '\\' && (i < src.size() - 1 && src[i + 1] == '\"'))
				{
					buffer.append("\"");
					++i;
					continue;
				}

				if (c == '"')
				{
					in_quote = !in_quote;
					continue;
				}

				if (c != ',' || in_quote)
				{
					buffer.push_back(src[i]);
				}
				else if (c == ',' && !in_quote)
				{
					res.push_back(buffer);
					buffer.clear();
				}
			}

			if (!buffer.empty())
			{
				res.push_back(buffer);
			}

			return res;
		}
	}

	// LEGACY ZONETOOL CODE, FIX ME!
	class CSV
	{
	protected:
		std::string _name;
		std::vector<std::vector<std::string>> _data;

	public:
		CSV(std::string name, char sep = ',')
			: _name(name)
		{
			auto f = filesystem::file(name);
			f.open("rb");

			auto fp = f.get_fp();

			if (fp)
			{
				auto len = f.size();
				auto buf = std::make_unique<char[]>(len + 1);
				memset(buf.get(), 0, len + 1);
				fread(buf.get(), len, 1, fp);
				fclose(fp);

				std::vector<std::string> rows = utils::string::split(std::string(buf.get()), '\n');

				for (auto& row : rows)
				{
					// Replace literal characters
					std::size_t pos;
					while ((pos = row.find("\\n")) != std::string::npos)
					{
						row.replace(pos, 2, "\n");
					}

					while ((pos = row.find("\\t")) != std::string::npos)
					{
						row.replace(pos, 2, "\t");
					}

					if (row.size() && row[row.size() - 1] == '\r')
					{
						row.pop_back();
					}

					_data.push_back(csv_split(row));
				}
			}

			f.close();
		}

		std::string entry(std::size_t row, std::size_t column)
		{
			return _data[row][column];
		}

		std::size_t rows()
		{
			return _data.size();
		}

		std::size_t columns(std::size_t row)
		{
			return _data[row].size();
		}

		std::size_t max_columns()
		{
			std::size_t _max = 0;

			for (std::size_t row = 0; row < this->rows(); row++)
			{
				if (_max < this->columns(row))
					_max = this->columns(row);
			}

			return _max;
		}

		void clear()
		{
			for (std::size_t i = 0; i < _data.size(); i++)
			{
				for (std::size_t j = 0; j < _data[i].size(); j++)
					_data[i][j].clear();

				_data[i].clear();
			}

			_data.clear();
		}
	};

	int StringTable_Hash(const char* string)
	{
		int hash = 0;
		char* data = _strdup(string);

		while (*data != 0)
		{
			hash = tolower(*data) + (31 * hash);
			data++;
		}

		return hash;
	}

	StringTable* StringTable_Parse(std::string name, zone_memory* mem)
	{
		auto table = std::make_unique<CSV>(name);
		auto stringtable = mem->allocate<StringTable>();

		stringtable->name = mem->duplicate_string(name.c_str());
		stringtable->rowCount = static_cast<int>(table->rows());
		stringtable->columnCount = static_cast<int>(table->max_columns());
		stringtable->values = mem->allocate<StringTableCell>(stringtable->rowCount * stringtable->columnCount);

		for (int row = 0; row < table->rows(); row++)
		{
			for (int col = 0; col < table->columns(row); col++)
			{
				int entry = (row * stringtable->columnCount) + col;
				stringtable->values[entry].string = mem->duplicate_string(table->entry(row, col).c_str());
				stringtable->values[entry].hash = StringTable_Hash(stringtable->values[entry].string);
			}
		}

		return stringtable;
	}

	void string_table::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<typename std::remove_reference<decltype(*this->asset_)>::type>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = db_find_x_asset_header_safe(static_cast<XAssetType>(this->type()), this->name().data()).stringTable;
		if (filesystem::file(name).exists())
		{
			ZONETOOL_INFO("Parsing stringtable \"%s\"...", name.data());
			this->asset_ = StringTable_Parse(name, mem);
		}
	}

	void string_table::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void string_table::load_depending(zone_base* zone)
	{
	}

	std::string string_table::name()
	{
		return this->name_;
	}

	std::int32_t string_table::type()
	{
		return ASSET_TYPE_STRINGTABLE;
	}

	void string_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto data = this->asset_;
		auto dest = buf->write(data);

		buf->push_stream(3);

		dest->name = buf->write_str(this->name());

		if (data->values)
		{
			buf->align(3);
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

			zone_buffer::clear_pointer(&dest->values);
		}

		buf->pop_stream();
	}

	void string_table::dump(StringTable* asset)
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
}