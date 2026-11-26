#include <std_include.hpp>
#include "keyvaluepairs.hpp"

namespace zonetool::s4
{
	KeyValuePairs* key_value_pairs::parse(const std::string& name, zone_memory* mem)
	{
		auto file = filesystem::file("keyvaluepairs\\"s + name + ".json"s);
		if (!file.exists())
		{
			return nullptr;
		}

		file.open("rb");
		const auto data = json::parse(file.read_bytes(file.size()));
		file.close();

		auto* asset = mem->allocate<KeyValuePairs>();
		asset->name = mem->duplicate_string(name);
		asset->numVariables = static_cast<int>(data.size());
		asset->keyValuePairs = mem->allocate<KeyValuePair>(asset->numVariables);
		for (auto i = 0; i < asset->numVariables; i++)
		{
			const auto& entry = data[i];
			auto& pair = asset->keyValuePairs[i];
			pair.keyHash = entry["keyHash"].get<unsigned int>();
			if (!entry["value"].is_null())
			{
				pair.value = mem->duplicate_string(entry["value"].get<std::string>());
			}
		}

		return asset;
	}

	void key_value_pairs::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<KeyValuePairs>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	void key_value_pairs::prepare(zone_buffer* buf, zone_memory* mem)
	{
	}

	void key_value_pairs::load_depending(zone_base* zone)
	{
	}

	std::string key_value_pairs::name()
	{
		return this->name_;
	}

	std::int32_t key_value_pairs::type()
	{
		return ASSET_TYPE_KEY_VALUE_PAIRS;
	}

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

	void key_value_pairs::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (data->keyValuePairs)
		{
			buf->align(7);
			const auto pairs = buf->write(data->keyValuePairs, data->numVariables);
			for (auto i = 0; i < data->numVariables; i++)
			{
				if (data->keyValuePairs[i].value)
				{
					pairs[i].value = buf->write_str(data->keyValuePairs[i].value);
				}
			}
			buf->clear_pointer(&dest->keyValuePairs);
		}

		buf->pop_stream();
	}

	void key_value_pairs::dump(KeyValuePairs* asset)
	{
		auto file = filesystem::file("keyvaluepairs\\"s + asset->name + ".json"s);
		file.open("wb");

		ordered_json data = ordered_json::array();
		for (auto i = 0; i < asset->numVariables; i++)
		{
			const auto& pair = asset->keyValuePairs[i];
			data.push_back({{"keyHash", pair.keyHash}, {"value", pair.value ? ordered_json(pair.value) : ordered_json()}});
		}

		file.write(data.dump(4));
		file.close();
	}
}
