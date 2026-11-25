#include <std_include.hpp>
#include "mapents.hpp"

namespace zonetool::iw5
{
	namespace
	{
		void read_triggers(zone_reader32& reader, MapTriggers& trigger)
		{
			reader.read_array(trigger.models, 3, trigger.count);
			reader.read_array(trigger.hulls, 3, trigger.hullCount);
			reader.read_array(trigger.slabs, 3, trigger.slabCount);
		}
	}

	void map_ents::read(zone_reader32& reader, MapEnts* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_array(asset->entityString, 0, asset->numEntityChars);
		read_triggers(reader, asset->trigger);

		auto& client = asset->clientTrigger;
		read_triggers(reader, client.trigger);
		reader.read_array(client.clientTriggerAabbTree, 3, client.numClientTriggerNodes);
		reader.read_array(client.triggerString, 0, client.triggerStringLength);
		reader.read_array(client.triggerStringOffsets, 1, client.trigger.count);
		reader.read_array(client.triggerType, 0, client.trigger.count);
		reader.read_array(client.origins, 3, client.trigger.count);
		reader.read_array(client.scriptDelay, 3, client.trigger.count);
		reader.read_array(client.audioTriggers, 1, client.trigger.count);
		reader.pop_stream();
	}

	void map_ents::dump(MapEnts* asset)
	{
		filesystem::file file(asset->name.get() + ".ents"s);
		file.open("wb");

		if (file.get_fp() && asset->numEntityChars > 0)
		{
			file.write(asset->entityString.get(), asset->numEntityChars, 1);
		}

		file.close();
	}
}
