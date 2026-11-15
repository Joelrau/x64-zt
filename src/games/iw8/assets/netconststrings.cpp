#include <std_include.hpp>
#include "netconststrings.hpp"

#include "io/json_reflect.hpp"

namespace zonetool::iw8
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
