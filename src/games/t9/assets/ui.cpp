#include <std_include.hpp>
#include "ui.hpp"

namespace zonetool::t9
{
	void font_icon::read(auto& reader, FontIcon* asset)
	{
		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0; i < asset->entryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, entries[i].image);
			}
		}

		reader.read_array(asset->aliases, 3, asset->aliasCount);
	}

	void objective::read(auto& reader, Objective* asset)
	{
		reader.read_string(asset->string);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image0);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image1);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image2);
		read_script_bundle_object(reader, asset->bundle);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image3);
	}

	void objective_list::read(auto& reader, ObjectiveList* asset)
	{
		reader.read_asset_array(ASSET_TYPE_OBJECTIVE, asset->objectives, asset->objectiveCount);
	}

	void ttf::read(auto& reader, TTFDef* asset)
	{
		reader.read_array(asset->glyphs, 3, asset->glyphCount);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->image);
		reader.read_asset(ASSET_TYPE_TTF, asset->fallback0);
		reader.read_asset(ASSET_TYPE_TTF, asset->fallback1);
		reader.read_array(asset->file, 15, asset->fileSize + 1);

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->kerningEntries, 1, asset->kerningCount);
		reader.pop_stream();

		reader.push_stream(XFILE_BLOCK_RUNTIME_VIRTUAL);
		reader.read_array(asset->glyphEntries, 1, asset->glyphEntryCount);
		reader.pop_stream();
	}

	void compass_tunables::read(auto& reader, CompassTunables* asset)
	{
		reader.read_asset(ASSET_TYPE_TTF, asset->mapData.font);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->mapData.image0);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->mapData.image1);
		reader.read_asset(ASSET_TYPE_IMAGE, asset->mapData.image2);
		reader.read_array(asset->label0, 0, 0x15);
		reader.read_array(asset->label1, 0, 0x15);

		if (const auto entries = reader.read_array(asset->entries, 7, asset->entryCount))
		{
			for (auto i = 0u; i < asset->entryCount; i++)
			{
				reader.read_asset(ASSET_TYPE_IMAGE, entries[i].image);

				for (auto& set : entries[i].iconSets)
				{
					for (auto& image : set.images)
					{
						reader.read_asset(ASSET_TYPE_IMAGE, image);
					}
				}
			}
		}
	}

	void overhead_camera_tunables::read(auto& reader, OverheadCameraTunables* asset)
	{
		const auto read_values = [&](OverheadCameraValues& values)
		{
			reader.read_array(values.values0, 3, values.valueCount0);
			reader.read_array(values.values1, 3, values.valueCount1);
			reader.read_array(values.values2, 3, values.valueCount2);
			reader.read_array(values.values3, 3, values.valueCount3);
		};

		read_values(asset->values0);
		read_values(asset->values1);
		reader.read_asset(ASSET_TYPE_CINEMATICMOTION, asset->cinematicMotion);
	}

	void player_global_tunables::read(auto& reader, PlayerGlobalTunables* asset)
	{
		for (auto& set : asset->sets)
		{
			for (auto& values : set.values0.values)
			{
				read_value_array(reader, values);
			}

			for (auto& group : set.values1.groups)
			{
				for (auto& values : group.values)
				{
					read_value_array(reader, values);
				}
			}

			for (auto& values : set.values1.values)
			{
				read_value_array(reader, values);
			}
		}

		for (auto& values : asset->values)
		{
			read_value_array(reader, values);
		}

		for (auto& motion : asset->cinematicMotions)
		{
			reader.read_asset(ASSET_TYPE_CINEMATICMOTION, motion);
		}
	}
}

namespace zonetool::t9
{
	template void font_icon::read(zone_reader&, FontIcon*);
	template void objective::read(zone_reader&, Objective*);
	template void objective_list::read(zone_reader&, ObjectiveList*);
	template void ttf::read(zone_reader&, TTFDef*);
	template void compass_tunables::read(zone_reader&, CompassTunables*);
	template void overhead_camera_tunables::read(zone_reader&, OverheadCameraTunables*);
	template void player_global_tunables::read(zone_reader&, PlayerGlobalTunables*);
	template void font_icon::read(zone_writer&, FontIcon*);
	template void objective::read(zone_writer&, Objective*);
	template void objective_list::read(zone_writer&, ObjectiveList*);
	template void ttf::read(zone_writer&, TTFDef*);
	template void compass_tunables::read(zone_writer&, CompassTunables*);
	template void overhead_camera_tunables::read(zone_writer&, OverheadCameraTunables*);
	template void player_global_tunables::read(zone_writer&, PlayerGlobalTunables*);
}
