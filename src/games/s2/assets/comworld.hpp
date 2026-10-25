#pragma once

namespace zonetool::s2
{
	class com_world : public asset_base<ComWorld, ASSET_TYPE_COMWORLD>
	{
	public:
		static void read(zone_reader& reader, ComWorld* asset);
		ComWorld* parse(const std::string& name, zone_memory* mem) override;
		void write(zone_base* zone, zone_buffer* buf) override;
		static void dump(ComWorld* asset);
	};

	class glass_world : public asset_base<GlassWorld, ASSET_TYPE_GLASSWORLD>
	{
	public:
		static void read(zone_reader& reader, GlassWorld* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto glass_data = reader.read_single(asset->g_glassData, 7))
			{
				reader.read_array(glass_data->glassPieces, 3, glass_data->pieceCount);
				if (const auto glass_names = reader.read_array(glass_data->glassNames, 7, glass_data->glassNameCount))
				{
					for (auto i = 0u; i < glass_data->glassNameCount; i++)
					{
						reader.read_string(glass_names[i].nameStr);
						reader.read_script_string(glass_names[i].name);
						reader.read_array(glass_names[i].pieceIndices, 1, glass_names[i].pieceCount);
					}
				}
			}
			reader.pop_stream();
		}

		GlassWorld* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_GLASSWORLD, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing glassworld \"%s\"...", name.data());

			auto* asset = read.read_single<GlassWorld>();
			asset->name = read.read_string();
			if (const auto glass_data = parse_array(read, mem, asset->g_glassData, 1))
			{
				parse_array(read, mem, glass_data->glassPieces, glass_data->pieceCount);
				if (const auto glass_names = parse_array(read, mem, glass_data->glassNames, glass_data->glassNameCount))
				{
					for (auto i = 0u; i < glass_data->glassNameCount; i++)
					{
						glass_names[i].nameStr = read.read_string();
						glass_names[i].name = parse_script_string(read);
						parse_array(read, mem, glass_names[i].pieceIndices, glass_names[i].pieceCount);
					}
				}
			}
			read.close();

			return asset;
		}

		void prepare(zone_buffer* buf, zone_memory* mem) override
		{
			const auto* glass_data = this->asset_->g_glassData;
			if (!glass_data || !glass_data->glassNames)
			{
				return;
			}

			script_string_preparer prepare(buf);
			for (auto i = 0u; i < glass_data->glassNameCount; i++)
			{
				prepare(glass_data->glassNames[i].name);
			}
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_glass_data = write_array(buf, 7, data->g_glassData, 1, dest->g_glassData))
			{
				auto* glass_data = data->g_glassData;
				write_array(buf, 3, glass_data->glassPieces, glass_data->pieceCount, dest_glass_data->glassPieces);
				if (const auto dest_glass_names = write_array(buf, 7, glass_data->glassNames, glass_data->glassNameCount, dest_glass_data->glassNames))
				{
					for (auto i = 0u; i < glass_data->glassNameCount; i++)
					{
						auto* glass = &glass_data->glassNames[i];
						write_string(buf, glass->nameStr, dest_glass_names[i].nameStr);
						write_array(buf, 1, glass->pieceIndices, glass->pieceCount, dest_glass_names[i].pieceIndices);
					}
				}
			}

			buf->pop_stream();
		}

		static void dump(GlassWorld* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_GLASSWORLD, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_single(asset->g_glassData);
			if (const auto* glass_data = asset->g_glassData)
			{
				write.dump_array(glass_data->glassPieces, glass_data->pieceCount);
				write.dump_array(glass_data->glassNames, glass_data->glassNameCount);
				for (auto i = 0u; glass_data->glassNames && i < glass_data->glassNameCount; i++)
				{
					auto* glass = &glass_data->glassNames[i];
					write.dump_string(glass->nameStr);
					dump_script_string(write, glass->name);
					write.dump_array(glass->pieceIndices, glass->pieceCount);
				}
			}
			write.close();
		}
	};
}
