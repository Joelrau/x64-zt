#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class vfx : public asset_interface
	{
	private:
		std::string name_;
		ParticleSystemDef* asset_ = nullptr;

	public:
		ParticleSystemDef* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, ParticleSystemDef* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(ParticleSystemDef* asset);
	};

	class vector_field : public asset_interface
	{
	private:
		std::string name_;
		VectorField* asset_ = nullptr;

	public:
		VectorField* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, VectorField* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(VectorField* asset);
	};

	class footstep_vfx : public asset_interface
	{
	private:
		std::string name_;
		FootstepVFX* asset_ = nullptr;

	public:
		FootstepVFX* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, FootstepVFX* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(FootstepVFX* asset);
	};
}
