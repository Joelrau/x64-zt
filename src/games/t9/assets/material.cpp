#include <std_include.hpp>
#include "material.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_physical_data(auto& reader, MaterialPhysicalData& data)
		{
			for (auto& field : data.entrySets)
			{
				if (const auto entry_set = reader.read_array(field, 7))
				{
					for (auto& entry : entry_set->entries)
					{
						reader.read_array(entry, 3);
					}
				}
			}

			reader.push_stream(XFILE_BLOCK_PHYSICAL);
			reader.read_array(data.data, 0xFF, data.dataSize);
			reader.pop_stream();
		}

		void read_texture_table(auto& reader, Material* asset)
		{
			if (const auto textures = reader.read_array(asset->textureTable, 7, asset->textureCount))
			{
				for (auto i = 0u; i < asset->textureCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, textures[i].image);
				}
			}
		}

		void read_image_set(auto& reader, MaterialImageSet*& field)
		{
			const auto image_set = reader.read_array(field, 7);
			if (!image_set)
			{
				return;
			}

			reader.read_array(image_set->instanceData, 0x3F, image_set->instanceDataCount);
			if (const auto entries = reader.read_array(image_set->entries, 7, image_set->entryCount))
			{
				for (auto i = 0u; i < image_set->entryCount; i++)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, entries[i].image);
				}
			}
		}

		void read_embedded_technique_set(auto& reader, MaterialEmbeddedTechniqueSet*& field)
		{
			const auto embedded = reader.read_array(field, 7);
			if (!embedded)
			{
				return;
			}

			reader.read_array(embedded->records, 0xF, embedded->recordCount);
			if (const auto technique_set = reader.read_array(embedded->techniqueSet, 7))
			{
				reader.push_stream(XFILE_BLOCK_VIRTUAL);
				techset::read(reader, technique_set);
				reader.pop_stream();
			}
		}
	}

	void material::read(auto& reader, Material* asset)
	{
		read_physical_data(reader, asset->physicalData);
		reader.read_asset(ASSET_TYPE_TECHSET, asset->techniqueSet);
		read_texture_table(reader, asset);
		reader.read_array(asset->samplerTable, 0xF, asset->samplerCount);

		for (auto& image_set : asset->imageSets)
		{
			read_image_set(reader, image_set);
		}

		reader.read_array(asset->recordTable.records, 3, asset->recordTable.recordCount);
		reader.read_array(asset->recordTable.values, 3, asset->recordTable.valueCount);
		read_embedded_technique_set(reader, asset->embeddedTechniqueSet);
	}

	void material::dump(Material* asset)
	{
		filesystem::file file(std::format("materials/{}.material.txt", xhash_name(asset->name)));
		file.open("wb");
		if (!file.get_fp())
		{
			return;
		}

		const auto image_name = [](const GfxImage* image)
		{
			return image ? xhash_name(image->name) : std::string("null");
		};

		const auto hex = [](const void* data, const std::size_t size)
		{
			std::string text;
			for (auto i = 0u; i < size; i++)
			{
				text += std::format("{:02X}", static_cast<const std::uint8_t*>(data)[i]);
			}
			return text;
		};

		auto text = std::format("techset: {}\n", asset->techniqueSet ? xhash_name(asset->techniqueSet->name) : "null");
		text += std::format("header: {}{}{} {:02X}{}\n", hex(asset->__pad0, sizeof(asset->__pad0)), hex(&asset->surfaceTypeBits, sizeof(asset->surfaceTypeBits)),
			hex(&asset->contents, sizeof(asset->contents)), static_cast<std::uint32_t>(asset->drawList),
			hex(asset->__pad1, sizeof(asset->__pad1)));
		for (auto i = 0u; asset->textureTable && i < asset->textureCount; i++)
		{
			const auto& texture = asset->textureTable[i];
			text += std::format("texture {}: {} {:08X} {} {} {:X}\n", i, image_name(texture.image), static_cast<std::uint32_t>(texture.slot),
				texture.uvScale[0], texture.uvScale[1], texture.flags);
		}

		for (auto i = 0u; asset->samplerTable && i < asset->samplerCount; i++)
		{
			text += std::format("sampler {}: {:016X}\n", i, asset->samplerTable[i]);
		}

		for (auto set = 0u; set < std::size(asset->imageSets); set++)
		{
			const auto image_set = asset->imageSets[set];
			for (auto i = 0u; image_set && image_set->entries && i < image_set->entryCount; i++)
			{
				text += std::format("imageset {} entry {}: {} {} {}\n", set, i, image_name(image_set->entries[i].image),
					image_set->entries[i].instanceDataIndex, hex(image_set->entries[i].__pad0, sizeof(image_set->entries[i].__pad0)));
			}
		}

		text += std::format("data: {} bytes\n", asset->physicalData.dataSize);
		file.write(text);
		file.close();
	}
}

namespace zonetool::t9
{
	template void material::read(zone_reader&, Material*);
	template void material::read(zone_writer&, Material*);
}
