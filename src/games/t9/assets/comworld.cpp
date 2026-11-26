#include <std_include.hpp>
#include "comworld.hpp"

namespace zonetool::t9
{
	void com_world::read_primary_lights(auto& reader, ComPrimaryLight*& field, const std::uint32_t count)
	{
		const auto lights = reader.read_array(field, 15, count);
		if (!lights)
		{
			return;
		}

		for (auto i = 0u; i < count; i++)
		{
			reader.read_asset(ASSET_TYPE_IMAGE, lights[i].image);
		}
	}

	void com_world::read(auto& reader, ComWorld* asset)
	{
		read_primary_lights(reader, asset->primaryLights, asset->primaryLightCount);
		read_record_groups(reader, asset->groups20, asset->groups20Count);
		reader.read_array(asset->values1, 3, asset->values1Count);
		reader.read_array(asset->values2, 3, asset->values2Count);
		read_record_groups(reader, asset->groups28, asset->groups28Count);
	}

	void com_world::dump(ComWorld* asset) // TODO: research dump, remove
	{
		const auto folder = std::format("commap/{}/", xhash_name(asset->name));
		const auto write_raw = [&](const std::string& name, const void* data, const std::size_t size)
		{
			if (!data || !size)
			{
				return;
			}

			filesystem::file file(folder + name);
			file.open("wb");
			file.write(data, size, 1);
			file.close();
		};

		write_raw("header", asset, sizeof(ComWorld));
		write_raw("lights", asset->primaryLights, sizeof(ComPrimaryLight) * asset->primaryLightCount);
		write_raw("groups20", asset->groups20, sizeof(ComRecordGroup<ComRecord20>) * asset->groups20Count);
		write_raw("values1", asset->values1, sizeof(std::uint32_t) * asset->values1Count);
		write_raw("values2", asset->values2, sizeof(std::uint32_t) * asset->values2Count);
		write_raw("groups28", asset->groups28, sizeof(ComRecordGroup<ComRecord28>) * asset->groups28Count);

		for (auto i = 0u; i < asset->groups20Count; i++)
		{
			write_raw(std::format("groups20_{}", i), asset->groups20[i].records, sizeof(ComRecord20) * asset->groups20[i].recordCount);
		}

		for (auto i = 0u; i < asset->groups28Count; i++)
		{
			write_raw(std::format("groups28_{}", i), asset->groups28[i].records, sizeof(ComRecord28) * asset->groups28[i].recordCount);
		}

		for (auto i = 0u; i < asset->primaryLightCount; i++)
		{
			const auto* image = asset->primaryLights[i].image;
			ZONETOOL_INFO("commap light %u image %s", i, image ? xhash_name(image->name).data() : "");
		}
	}
}

namespace zonetool::t9
{
	template void com_world::read_primary_lights(zone_reader&, ComPrimaryLight*&, std::uint32_t);
	template void com_world::read(zone_reader&, ComWorld*);
	template void com_world::read_primary_lights(zone_writer&, ComPrimaryLight*&, std::uint32_t);
	template void com_world::read(zone_writer&, ComWorld*);
}
