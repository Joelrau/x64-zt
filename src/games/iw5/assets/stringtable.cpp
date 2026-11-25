#include <std_include.hpp>
#include "stringtable.hpp"

namespace zonetool::iw5
{
	namespace
	{
		std::string format_cell(const std::string_view value)
		{
			const auto has_comma = value.contains(',');
			const auto has_quote = value.contains('"');
			std::string text;
			for (const auto c : value)
			{
				if (c == '"' && !has_comma)
				{
					text += "\\\"";
				}
				else if (c == '\n')
				{
					text += "\\n";
				}
				else
				{
					text += c;
				}
			}

			return has_comma || has_quote ? "\"" + text + "\"" : text;
		}
	}

	void string_table::read(zone_reader32& reader, StringTable* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		const auto count = asset->columnCount * asset->rowCount;
		if (const auto cells = reader.read_array(asset->values, 3, count))
		{
			for (auto i = 0; i < count; i++)
			{
				reader.read_string(cells[i].string);
			}
		}
		reader.pop_stream();
	}

	void string_table::dump(StringTable* asset)
	{
		filesystem::file file(asset->name.get());
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		std::string text;
		const auto values = asset->values.get();
		for (auto row = 0; row < asset->rowCount; row++)
		{
			for (auto column = 0; column < asset->columnCount; column++)
			{
				if (const auto value = values[row * asset->columnCount + column].string.get())
				{
					text += format_cell(value);
				}

				text += column == asset->columnCount - 1 ? '\n' : ',';
			}
		}

		file.write(text);
		file.close();
	}
}
