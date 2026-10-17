#pragma once

namespace zonetool::s2
{
	class com_world : public asset_base<ComWorld, ASSET_TYPE_COMWORLD>
	{
	public:
		static void read(zone_reader& reader, ComWorld* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto lights = reader.read_array(asset->primaryLights, 7, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					reader.read_string(lights[i].defName);
				}
			}
			reader.read_array(asset->primaryLightEnvs, 1, asset->primaryLightEnvCount);
			reader.pop_stream();
		}

		ComWorld* parse(const std::string& name, zone_memory* mem) override
		{
			assetmanager::reader read(mem);
			if (!read.open(dump_path(ASSET_TYPE_COMWORLD, name.data())))
			{
				return nullptr;
			}

			ZONETOOL_INFO("Parsing comworld \"%s\"...", name.data());

			auto* asset = read.read_single<ComWorld>();
			asset->name = read.read_string();
			if (const auto lights = parse_array(read, mem, asset->primaryLights, asset->primaryLightCount))
			{
				for (auto i = 0u; i < asset->primaryLightCount; i++)
				{
					lights[i].defName = read.read_string();
				}
			}
			parse_array(read, mem, asset->primaryLightEnvs, asset->primaryLightEnvCount);
			read.close();

			return asset;
		}

		void write(zone_base* zone, zone_buffer* buf) override
		{
			auto* data = this->asset_;
			auto* dest = buf->write(data);

			buf->push_stream(XFILE_BLOCK_VIRTUAL);

			dest->name = this->write_name(buf);
			if (const auto dest_lights = write_array(buf, 7, data->primaryLights, data->primaryLightCount, dest->primaryLights))
			{
				for (auto i = 0u; i < data->primaryLightCount; i++)
				{
					write_string(buf, data->primaryLights[i].defName, dest_lights[i].defName);
				}
			}
			write_array(buf, 1, data->primaryLightEnvs, data->primaryLightEnvCount, dest->primaryLightEnvs);

			buf->pop_stream();
		}

		static void dump(ComWorld* asset)
		{
			assetmanager::dumper write;
			if (!write.open(dump_path(ASSET_TYPE_COMWORLD, asset->name)))
			{
				return;
			}

			write.dump_single(asset);
			write.dump_string(asset->name);
			write.dump_array(asset->primaryLights, asset->primaryLightCount);
			for (auto i = 0u; asset->primaryLights && i < asset->primaryLightCount; i++)
			{
				write.dump_string(asset->primaryLights[i].defName);
			}
			write.dump_array(asset->primaryLightEnvs, asset->primaryLightEnvCount);
			write.close();
		}
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
						reader.read_string(glass_names[i].name);
						reader.read_script_string(glass_names[i].targetName);
						reader.read_array(glass_names[i].indices, 1, glass_names[i].indexCount);
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
						glass_names[i].name = read.read_string();
						glass_names[i].targetName = parse_script_string(read);
						parse_array(read, mem, glass_names[i].indices, glass_names[i].indexCount);
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
				prepare(glass_data->glassNames[i].targetName);
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
						write_string(buf, glass->name, dest_glass_names[i].name);
						write_array(buf, 1, glass->indices, glass->indexCount, dest_glass_names[i].indices);
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
					write.dump_string(glass->name);
					dump_script_string(write, glass->targetName);
					write.dump_array(glass->indices, glass->indexCount);
				}
			}
			write.close();
		}
	};
}
