#pragma once

namespace zonetool::s2
{
	class dlog_schema
	{
	public:
		static void read_hash_table(zone_reader& reader, DLogHashTable& table)
		{
			reader.read_stream(table.list, 3, 8, table.count);
		}

		static void read_field(zone_reader& reader, DLogField* field)
		{
			reader.read_string(field->name);
			reader.read_string(field->typeName);
			if (const auto values = reader.read_array(field->values, 7, field->valueCount))
			{
				for (auto i = 0; i < field->valueCount; i++)
				{
					reader.read_string(values[i].name);
				}
			}
			read_hash_table(reader, field->hashTable);
		}

		static void read(zone_reader& reader, DLogSchema* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto events = reader.read_array(asset->events, 7, asset->eventCount))
			{
				for (auto i = 0; i < asset->eventCount; i++)
				{
					reader.read_string(events[i].name);
					if (const auto fields = reader.read_array(events[i].fields, 7, events[i].fieldCount))
					{
						for (auto j = 0; j < events[i].fieldCount; j++)
						{
							read_field(reader, &fields[j]);
						}
					}
					read_hash_table(reader, events[i].hashTable);
				}
			}
			read_hash_table(reader, asset->hashTable);
			reader.pop_stream();
		}
	};

	class dlog_routes
	{
	public:
		static void read_destination(zone_reader& reader, DLogDestination* destination)
		{
			reader.read_string(destination->name);
			reader.read_string(destination->type);
			reader.read_string(destination->address);
		}

		static void read(zone_reader& reader, DLogRoutes* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto destinations = reader.read_array(asset->destinations, 7, asset->destinationCount))
			{
				for (auto i = 0; i < asset->destinationCount; i++)
				{
					read_destination(reader, &destinations[i]);
				}
			}

			if (const auto routes = reader.read_array(asset->routes, 7, asset->routeCount))
			{
				for (auto i = 0; i < asset->routeCount; i++)
				{
					reader.read_string(routes[i].name);
					if (const auto entries = reader.read_array(routes[i].entries, 7, routes[i].entryCount))
					{
						for (auto j = 0; j < routes[i].entryCount; j++)
						{
							if (const auto destination = reader.read_single(entries[j].destination, 7))
							{
								read_destination(reader, destination);
							}
							reader.read_string(entries[j].name);
						}
					}
				}
			}
			reader.pop_stream();
		}
	};
}
