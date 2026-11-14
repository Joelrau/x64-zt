#include <std_include.hpp>
#include "vehicletrack.hpp"

namespace zonetool::h1
{
	namespace
	{
		void read_segment(zone_reader& reader, VehicleTrackSegment* segment);

		void read_sectors(zone_reader& reader, VehicleTrackSegment* segment)
		{
			if (const auto sectors = reader.read_array(segment->sectors, 3, segment->sectorCount))
			{
				for (auto i = 0u; i < segment->sectorCount; i++)
				{
					reader.read_array(sectors[i].obstacles, 3, sectors[i].obstacleCount);
				}
			}
		}

		void read_branches(zone_reader& reader, VehicleTrackSegment**& branches, const unsigned int count)
		{
			if (const auto entries = reader.read_array(branches, 7, count))
			{
				for (auto i = 0u; i < count; i++)
				{
					if (const auto segment = reader.read_single(entries[i], 3))
					{
						read_segment(reader, segment);
					}
				}
			}
		}

		void read_segment(zone_reader& reader, VehicleTrackSegment* segment)
		{
			reader.read_string(segment->targetName);
			read_sectors(reader, segment);
			read_branches(reader, segment->nextBranches, segment->nextBranchesCount);
			read_branches(reader, segment->prevBranches, segment->prevBranchesCount);
		}
	}

	void vehicle_track::read(zone_reader& reader, VehicleTrack* asset)
	{
		reader.push_stream(XFILE_BLOCK_VIRTUAL);
		reader.read_string(asset->name);

		if (const auto segments = reader.read_array(asset->segments, 3, asset->segmentCount))
		{
			for (auto i = 0u; i < asset->segmentCount; i++)
			{
				read_segment(reader, &segments[i]);
			}
		}

		reader.pop_stream();
	}
}
