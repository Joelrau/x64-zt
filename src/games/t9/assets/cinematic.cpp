#include <std_include.hpp>
#include "cinematic.hpp"

namespace zonetool::t9
{
	namespace
	{
		void read_highlight_reel_info(auto& reader, HighlightReelInfo& info)
		{
			if (const auto sequences = reader.read_array(info.sequences, 7, info.sequenceCount))
			{
				for (auto i = 0; i < info.sequenceCount; i++)
				{
					reader.read_asset(ASSET_TYPE_CINEMATIC_SEQUENCE, sequences[i].sequence);
				}
			}

			reader.read_array(info.segments, 3, info.segmentCount);
		}
	}

	void cinematic_camera::read(auto&, CinematicCamera*)
	{
	}

	void cinematic_sequence::read(auto& reader, CinematicSequence* asset)
	{
		if (const auto shots = reader.read_array(asset->shots, 7, asset->shotCount))
		{
			for (auto i = 0; i < asset->shotCount; i++)
			{
				reader.read_asset(ASSET_TYPE_CINEMATIC_CAMERA, shots[i].camera);
			}
		}
	}

	void spectate_camera::read(auto& reader, SpectateCamera* asset)
	{
		const auto entries = reader.read_array(asset->entries, 7, asset->entryCount);
		if (!entries)
		{
			return;
		}

		for (auto i = 0; i < asset->entryCount; i++)
		{
			auto& entry = entries[i];
			if (const auto shots = reader.read_array(entry.shots, 7, entry.shotCount))
			{
				for (auto j = 0; j < entry.shotCount; j++)
				{
					reader.read_asset(ASSET_TYPE_CINEMATIC_CAMERA, shots[j].camera);
				}
			}

			reader.read_array(entry.points, 3, entry.pointCount);
		}
	}

	void highlight_reel_info_defines::read(auto& reader, HighlightReelInfoDefines* asset)
	{
		if (const auto infos = reader.read_array(asset->infos, 7, asset->infoCount))
		{
			for (auto i = 0; i < asset->infoCount; i++)
			{
				read_highlight_reel_info(reader, infos[i]);
			}
		}

		if (const auto pointers = reader.read_array(asset->infoPointers, 7, asset->infoCount))
		{
			for (auto i = 0; i < asset->infoCount; i++)
			{
				if (const auto info = reader.read_single(pointers[i], 7))
				{
					read_highlight_reel_info(reader, *info);
				}
			}
		}
	}

	void highlight_reel_value_table::read(auto& reader, HighlightReelValueTable* asset)
	{
		reader.read_array(asset->values, 3, asset->valueCount);
	}
}

namespace zonetool::t9
{
	template void cinematic_camera::read(zone_reader&, CinematicCamera*);
	template void cinematic_sequence::read(zone_reader&, CinematicSequence*);
	template void spectate_camera::read(zone_reader&, SpectateCamera*);
	template void highlight_reel_info_defines::read(zone_reader&, HighlightReelInfoDefines*);
	template void highlight_reel_value_table::read(zone_reader&, HighlightReelValueTable*);
	template void cinematic_camera::read(zone_writer&, CinematicCamera*);
	template void cinematic_sequence::read(zone_writer&, CinematicSequence*);
	template void spectate_camera::read(zone_writer&, SpectateCamera*);
	template void highlight_reel_info_defines::read(zone_writer&, HighlightReelInfoDefines*);
	template void highlight_reel_value_table::read(zone_writer&, HighlightReelValueTable*);
}
