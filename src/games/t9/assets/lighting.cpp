#include <std_include.hpp>
#include "lighting.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_image_ref(auto& reader, LightingImageRef*& field)
		{
			if (const auto ref = reader.read_array(field, 7))
			{
				reader.read_asset(ASSET_TYPE_IMAGE, ref->image);
			}
		}

		void read_probe(auto& reader, LightingProbe*& field)
		{
			const auto probe = reader.read_array(field, 15);
			if (!probe)
			{
				return;
			}

			reader.read_asset(ASSET_TYPE_STREAMKEY, probe->streamKey);
			reader.read_array(probe->values1, 3, probe->valueCount);
			reader.read_array(probe->values2, 3, probe->valueCount);
			read_image_ref(reader, probe->imageRef1);

			if (const auto buffer = reader.read_array(probe->buffer, 15))
			{
				read_gfx_buffer(reader, buffer);
			}

			read_image_ref(reader, probe->imageRef2);
		}

		void read_grid_set(auto& reader, LightingGridSet& set)
		{
			if (const auto grids = reader.read_array(set.grids, 7, set.gridCount))
			{
				for (auto i = 0u; i < set.gridCount; i++)
				{
					auto& grid = grids[i];
					reader.read_array(grid.points, 3, static_cast<std::size_t>(grid.sizeX) * grid.sizeY * grid.sizeZ);

					if (const auto cells = reader.read_array(grid.cells, 7, grid.cellCount))
					{
						for (auto j = 0u; j < grid.cellCount; j++)
						{
							reader.read_array(cells[j].values, 3, cells[j].valueCount);
						}
					}
				}
			}

			reader.read_array(set.table, 3, static_cast<std::size_t>(set.tableSize) * set.tableSize);
			reader.read_array(set.values, 3, set.valueCount);
		}

		void read_volume(auto& reader, LightingVolume& volume)
		{
			for (auto& slot : volume.probeSlots)
			{
				read_probe(reader, slot.probe);
			}

			reader.read_array(volume.values, 1, volume.valueCount);

			for (auto& group : volume.groups)
			{
				reader.read_array(group.records1, 15, group.records1Count + 1);
				reader.read_array(group.records2, 15, group.records2Count);
				read_occlusion_buffer(reader, group.buffer);
				reader.read_asset_array(ASSET_TYPE_IMAGE, group.images, group.imageCount);
			}

			for (auto& list : volume.imageTripleLists)
			{
				if (const auto triples = reader.read_array(list.triples, 7, list.tripleCount))
				{
					for (auto i = 0ull; i < list.tripleCount; i++)
					{
						for (auto& image : triples[i].images)
						{
							reader.read_asset(ASSET_TYPE_IMAGE, image);
						}
					}
				}
			}

			for (auto& image : volume.images)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, image);
			}

			for (auto& ref : volume.modelRefs)
			{
				reader.read_asset(ASSET_TYPE_XMODEL, ref.model);
				reader.read_asset(ASSET_TYPE_IMAGE, ref.image);
				reader.read_asset(ASSET_TYPE_KLF, ref.klf);
			}

			for (auto& set : volume.gridSets)
			{
				read_grid_set(reader, set);
			}

			read_occlusion_buffer(reader, volume.occlusionBuffer);
		}
	}

	void lighting::read(auto& reader, Lighting* asset)
	{
		com_world::read_primary_lights(reader, asset->primaryLights, asset->primaryLightCount);
		com_world::read_record_groups(reader, asset->groups20, asset->groups20Count);
		com_world::read_record_groups(reader, asset->groups28, asset->groups28Count);

		if (const auto volumes = reader.read_array(asset->volumes, 15, asset->volumeCount))
		{
			for (auto i = 0u; i < asset->volumeCount; i++)
			{
				read_volume(reader, volumes[i]);
			}
		}

		reader.read_array(asset->values1, 3, asset->values1Count);
		reader.read_array(asset->volumeValues, 3, asset->volumeCount);
		reader.read_array(asset->values2, 3, asset->values2Count);

		for (auto& image : asset->images)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, image);
		}

		if (const auto sets = reader.read_array(asset->probeSets, 15, asset->probeSetCount))
		{
			for (auto i = 0u; i < asset->probeSetCount; i++)
			{
				for (auto& slot : sets[i].slots)
				{
					read_probe(reader, slot.probe);
				}
			}
		}

		reader.read_array(asset->values3, 3, asset->values3Count);
		reader.read_array(asset->records1, 15, asset->records1Count);
		reader.read_array(asset->values4, 3, asset->values4Count);

		if (const auto sets = reader.read_array(asset->imageSets, 7, asset->imageSetCount))
		{
			for (auto i = 0ull; i < asset->imageSetCount; i++)
			{
				for (auto& image : sets[i].images)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, image);
				}
			}
		}

		reader.read_array(asset->values5, 3, asset->values5Count);
		reader.read_array(asset->records2, 3, asset->records2Count);
		reader.read_array(asset->values6, 3, asset->values6Count);
		reader.read_array(asset->records3, 15, asset->records3Count);
		reader.read_array(asset->records4, 3, asset->records4Count);
		reader.read_array(asset->records5, 15, asset->records5Count);
		reader.read_array(asset->values7, 3, asset->values7Count);

		if (const auto refs = reader.read_array(asset->windRefs, 7, asset->windRefCount))
		{
			for (auto i = 0ull; i < asset->windRefCount; i++)
			{
				reader.read_asset(ASSET_TYPE_WINDDEF, refs[i].windDef);
			}
		}

		reader.read_array(asset->values9, 3, asset->values9Count);
		reader.read_array(asset->values8, 3, asset->values8Count);

		if (const auto records = reader.read_array(asset->imageRecords, 7, asset->imageRecordCount))
		{
			for (auto i = 0u; i < asset->imageRecordCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, records[i].image);
			}
		}

		if (const auto records = reader.read_array(asset->imagePairRecords, 7, asset->imagePairRecordCount))
		{
			for (auto i = 0u; i < asset->imagePairRecordCount; i++)
			{
				for (auto& image : records[i].images)
				{
					reader.read_asset(ASSET_TYPE_IMAGE, image);
				}

				reader.read_array(records[i].values, 3, records[i].valueCount);
			}
		}

		if (const auto records = reader.read_array(asset->imageValuesRecords, 7, asset->imageValuesRecordCount))
		{
			for (auto i = 0u; i < asset->imageValuesRecordCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, records[i].image);
				reader.read_array(records[i].values1, 3, records[i].valueCount);
				reader.read_array(records[i].values2, 3, records[i].valueCount);
				reader.read_array(records[i].values3, 3, records[i].valueCount);
			}
		}

		if (const auto records = reader.read_array(asset->imageRecordsLarge, 7, asset->imageRecordLargeCount))
		{
			for (auto i = 0u; i < asset->imageRecordLargeCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, records[i].image);
			}
		}
	}

	namespace
	{
		template <typename T>
		void write_raw(const std::string& path, const T* data, const std::size_t count) // TODO: research dump, remove
		{
			if (!data || !count)
			{
				return;
			}

			filesystem::file file(path);
			file.open("wb");
			file.write(data, sizeof(T) * count, 1);
			file.close();
		}

		std::string image_name(const GfxImage* image)
		{
			return image ? xhash_name(image->name) : "";
		}

		std::string image_ref_name(const LightingImageRef* ref)
		{
			return ref ? image_name(ref->image) : "";
		}

		void write_raw_probe(const std::string& prefix, const LightingProbe* probe)
		{
			if (!probe)
			{
				return;
			}

			write_raw(prefix + "probe", probe, 1);
			write_raw(prefix + "values1", probe->values1, probe->valueCount);
			write_raw(prefix + "values2", probe->values2, probe->valueCount);
			ZONETOOL_INFO("%s streamkey %s image1 %s image2 %s", prefix.data(),
				probe->streamKey ? xhash_name(probe->streamKey->name).data() : "",
				image_ref_name(probe->imageRef1).data(), image_ref_name(probe->imageRef2).data());
		}
	}

	void lighting::dump(Lighting* asset)
	{
		const auto folder = std::format("lighting/{}/", xhash_name(asset->name));
		write_raw(folder + "header", asset, 1);
		write_raw(folder + "lights", asset->primaryLights, asset->primaryLightCount);
		write_raw(folder + "volumes", asset->volumes, asset->volumeCount);
		write_raw(folder + "values1", asset->values1, asset->values1Count);
		write_raw(folder + "volumevalues", asset->volumeValues, asset->volumeCount);
		write_raw(folder + "values2", asset->values2, asset->values2Count);
		write_raw(folder + "probesets", asset->probeSets, asset->probeSetCount);
		write_raw(folder + "values3", asset->values3, asset->values3Count);
		write_raw(folder + "records1", asset->records1, asset->records1Count);
		write_raw(folder + "values4", asset->values4, asset->values4Count);
		write_raw(folder + "imagesets", asset->imageSets, asset->imageSetCount);
		write_raw(folder + "values5", asset->values5, asset->values5Count);
		write_raw(folder + "records2", asset->records2, asset->records2Count);
		write_raw(folder + "values6", asset->values6, asset->values6Count);
		write_raw(folder + "records3", asset->records3, asset->records3Count);
		write_raw(folder + "records4", asset->records4, asset->records4Count);
		write_raw(folder + "records5", asset->records5, asset->records5Count);
		write_raw(folder + "values7", asset->values7, asset->values7Count);
		write_raw(folder + "values8", asset->values8, asset->values8Count);
		write_raw(folder + "values9", asset->values9, asset->values9Count);
		write_raw(folder + "imagerecords", asset->imageRecords, asset->imageRecordCount);
		write_raw(folder + "imagepairrecords", asset->imagePairRecords, asset->imagePairRecordCount);
		write_raw(folder + "imagevaluesrecords", asset->imageValuesRecords, asset->imageValuesRecordCount);
		write_raw(folder + "imagerecordslarge", asset->imageRecordsLarge, asset->imageRecordLargeCount);

		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			ZONETOOL_INFO("light %u image %s", i, image_name(asset->primaryLights[i].image).data());
		}

		for (auto i = 0u; i < 4; i++)
		{
			ZONETOOL_INFO("image %u %s", i, image_name(asset->images[i]).data());
		}

		for (auto i = 0u; i < asset->imageRecordCount; i++)
		{
			ZONETOOL_INFO("imagerecord %u %s", i, image_name(asset->imageRecords[i].image).data());
		}

		for (auto i = 0u; i < asset->imageValuesRecordCount; i++)
		{
			ZONETOOL_INFO("imagevaluesrecord %u %s", i, image_name(asset->imageValuesRecords[i].image).data());
		}

		for (auto i = 0u; i < asset->imageRecordLargeCount; i++)
		{
			ZONETOOL_INFO("imagerecordlarge %u %s", i, image_name(asset->imageRecordsLarge[i].image).data());
		}

		for (auto i = 0ull; i < asset->imageSetCount; i++)
		{
			for (auto j = 0u; j < 6; j++)
			{
				ZONETOOL_INFO("imageset %llu %u %s", i, j, image_name(asset->imageSets[i].images[j]).data());
			}
		}

		for (auto i = 0u; i < asset->probeSetCount; i++)
		{
			for (auto p = 0u; p < 4; p++)
			{
				write_raw_probe(std::format("{}probeset{}_{}_", folder, i, p), asset->probeSets[i].slots[p].probe);
			}
		}

		for (auto v = 0u; v < asset->volumeCount; v++)
		{
			const auto& volume = asset->volumes[v];
			const auto prefix = std::format("{}volume{}_", folder, v);
			write_raw(prefix + "values", volume.values, volume.valueCount);

			for (auto p = 0u; p < 4; p++)
			{
				write_raw_probe(std::format("{}probe{}_", prefix, p), volume.probeSlots[p].probe);
			}

			for (auto i = 0u; i < 4; i++)
			{
				ZONETOOL_INFO("volume %u image %u %s", v, i, image_name(volume.images[i]).data());
			}

			for (auto s = 0u; s < 4; s++)
			{
				const auto& set = volume.gridSets[s];
				const auto set_prefix = std::format("{}set{}_", prefix, s);
				write_raw(set_prefix + "grids", set.grids, set.gridCount);
				write_raw(set_prefix + "table", set.table, static_cast<std::size_t>(set.tableSize) * set.tableSize);
				write_raw(set_prefix + "values", set.values, set.valueCount);

				for (auto g = 0u; g < set.gridCount; g++)
				{
					const auto& grid = set.grids[g];
					const auto grid_prefix = std::format("{}grid{}_", set_prefix, g);
					write_raw(grid_prefix + "points", grid.points, static_cast<std::size_t>(grid.sizeX) * grid.sizeY * grid.sizeZ);
					write_raw(grid_prefix + "cells", grid.cells, grid.cellCount);

					for (auto c = 0u; c < grid.cellCount; c++)
					{
						write_raw(std::format("{}cell{}", grid_prefix, c), grid.cells[c].values, grid.cells[c].valueCount);
					}
				}
			}
		}
	}
}

namespace zonetool::t9
{
	template void lighting::read(zone_reader&, Lighting*);
	template void lighting::read(zone_writer&, Lighting*);
}
