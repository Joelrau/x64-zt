#include <std_include.hpp>
#include "scriptbundle.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_object(zone_reader& reader, ScriptBundleObject& object);

		void read_value(zone_reader& reader, ScriptBundleValue& value)
		{
			if (value.type == SCRIPT_BUNDLE_VALUE_OBJECT)
			{
				read_object(reader, value.data.object);
			}
			else if (value.type == SCRIPT_BUNDLE_VALUE_ARRAY)
			{
				auto& array = value.data.array;
				if (const auto values = reader.read_array(array.values, 7, array.count))
				{
					for (auto i = 0u; i < array.count; i++)
					{
						read_value(reader, values[i]);
					}
				}
			}
		}

		void read_object(zone_reader& reader, ScriptBundleObject& object)
		{
			if (const auto entries = reader.read_array(object.entries, 7, object.count))
			{
				for (auto i = 0u; i < object.count; i++)
				{
					read_value(reader, entries[i].value);
				}
			}
		}
	}

	void script_bundle::read(zone_reader& reader, ScriptBundle* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->data, 0, asset->dataSize);
		read_object(reader, asset->root);
		reader.pop_stream();
	}
}
