#include <std_include.hpp>
#include "stringtable.hpp"

namespace zonetool::s4
{
	namespace
	{
		std::string csv_cell(std::string value)
		{
			const auto has_comma = value.contains(',');
			if (value.contains('"') && !has_comma)
			{
				value = std::regex_replace(value, std::regex("\""), "\\\"");
			}

			if (has_comma || value.contains('"'))
			{
				value = '"' + value + '"';
			}

			return std::regex_replace(value, std::regex("\n"), "\n");
		}
	}

	StringTable* string_table::parse(const std::string& name, zone_memory* mem)
	{
		const auto path = filesystem::get_file_path(name) + name;
		if (!filesystem::file(name).exists())
		{
			return nullptr;
		}

		csv::parser table(path);
		auto* asset = mem->allocate<StringTable>();
		asset->name = mem->duplicate_string(name);
		asset->rowCount = table.get_num_rows();
		asset->columnCount = table.get_max_columns();

		const auto cell_count = asset->rowCount * asset->columnCount;
		asset->cellIndices = mem->allocate<unsigned short>(cell_count);

		std::vector<std::string> unique_strings;
		std::unordered_map<std::string, unsigned short> indices;

		const auto rows = table.get_rows();
		for (auto row = 0; row < asset->rowCount; row++)
		{
			for (auto column = 0; column < asset->columnCount; column++)
			{
				const std::string value = column < rows[row]->num_fields ? rows[row]->fields[column] : "";
				auto [entry, added] = indices.try_emplace(value, static_cast<unsigned short>(unique_strings.size()));
				if (added)
				{
					unique_strings.emplace_back(value);
				}

				asset->cellIndices[row * asset->columnCount + column] = entry->second;
			}
		}

		asset->uniqueCellCount = static_cast<int>(unique_strings.size());
		asset->hashes = mem->allocate<int>(asset->uniqueCellCount);
		asset->strings = mem->allocate<const char*>(asset->uniqueCellCount);
		for (auto i = 0; i < asset->uniqueCellCount; i++)
		{
			asset->hashes[i] = static_cast<int>(string_table_hash(unique_strings[i]));
			asset->strings[i] = mem->duplicate_string(unique_strings[i]);
		}

		return asset;
	}

	void string_table::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<StringTable>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
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

	void string_table::read(zone_reader& reader, StringTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->cellIndices, 1, asset->columnCount * asset->rowCount);
		reader.read_array(asset->hashes, 3, asset->uniqueCellCount);

		if (const auto strings = reader.read_array(asset->strings, 7, asset->uniqueCellCount))
		{
			for (auto i = 0; i < asset->uniqueCellCount; i++)
			{
				reader.read_string(strings[i]);
			}
		}
		reader.pop_stream();
	}

	void string_table::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->cellIndices)
		{
			buf->align(1);
			buf->write(data->cellIndices, data->columnCount * data->rowCount);
			buf->clear_pointer(&dest->cellIndices);
		}

		if (data->hashes)
		{
			buf->align(3);
			buf->write(data->hashes, data->uniqueCellCount);
			buf->clear_pointer(&dest->hashes);
		}

		if (data->strings)
		{
			buf->align(7);
			const auto strings = buf->write(data->strings, data->uniqueCellCount);
			for (auto i = 0; i < data->uniqueCellCount; i++)
			{
				if (data->strings[i])
				{
					strings[i] = buf->write_str(data->strings[i]);
				}
			}
			buf->clear_pointer(&dest->strings);
		}

		buf->pop_stream();
	}

	void string_table::dump(StringTable* asset)
	{
		filesystem::file file(asset->name);
		file.open("wb");
		if (!file.get_fp())
		{
			return;
		}

		std::string text;
		for (auto row = 0; row < asset->rowCount; row++)
		{
			for (auto column = 0; column < asset->columnCount; column++)
			{
				const auto index = asset->cellIndices[row * asset->columnCount + column];
				const auto value = asset->strings[index];
				text += value ? csv_cell(value) : "";
				text += column == asset->columnCount - 1 ? "\n" : ",";
			}
		}

		file.write(text);
		file.close();
	}
}
