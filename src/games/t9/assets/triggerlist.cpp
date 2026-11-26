#include <std_include.hpp>
#include "triggerlist.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_groups(auto& reader, TriggerGroup*& field, const std::uint64_t count)
		{
			const auto groups = reader.read_array(field, 7, count);
			if (!groups)
			{
				return;
			}

			for (auto i = 0ull; i < count; i++)
			{
				const auto entries = reader.read_array(groups[i].entries, 7, groups[i].entryCount);
				if (!entries)
				{
					continue;
				}

				for (auto j = 0u; j < groups[i].entryCount; j++)
				{
					reader.read_string(entries[j].key);
					if (entries[j].type == TRIGGER_ENTRY_TYPE_STRING)
					{
						reader.read_string(entries[j].value.string);
					}
				}
			}
		}

		void write_entities(const std::string& path, const TriggerGroup* groups, const std::uint64_t count)
		{
			std::string text;
			for (auto i = 0ull; groups && i < count; i++)
			{
				text += "{\n";
				for (auto j = 0u; groups[i].entries && j < groups[i].entryCount; j++)
				{
					const auto& entry = groups[i].entries[j];
					text += std::format("\"{}\" \"{}\"\n", entry.key ? entry.key : "", trigger_entry_value(entry));
				}

				text += "}\n";
			}

			filesystem::file file(path);
			file.open("wb");
			file.write(text.data(), text.size(), 1);
			file.close();
		}
	}

	std::string trigger_entry_value(const TriggerEntry& entry)
	{
		switch (entry.type)
		{
		case TRIGGER_ENTRY_TYPE_STRING:
			return entry.value.string ? entry.value.string : "";
		case TRIGGER_ENTRY_TYPE_VECTOR:
			return std::format("{} {} {}", entry.value.vector[0], entry.value.vector[1], entry.value.vector[2]);
		case TRIGGER_ENTRY_TYPE_HASH:
			return xhash_name(entry.value.hash);
		case TRIGGER_ENTRY_TYPE_FLOAT:
			return std::format("{}", entry.value.number);
		case TRIGGER_ENTRY_TYPE_INT:
			return std::format("{}", entry.value.integer);
		default:
			ZONETOOL_WARNING("Entity key \"%s\" has unknown value type %u", entry.key ? entry.key : "", static_cast<unsigned>(entry.type));
			return std::format("<type {}>", static_cast<int>(entry.type));
		}
	}

	void trigger_list::read(auto& reader, TriggerList* asset)
	{
		reader.read_array(asset->models, 3, asset->modelCount);
		reader.read_array(asset->hulls, 3, asset->hullCount);
		reader.read_array(asset->slabs, 3, asset->slabCount);
		read_groups(reader, asset->groups, asset->groupCount);
	}

	void trigger_list::dump(TriggerList* asset)
	{
		write_entities(std::format("maps/{}.triggerlist.ents", xhash_name(asset->name)), asset->groups, asset->groupCount);

		filesystem::file file(std::format("maps/{}.triggerlist.raw", xhash_name(asset->name)));
		file.open("wb");
		file.write(asset);
		if (asset->models)
		{
			file.write(asset->models, sizeof(*asset->models), asset->modelCount);
		}

		if (asset->hulls)
		{
			file.write(asset->hulls, sizeof(*asset->hulls), asset->hullCount);
		}

		if (asset->slabs)
		{
			file.write(asset->slabs, sizeof(*asset->slabs), asset->slabCount);
		}

		file.close();
	}

	void entity_list::read(auto& reader, EntityList* asset)
	{
		read_groups(reader, asset->groups, asset->groupCount);
	}

	void entity_list::dump(EntityList* asset)
	{
		write_entities(std::format("maps/{}.entitylist.ents", xhash_name(asset->name)), asset->groups, asset->groupCount);
	}
}

namespace zonetool::t9
{
	template void trigger_list::read(zone_reader&, TriggerList*);
	template void entity_list::read(zone_reader&, EntityList*);
	template void trigger_list::read(zone_writer&, TriggerList*);
	template void entity_list::read(zone_writer&, EntityList*);
}
