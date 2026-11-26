#pragma once
#include "../t9.hpp"

namespace zonetool::t9
{
	class footstep_table
	{
	public:
		static void read(auto& reader, FootstepTable* asset);
	};

	class string_table
	{
	public:
		static void read(auto& reader, StringTable* asset);
	};

	class structured_table
	{
	public:
		static void read(auto& reader, StructuredTable* asset);
	};

	class script_bundle_list
	{
	public:
		static void read(auto& reader, ScriptBundleList* asset);
	};

	class map_table_entry
	{
	public:
		static void read(auto& reader, MapTableEntry* asset);
	};

	class map_table
	{
	public:
		static void read(auto& reader, MapTable* asset);
	};

	class map_table_list
	{
	public:
		static void read(auto& reader, MapTableList* asset);
	};

	class bitfield
	{
	public:
		static void read(auto& reader, BitField* asset);
	};

	class leaderboard_def
	{
	public:
		static void read(auto& reader, LeaderboardDef* asset);
	};

	class cg_media_table
	{
	public:
		static void read(auto& reader, CgMediaTable* asset);
	};

	class bullet_penetration
	{
	public:
		static void read(auto& reader, BulletPenetration* asset);
	};

	class loc_dmg_table
	{
	public:
		static void read(auto& reader, LocDmgTable* asset);
	};

	class feature_table
	{
	public:
		static void read(auto&, FeatureTable*)
		{
		}
	};

	class playlists
	{
	public:
		static void read(auto& reader, Playlists* asset);
	};

	class playlist_schedule
	{
	public:
		static void read(auto& reader, PlaylistSchedule* asset);
	};

	class arena_seasons
	{
	public:
		static void read(auto& reader, ArenaSeasons* asset);
	};

	class scenario
	{
	public:
		static void read(auto& reader, Scenario* asset);
	};

	class dlog_event
	{
	public:
		static void read(auto& reader, DLogEvent* asset);
	};
}
