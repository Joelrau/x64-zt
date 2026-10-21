#pragma once

namespace zonetool::s2
{
	class dlog_schema : public asset_base<DLogSchema, ASSET_TYPE_DLOG_SCHEMA>
	{
	public:
		static void read(zone_reader& reader, DLogSchema* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto events = reader.read_array(asset->events, 7, asset->eventCount))
			{
				for (auto i = 0; i < asset->eventCount; i++)
				{
					read_event(reader, &events[i]);
				}
			}
			read_hash_table(reader, &asset->hashTable);
			reader.pop_stream();
		}

		DLogSchema* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_DLOG_SCHEMA, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing dlogschema \"%s\"...", name.data());

			auto* asset = read.read_single<DLogSchema>();
			asset->name = read.read_string();
			if (const auto events = parse_array(read, mem, asset->events, asset->eventCount))
			{
				for (auto i = 0; i < asset->eventCount; i++)
				{
					parse_event(read, mem, &events[i]);
				}
			}
			parse_array(read, mem, asset->hashTable.list, asset->hashTable.count);
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_events = write_array(buf, 7, data->events, data->eventCount, dest->events))
			{
				for (auto i = 0; i < data->eventCount; i++)
				{
					write_event(buf, &data->events[i], &dest_events[i]);
				}
			}
			write_hash_table(buf, &data->hashTable, &dest->hashTable);

			buf->pop_stream();
		}

		static void dump(DLogSchema* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_DLOG_SCHEMA, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->events, asset->eventCount);
			for (auto i = 0; asset->events && i < asset->eventCount; i++)
			{
				dump_event(write, &asset->events[i]);
			}
			write.dump_array(asset->hashTable.list, asset->hashTable.count);
			write.close();
		}

	private:
		static void read_hash_table(zone_reader& reader, DLogHashTable* table)
		{
			reader.read_array(table->list, 3, table->count);
		}

		static void read_event(zone_reader& reader, DLogEvent* event)
		{
			reader.read_string(event->name);
			if (const auto fields = reader.read_array(event->fields, 7, event->fieldCount))
			{
				for (auto i = 0; i < event->fieldCount; i++)
				{
					reader.read_string(fields[i].name);
					reader.read_string(fields[i].typeName);
					if (const auto values = reader.read_array(fields[i].values, 7, fields[i].valueCount))
					{
						for (auto j = 0; j < fields[i].valueCount; j++)
						{
							reader.read_string(values[j].name);
						}
					}
					read_hash_table(reader, &fields[i].hashTable);
				}
			}
			read_hash_table(reader, &event->hashTable);
		}

		static void parse_event(assetmanager::reader& read, zone_memory* mem, DLogEvent* event)
		{
			event->name = read.read_string();
			if (const auto fields = parse_array(read, mem, event->fields, event->fieldCount))
			{
				for (auto i = 0; i < event->fieldCount; i++)
				{
					fields[i].name = read.read_string();
					fields[i].typeName = read.read_string();
					if (const auto values = parse_array(read, mem, fields[i].values, fields[i].valueCount))
					{
						for (auto j = 0; j < fields[i].valueCount; j++)
						{
							values[j].name = read.read_string();
						}
					}
					parse_array(read, mem, fields[i].hashTable.list, fields[i].hashTable.count);
				}
			}
			parse_array(read, mem, event->hashTable.list, event->hashTable.count);
		}

		static void write_hash_table(zone_buffer* buf, DLogHashTable* table, DLogHashTable* dest)
		{
			write_array(buf, 3, table->list, table->count, dest->list);
		}

		static void write_event(zone_buffer* buf, DLogEvent* event, DLogEvent* dest)
		{
			write_string(buf, event->name, dest->name);
			if (const auto dest_fields = write_array(buf, 7, event->fields, event->fieldCount, dest->fields))
			{
				for (auto i = 0; i < event->fieldCount; i++)
				{
					auto* field = &event->fields[i];
					write_string(buf, field->name, dest_fields[i].name);
					write_string(buf, field->typeName, dest_fields[i].typeName);
					if (const auto dest_values = write_array(buf, 7, field->values, field->valueCount, dest_fields[i].values))
					{
						for (auto j = 0; j < field->valueCount; j++)
						{
							write_string(buf, field->values[j].name, dest_values[j].name);
						}
					}
					write_hash_table(buf, &field->hashTable, &dest_fields[i].hashTable);
				}
			}
			write_hash_table(buf, &event->hashTable, &dest->hashTable);
		}

		static void dump_event(assetmanager::dumper& write, DLogEvent* event)
		{
			write.dump_string(event->name);
			write.dump_array(event->fields, event->fieldCount);
			for (auto i = 0; event->fields && i < event->fieldCount; i++)
			{
				auto* field = &event->fields[i];
				write.dump_string(field->name);
				write.dump_string(field->typeName);
				write.dump_array(field->values, field->valueCount);
				for (auto j = 0; field->values && j < field->valueCount; j++)
				{
					write.dump_string(field->values[j].name);
				}
				write.dump_array(field->hashTable.list, field->hashTable.count);
			}
			write.dump_array(event->hashTable.list, event->hashTable.count);
		}
	};

	class dlog_routes : public asset_base<DLogRoutes, ASSET_TYPE_DLOG_ROUTES>
	{
	public:
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

		DLogRoutes* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_DLOG_ROUTES, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing dlogroutes \"%s\"...", name.data());

			auto* asset = read.read_single<DLogRoutes>();
			asset->name = read.read_string();
			if (const auto destinations = parse_array(read, mem, asset->destinations, asset->destinationCount))
			{
				for (auto i = 0; i < asset->destinationCount; i++)
				{
					parse_destination(read, &destinations[i]);
				}
			}
			if (const auto routes = parse_array(read, mem, asset->routes, asset->routeCount))
			{
				for (auto i = 0; i < asset->routeCount; i++)
				{
					routes[i].name = read.read_string();
					if (const auto entries = parse_array(read, mem, routes[i].entries, routes[i].entryCount))
					{
						for (auto j = 0; j < routes[i].entryCount; j++)
						{
							if (const auto destination = parse_array(read, mem, entries[j].destination, 1))
							{
								parse_destination(read, destination);
							}
							entries[j].name = read.read_string();
						}
					}
				}
			}
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_destinations = write_array(buf, 7, data->destinations, data->destinationCount, dest->destinations))
			{
				for (auto i = 0; i < data->destinationCount; i++)
				{
					write_destination(buf, &data->destinations[i], &dest_destinations[i]);
				}
			}
			if (const auto dest_routes = write_array(buf, 7, data->routes, data->routeCount, dest->routes))
			{
				for (auto i = 0; i < data->routeCount; i++)
				{
					auto* route = &data->routes[i];
					write_string(buf, route->name, dest_routes[i].name);
					if (const auto dest_entries = write_array(buf, 7, route->entries, route->entryCount, dest_routes[i].entries))
					{
						for (auto j = 0; j < route->entryCount; j++)
						{
							if (const auto dest_destination = write_array(buf, 7, route->entries[j].destination, 1, dest_entries[j].destination))
							{
								write_destination(buf, route->entries[j].destination, dest_destination);
							}
							write_string(buf, route->entries[j].name, dest_entries[j].name);
						}
					}
				}
			}

			buf->pop_stream();
		}

		static void dump(DLogRoutes* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_DLOG_ROUTES, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->destinations, asset->destinationCount);
			for (auto i = 0; asset->destinations && i < asset->destinationCount; i++)
			{
				dump_destination(write, &asset->destinations[i]);
			}
			write.dump_array(asset->routes, asset->routeCount);
			for (auto i = 0; asset->routes && i < asset->routeCount; i++)
			{
				auto* route = &asset->routes[i];
				write.dump_string(route->name);
				write.dump_array(route->entries, route->entryCount);
				for (auto j = 0; route->entries && j < route->entryCount; j++)
				{
					write.dump_single(route->entries[j].destination);
					if (route->entries[j].destination)
					{
						dump_destination(write, route->entries[j].destination);
					}
					write.dump_string(route->entries[j].name);
				}
			}
			write.close();
		}

	private:
		static void read_destination(zone_reader& reader, DLogDestination* destination)
		{
			reader.read_string(destination->name);
			reader.read_string(destination->type);
			reader.read_string(destination->address);
		}

		static void parse_destination(assetmanager::reader& read, DLogDestination* destination)
		{
			destination->name = read.read_string();
			destination->type = read.read_string();
			destination->address = read.read_string();
		}

		static void write_destination(zone_buffer* buf, DLogDestination* destination, DLogDestination* dest)
		{
			write_string(buf, destination->name, dest->name);
			write_string(buf, destination->type, dest->type);
			write_string(buf, destination->address, dest->address);
		}

		static void dump_destination(assetmanager::dumper& write, DLogDestination* destination)
		{
			write.dump_string(destination->name);
			write.dump_string(destination->type);
			write.dump_string(destination->address);
		}
	};
}
