#pragma once

namespace zonetool::s2
{
	class com_world
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
	};

	class glass_world
	{
	public:
		static void read(zone_reader& reader, GlassWorld* asset)
		{
			reader.push_stream(XFILE_BLOCK_VIRTUAL);
			reader.read_string(asset->name);
			if (const auto data = reader.read_single(asset->g_glassData, 7))
			{
				reader.read_array(data->glassPieces, 3, data->pieceCount);
				if (const auto names = reader.read_array(data->glassNames, 7, data->glassNameCount))
				{
					for (auto i = 0u; i < data->glassNameCount; i++)
					{
						reader.read_string(names[i].name);
						reader.read_script_string(names[i].targetName);
						reader.read_array(names[i].indices, 1, names[i].indexCount);
					}
				}
			}
			reader.pop_stream();
		}
	};
}
