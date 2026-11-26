#include <std_include.hpp>
#include "netconststrings.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::s4
{
	namespace
	{
		constexpr auto net_const_strings_fields = glz::tuple
		{
			&NetConstStrings::stringType,
			&NetConstStrings::sourceType,
			&NetConstStrings::flags,
		};
	}

	NetConstStrings* net_const_strings::parse(const std::string& name, zone_memory* mem)
	{
		filesystem::file file("netconststrings\\"s + name + ".json"s);
		if (!file.exists())
		{
			return nullptr;
		}

		ZONETOOL_INFO("Parsing netconststrings \"%s\"...", name.data());

		file.open("rb");
		const auto bytes = file.read_bytes(file.size());
		file.close();

		auto data = json::parse(bytes);
		auto* asset = mem->allocate<NetConstStrings>();
		asset->name = mem->duplicate_string(name);
		json_reflect::read<net_const_strings_fields>(data, *asset, mem);
		json_reflect::read_array(data["stringList"], asset->stringList, asset->entryCount, mem);
		return asset;
	}

	void net_const_strings::init(const std::string& name, zone_memory* mem)
	{
		this->name_ = name;

		if (this->referenced())
		{
			this->asset_ = mem->allocate<NetConstStrings>();
			this->asset_->name = mem->duplicate_string(name);
			return;
		}

		this->asset_ = this->parse(name, mem);
	}

	std::string net_const_strings::name()
	{
		return this->name_;
	}

	std::int32_t net_const_strings::type()
	{
		return ASSET_TYPE_NET_CONST_STRINGS;
	}

	void net_const_strings::read(zone_reader& reader, NetConstStrings* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		if (const auto strings = reader.read_array(asset->stringList, 7, asset->entryCount))
		{
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				reader.read_string(strings[i]);
			}
		}
		reader.pop_stream();
	}

	void net_const_strings::write(zone_base* zone, zone_buffer* buf)
	{
		auto* data = this->asset_;
		auto* dest = buf->write(data);

		buf->push_stream(XFILE_BLOCK_VIRTUAL);
		dest->name = buf->write_str(this->name());

		if (const auto strings = write_array(buf, dest->stringList, data->entryCount, 7))
		{
			for (auto i = 0u; i < data->entryCount; i++)
			{
				write_string(buf, strings[i]);
			}
		}
		buf->pop_stream();
	}

	void net_const_strings::dump(NetConstStrings* asset)
	{
		auto file = filesystem::file("netconststrings\\"s + asset->name + ".json"s);
		file.open("wb");

		ordered_json data;
		json_reflect::write<net_const_strings_fields>(data, *asset);
		json_reflect::write_array(data["stringList"], asset->stringList, asset->entryCount);

		file.write(data.dump(4));
		file.close();
	}
}
