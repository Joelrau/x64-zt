#include <std_include.hpp>
#include "keyvaluepairs.hpp"

namespace zonetool::t9
{
	void key_value_pairs::read(auto& reader, KeyValuePairs* asset)
	{
		if (const auto pairs = reader.read_array(asset->keyValuePairs, 7, asset->numVariables))
		{
			for (auto i = 0; i < asset->numVariables; i++)
			{
				reader.read_string(pairs[i].value);
			}
		}
	}

	void key_value_pairs::dump(KeyValuePairs* asset)
	{
		filesystem::file file(std::format("keyvaluepairs/{}.csv", xhash_name(asset->name)));
		file.open("wb");

		if (!file.get_fp())
		{
			return;
		}

		std::string text = "key,value\n";
		for (auto i = 0; i < asset->numVariables; i++)
		{
			const auto& pair = asset->keyValuePairs[i];
			text += std::format("{},{}\n", xhash_name(pair.key), pair.value ? pair.value : "");
		}

		file.write(text);
		file.close();
	}

	KeyValuePairs* key_value_pairs::parse(const std::string& name, utils::memory::allocator& allocator)
	{
		filesystem::file file(std::format("keyvaluepairs/{}.csv", name));
		if (!file.exists())
		{
			return nullptr;
		}

		file.open("rb");
		const auto bytes = file.read_bytes(file.size());
		file.close();

		std::vector<std::pair<std::string, std::string>> pairs;
		std::istringstream stream(std::string(bytes.begin(), bytes.end()));
		std::string line;
		std::getline(stream, line);
		while (std::getline(stream, line))
		{
			if (!line.empty() && line.back() == '\r')
			{
				line.pop_back();
			}

			const auto separator = line.find(',');
			if (separator == std::string::npos)
			{
				continue;
			}

			pairs.emplace_back(line.substr(0, separator), line.substr(separator + 1));
		}

		auto* asset = allocator.allocate<KeyValuePairs>();
		asset->name = xhash_from_name(name);
		asset->numVariables = static_cast<std::int32_t>(pairs.size());
		asset->keyValuePairs = allocator.allocate_array<KeyValuePair>(pairs.size());
		for (auto i = 0u; i < pairs.size(); i++)
		{
			asset->keyValuePairs[i].key = xhash_from_name(pairs[i].first);
			asset->keyValuePairs[i].value = allocator.duplicate_string(pairs[i].second);
		}

		return asset;
	}

	void bg_cache::read(auto& reader, BGCache* asset)
	{
		reader.read_array(asset->infos, 7, asset->infoCount);
	}
}

namespace zonetool::t9
{
	template void key_value_pairs::read(zone_reader&, KeyValuePairs*);
	template void bg_cache::read(zone_reader&, BGCache*);
	template void key_value_pairs::read(zone_writer&, KeyValuePairs*);
	template void bg_cache::read(zone_writer&, BGCache*);
}
