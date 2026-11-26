#include <std_include.hpp>
#include "scriptbundle.hpp"

namespace zonetool::t9
{
	void read_script_bundle_object(auto& reader, ScriptBundleObject& object)
	{
		reader.read_array(object.keyValues, 7, object.keyValueCount);

		const auto lists = reader.read_array(object.lists, 7, object.listCount);
		if (!lists)
		{
			return;
		}

		for (auto i = 0ull; i < object.listCount; i++)
		{
			const auto objects = reader.read_array(lists[i].objects, 7, lists[i].objectCount);
			if (!objects)
			{
				continue;
			}

			for (auto j = 0ull; j < lists[i].objectCount; j++)
			{
				read_script_bundle_object(reader, objects[j]);
			}
		}
	}

	void script_bundle::read(auto& reader, ScriptBundle* asset)
	{
		read_script_bundle_object(reader, asset->object);
	}
}

namespace zonetool::t9
{
	template void read_script_bundle_object(zone_reader&, ScriptBundleObject&);
	template void script_bundle::read(zone_reader&, ScriptBundle*);
	template void read_script_bundle_object(zone_writer&, ScriptBundleObject&);
	template void script_bundle::read(zone_writer&, ScriptBundle*);
}
