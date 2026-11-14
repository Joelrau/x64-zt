#pragma once

namespace zonetool::s2
{
	class net_const_strings : public asset_base<NetConstStrings, ASSET_TYPE_NET_CONST_STRINGS>
	{
	public:
		static void read(zone_reader& reader, NetConstStrings* asset)
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

		NetConstStrings* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_NET_CONST_STRINGS, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing netconststrings \"%s\"...", name.data());

			auto* asset = read.read_single<NetConstStrings>();
			asset->name = read.read_string();
			if (asset->entryCount)
			{
				asset->stringList = mem->allocate<const char*>(asset->entryCount);
				for (auto i = 0u; i < asset->entryCount; i++)
				{
					asset->stringList[i] = read.read_string();
				}
			}
			else
			{
				asset->stringList = nullptr;
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
			if (data->stringList)
			{
				const char** strings = nullptr;
				dest->stringList = buf->write_s(7, data->stringList, data->entryCount, sizeof(const char*), &strings);
				for (auto i = 0u; i < data->entryCount; i++)
				{
					strings[i] = buf->write_str(data->stringList[i]);
				}
			}

			buf->pop_stream();
		}

		static void dump(NetConstStrings* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_NET_CONST_STRINGS, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			for (auto i = 0u; asset->stringList && i < asset->entryCount; i++)
			{
				write.dump_string(asset->stringList[i]);
			}
			write.close();
		}
	};
}
