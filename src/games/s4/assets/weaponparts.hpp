#pragma once
#include "../s4.hpp"

namespace zonetool::s4
{
	class beam : public asset_interface
	{
	private:
		std::string name_;
		Beam* asset_ = nullptr;

	public:
		Beam* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, Beam* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(Beam* asset);
	};

	class curve : public asset_interface
	{
	private:
		std::string name_;
		Curve* asset_ = nullptr;

	public:
		Curve* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, Curve* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(Curve* asset);
	};

	class trigger_effect : public asset_interface
	{
	private:
		std::string name_;
		TriggerEffect* asset_ = nullptr;

	public:
		TriggerEffect* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, TriggerEffect* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(TriggerEffect* asset);
	};

	class weapon_trigger : public asset_interface
	{
	private:
		std::string name_;
		WeaponTrigger* asset_ = nullptr;

	public:
		WeaponTrigger* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, WeaponTrigger* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WeaponTrigger* asset);
	};

	class reactive_stage_set : public asset_interface
	{
	private:
		std::string name_;
		ReactiveStageSet* asset_ = nullptr;

	public:
		ReactiveStageSet* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, ReactiveStageSet* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(ReactiveStageSet* asset);
	};

	class weapon_accuracy : public asset_interface
	{
	private:
		std::string name_;
		WeaponAccuracy* asset_ = nullptr;

	public:
		WeaponAccuracy* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, WeaponAccuracy* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(WeaponAccuracy* asset);
	};
}
