#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class rumble : public asset_interface
	{
	private:
		std::string name_;
		RumbleInfo* asset_ = nullptr;

	public:
		RumbleInfo* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, RumbleInfo* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(RumbleInfo* asset);
	};

	class rumble_graph : public asset_interface
	{
	private:
		std::string name_;
		RumbleGraph* asset_ = nullptr;

	public:
		RumbleGraph* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, RumbleGraph* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(RumbleGraph* asset);
	};
}
