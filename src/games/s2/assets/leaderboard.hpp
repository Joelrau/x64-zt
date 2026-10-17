#pragma once

namespace zonetool::s2
{
	class leaderboard : public asset_base<LeaderboardDef, ASSET_TYPE_LEADERBOARDDEF>
	{
	public:
		static void read(zone_reader& reader, LeaderboardDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto columns = reader.read_array(asset->columns, 7, asset->columnCount))
			{
				for (auto i = 0; i < asset->columnCount; i++)
				{
					reader.read_string(columns[i].name);
					reader.read_string(columns[i].statName);
				}
			}
			reader.pop_stream();
		}

		LeaderboardDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_LEADERBOARDDEF, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing leaderboard \"%s\"...", name.data());

			auto* asset = read.read_single<LeaderboardDef>();
			asset->name = read.read_string();
			if (const auto columns = parse_array(read, mem, asset->columns, asset->columnCount))
			{
				for (auto i = 0; i < asset->columnCount; i++)
				{
					columns[i].name = read.read_string();
					columns[i].statName = read.read_string();
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
			if (const auto dest_columns = write_array(buf, 7, data->columns, data->columnCount, dest->columns))
			{
				for (auto i = 0; i < data->columnCount; i++)
				{
					write_string(buf, data->columns[i].name, dest_columns[i].name);
					write_string(buf, data->columns[i].statName, dest_columns[i].statName);
				}
			}

			buf->pop_stream();
		}

		static void dump(LeaderboardDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_LEADERBOARDDEF, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->columns, asset->columnCount);
			for (auto i = 0; asset->columns && i < asset->columnCount; i++)
			{
				write.dump_string(asset->columns[i].name);
				write.dump_string(asset->columns[i].statName);
			}
			write.close();
		}
	};

	class virtual_leaderboard : public asset_base<VirtualLeaderboardDef, ASSET_TYPE_VIRTUAL_LEADERBOARD>
	{
	public:
		static void read(zone_reader& reader, VirtualLeaderboardDef* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			reader.read_string(asset->sourceName);
			if (const auto columns = reader.read_array(asset->columns, 7, asset->columnCount))
			{
				for (auto i = 0; i < asset->columnCount; i++)
				{
					reader.read_string(columns[i].name);
				}
			}
			reader.pop_stream();
		}

		VirtualLeaderboardDef* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_VIRTUAL_LEADERBOARD, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing virtual leaderboard \"%s\"...", name.data());

			auto* asset = read.read_single<VirtualLeaderboardDef>();
			asset->name = read.read_string();
			asset->sourceName = read.read_string();
			if (const auto columns = parse_array(read, mem, asset->columns, asset->columnCount))
			{
				for (auto i = 0; i < asset->columnCount; i++)
				{
					columns[i].name = read.read_string();
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
			write_string(buf, data->sourceName, dest->sourceName);
			if (const auto dest_columns = write_array(buf, 7, data->columns, data->columnCount, dest->columns))
			{
				for (auto i = 0; i < data->columnCount; i++)
				{
					write_string(buf, data->columns[i].name, dest_columns[i].name);
				}
			}

			buf->pop_stream();
		}

		static void dump(VirtualLeaderboardDef* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_VIRTUAL_LEADERBOARD, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_string(asset->sourceName);
			write.dump_array(asset->columns, asset->columnCount);
			for (auto i = 0; asset->columns && i < asset->columnCount; i++)
			{
				write.dump_string(asset->columns[i].name);
			}
			write.close();
		}
	};
}
