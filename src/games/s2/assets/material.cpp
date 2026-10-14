#include <std_include.hpp>
#include "s2.hpp"

namespace zonetool::s2
{
	namespace
	{
		void read_constant_buffer(zone_reader& reader, MaterialConstantBufferDef* buffer)
		{
			reader.read_array(buffer->vsData, 15, buffer->vsDataSize);
			reader.read_array(buffer->hsData, 15, buffer->hsDataSize);
			reader.read_array(buffer->dsData, 15, buffer->dsDataSize);
			reader.read_array(buffer->psData, 15, buffer->psDataSize);
			reader.read_array(buffer->vsOffsetData, 1, buffer->vsOffsetDataSize);
			reader.read_array(buffer->hsOffsetData, 1, buffer->hsOffsetDataSize);
			reader.read_array(buffer->dsOffsetData, 1, buffer->dsOffsetDataSize);
			reader.read_array(buffer->psOffsetData, 1, buffer->psOffsetDataSize);
		}
	}

	void material::read(zone_reader& reader, Material* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);
		reader.read_asset(ASSET_TYPE_TECHNIQUE_SET, asset->techniqueSet);

		if (const auto textures = reader.read_array(asset->textureTable, 7, asset->textureCount))
		{
			for (auto i = 0; i < asset->textureCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, textures[i].u.image);
			}
		}

		reader.read_array(asset->constantTable, 15, asset->constantCount);
		reader.read_array(asset->stateBitsTable, 3, asset->stateBitsCount);
		reader.read_asset(ASSET_TYPE_MATERIAL, asset->parentMaterial);

		if (const auto buffers = reader.read_array(asset->constantBufferTable, 7, asset->constantBufferCount))
		{
			for (auto i = 0; i < asset->constantBufferCount; i++)
			{
				read_constant_buffer(reader, &buffers[i]);
			}
		}

		if (const auto sub_materials = reader.read_array(asset->subMaterials, 7, asset->layerCount))
		{
			for (auto i = 0; i < asset->layerCount; i++)
			{
				reader.read_string(sub_materials[i]);
			}
		}

		reader.pop_stream();
	}
}
