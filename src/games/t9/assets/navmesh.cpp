#include <std_include.hpp>
#include "navmesh.hpp"

namespace zonetool::t9
{
	void nav_mesh::read(auto& reader, NavMesh* asset)
	{
		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey1);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_STREAMKEY, entries[i].streamKey);
			}
		}

		reader.read_asset(ASSET_TYPE_STREAMKEY, asset->streamKey2);
	}

	void nav_volume::read(auto& reader, NavVolume* asset)
	{
		const auto count = static_cast<std::size_t>(asset->entryCount);
		if (const auto entries = reader.read_array(asset->entries, 7, count))
		{
			for (auto i = 0u; i < count; i++)
			{
				reader.read_asset(ASSET_TYPE_STREAMKEY, entries[i].streamKey1);
				reader.read_asset(ASSET_TYPE_STREAMKEY, entries[i].streamKey2);
			}
		}
	}
}

namespace zonetool::t9
{
	template void nav_mesh::read(zone_reader&, NavMesh*);
	template void nav_volume::read(zone_reader&, NavVolume*);
	template void nav_mesh::read(zone_writer&, NavMesh*);
	template void nav_volume::read(zone_writer&, NavVolume*);
}
