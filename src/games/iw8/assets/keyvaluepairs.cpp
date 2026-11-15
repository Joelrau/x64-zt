#include <std_include.hpp>
#include "keyvaluepairs.hpp"

namespace zonetool::iw8
{
	void key_value_pairs::read(zone_reader& reader, KeyValuePairs* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto pairs = reader.read_array(asset->keyValuePairs, 7, asset->numVariables))
		{
			for (auto i = 0; i < asset->numVariables; i++)
			{
				reader.read_string(pairs[i].value);
			}
		}
		reader.pop_stream();
	}

	void key_value_pairs::dump(KeyValuePairs* asset)
	{
		auto file = filesystem::file("keyvaluepairs\\"s + asset->name + ".json"s);
		file.open("wb");

		ordered_json data = ordered_json::array();
		for (auto i = 0; i < asset->numVariables; i++)
		{
			const auto& pair = asset->keyValuePairs[i];
			data.push_back({{"keyHash", pair.keyHash}, {"value", pair.value ? pair.value : ""}});
		}

		file.write(data.dump(4));
		file.close();
	}
}
