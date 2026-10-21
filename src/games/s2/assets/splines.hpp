#pragma once

namespace zonetool::s2
{
	class splines : public asset_base<Spline, ASSET_TYPE_SPLINES>
	{
	public:
		static void read(zone_reader& reader, Spline* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto splines = reader.read_array(asset->splines, 7, asset->splineCount))
			{
				for (auto i = 0; i < asset->splineCount; i++)
				{
					reader.read_array(splines[i].points, 3, splines[i].pointCount);
					reader.read_array(splines[i].data, 0, splines[i].dataSize);
					reader.read_script_string(splines[i].name);
				}
			}
			reader.pop_stream();
		}

		Spline* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_SPLINES, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing splines \"%s\"...", name.data());

			auto* asset = read.read_single<Spline>();
			asset->name = read.read_string();
			if (const auto splines = parse_array(read, mem, asset->splines, asset->splineCount))
			{
				for (auto i = 0; i < asset->splineCount; i++)
				{
					parse_array(read, mem, splines[i].points, splines[i].pointCount);
					parse_array(read, mem, splines[i].data, splines[i].dataSize);
					splines[i].name = parse_script_string(read);
				}
			}
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			script_string_preparer prepare(buf);
			for (auto i = 0; this->asset_->splines && i < this->asset_->splineCount; i++)
			{
				prepare(this->asset_->splines[i].name);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_splines = write_array(buf, 7, data->splines, data->splineCount, dest->splines))
			{
				for (auto i = 0; i < data->splineCount; i++)
				{
					write_array(buf, 3, data->splines[i].points, data->splines[i].pointCount, dest_splines[i].points);
					write_array(buf, 0, data->splines[i].data, data->splines[i].dataSize, dest_splines[i].data);
				}
			}

			buf->pop_stream();
		}

		static void dump(Spline* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_SPLINES, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->splines, asset->splineCount);
			for (auto i = 0; asset->splines && i < asset->splineCount; i++)
			{
				write.dump_array(asset->splines[i].points, asset->splines[i].pointCount);
				write.dump_array(asset->splines[i].data, asset->splines[i].dataSize);
				dump_script_string(write, asset->splines[i].name);
			}
			write.close();
		}
	};
}
