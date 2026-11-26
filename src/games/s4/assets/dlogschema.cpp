#include <std_include.hpp>
#include "dlogschema.hpp"

namespace zonetool::s4
{
	namespace
	{
		void read_hash_table(zone_reader& reader, DLogHashTable& table)
		{
			reader.read_array(table.list, 3, table.count);
		}

		void read_serializers(zone_reader& reader, DLogSerializer**& field, const int count)
		{
			if (const auto serializers = reader.read_array(field, 7, count))
			{
				for (auto i = 0; i < count; i++)
				{
					if (const auto serializer = reader.read_single(serializers[i], 7))
					{
						reader.read_string(serializer->name);
					}
				}
			}
		}

		void read_channel(zone_reader& reader, DLogChannel& channel)
		{
			reader.read_string(channel.name);
			read_serializers(reader, channel.serializers, channel.serializerCount);
			read_serializers(reader, channel.postSerializers, channel.postSerializerCount);
			if (const auto endpoint = reader.read_single(channel.endpoint, 7))
			{
				reader.read_string(endpoint->name);
			}
		}

		void read_columns(zone_reader& reader, DLogColumn*& field, const int count)
		{
			const auto columns = reader.read_array(field, 7, count);
			if (!columns)
			{
				return;
			}

			for (auto i = 0; i < count; i++)
			{
				reader.read_string(columns[i].name);
				if (const auto enum_ref = reader.read_single(columns[i].enumRef, 7))
				{
					reader.read_string(enum_ref->name);
					read_hash_table(reader, enum_ref->values);
				}
			}
		}

		void read_event(zone_reader& reader, DLogEvent& event)
		{
			reader.read_string(event.name);
			reader.read_string(event.unknownString);

			if (const auto rows = reader.read_array(event.rows, 7, event.rowCount))
			{
				for (auto i = 0; i < event.rowCount; i++)
				{
					reader.read_string(rows[i].name);
					reader.read_string(rows[i].type);
					read_columns(reader, rows[i].columns, rows[i].columnCount);
					read_hash_table(reader, rows[i].columnHashTable);
				}
			}

			read_hash_table(reader, event.rowHashTable);
			read_columns(reader, event.columns, event.columnCount);
			read_hash_table(reader, event.columnHashTable);

			if (const auto refs = reader.read_array(event.channelRefs, 7, event.channelRefCount))
			{
				for (auto i = 0; i < event.channelRefCount; i++)
				{
					if (const auto channel = reader.read_single(refs[i].channel, 7))
					{
						read_channel(reader, *channel);
					}
					reader.read_string(refs[i].categoryName);
					for (auto& sample : refs[i].samples)
					{
						reader.read_string(sample.name);
					}
				}
			}

			reader.read_string(event.sampleGroup.name);
		}
	}

	void dlog_schema::read(zone_reader& reader, DLogSchema* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->project);

		if (const auto channels = reader.read_array(asset->channels, 7, asset->channelCount))
		{
			for (auto i = 0; i < asset->channelCount; i++)
			{
				read_channel(reader, channels[i]);
			}
		}

		if (const auto events = reader.read_array(asset->events, 7, asset->eventCount))
		{
			for (auto i = 0; i < asset->eventCount; i++)
			{
				read_event(reader, events[i]);
			}
		}

		read_hash_table(reader, asset->eventHashTable);
		reader.pop_stream();
	}
}
