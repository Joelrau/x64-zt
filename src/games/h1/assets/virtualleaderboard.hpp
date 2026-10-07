#pragma once
#include "../h1.hpp"

namespace zonetool::h1
{
	class virtual_leaderboard : public asset_interface
	{
	private:
		std::string name_;
		VirtualLeaderboardDef* asset_ = nullptr;

	public:
		VirtualLeaderboardDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, VirtualLeaderboardDef* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(VirtualLeaderboardDef* asset);
	};
}
