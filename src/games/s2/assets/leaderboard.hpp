#pragma once

namespace zonetool::s2
{
	struct leaderboard_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(LeaderboardDef, name));
			w.entries(offsetof(LeaderboardDef, columns), sizeof(LeaderboardColumnDef),
				w.template at<int>(offsetof(LeaderboardDef, columnCount)), [](const auto& column, std::size_t)
			{
				column.string(offsetof(LeaderboardColumnDef, name));
				column.string(offsetof(LeaderboardColumnDef, statName));
			});
		}
	};

	class leaderboard : public field_layout_asset<LeaderboardDef, ASSET_TYPE_LEADERBOARDDEF, leaderboard_layout>
	{
	};

	struct virtual_leaderboard_layout
	{
		template <typename V>
		static void visit(const V& w)
		{
			w.string(offsetof(VirtualLeaderboardDef, name));
			w.string(offsetof(VirtualLeaderboardDef, sourceName));
			w.entries(offsetof(VirtualLeaderboardDef, columns), sizeof(VirtualLeaderboardColumnDef),
				w.template at<int>(offsetof(VirtualLeaderboardDef, columnCount)), [](const auto& column, std::size_t)
			{
				column.string(offsetof(VirtualLeaderboardColumnDef, name));
			});
		}
	};

	class virtual_leaderboard : public field_layout_asset<VirtualLeaderboardDef, ASSET_TYPE_VIRTUAL_LEADERBOARD, virtual_leaderboard_layout>
	{
	};
}
