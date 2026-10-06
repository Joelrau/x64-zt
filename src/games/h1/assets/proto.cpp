#include <std_include.hpp>
#include "proto.hpp"

namespace zonetool::h1
{
	namespace
	{
		void read_fields(zone_reader& reader, Proto_A_A*& field, int count);

		template <typename T>
		void read_named_values(zone_reader& reader, T* list)
		{
			reader.read_string(list->unk1);

			if (const auto values = reader.read_array(list->unk2, 3, list->unk2_count))
			{
				for (auto i = 0; i < list->unk2_count; i++)
				{
					reader.read_string(values[i].unk1);
				}
			}
		}

		template <typename T>
		void read_message(zone_reader& reader, T* message)
		{
			reader.read_string(message->unk1);
			read_fields(reader, message->unk2, message->unk2_count);
			reader.read_array(message->unk3.unk1, 3, message->unk3.unk1_count);
		}

		void read_fields(zone_reader& reader, Proto_A_A*& field, const int count)
		{
			if (const auto fields = reader.read_array(field, 3, count))
			{
				for (auto i = 0; i < count; i++)
				{
					reader.read_string(fields[i].unk1);

					if (const auto message = reader.read_single(fields[i].unk2, 3))
					{
						read_message(reader, message);
					}

					if (const auto values = reader.read_single(fields[i].unk3, 3))
					{
						read_named_values(reader, values);
					}
				}
			}
		}
	}

	void proto::read(zone_reader& reader, Proto* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_string(asset->checksum);

		if (const auto messages = reader.read_array(asset->unk2, 3, asset->unk2_count))
		{
			for (auto i = 0; i < asset->unk2_count; i++)
			{
				read_message(reader, &messages[i]);
			}
		}

		reader.read_array(asset->unk3.unk1, 3, asset->unk3.unk1_count);

		if (const auto values = reader.read_array(asset->unk4, 3, asset->unk4_count))
		{
			for (auto i = 0; i < asset->unk4_count; i++)
			{
				read_named_values(reader, &values[i]);
			}
		}

		reader.read_array(asset->unk5.unk1, 3, asset->unk5.unk1_count);
		reader.pop_stream();
	}
}
