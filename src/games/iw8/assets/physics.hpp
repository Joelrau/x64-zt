#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class physics_sfx_event : public asset_interface
	{
	private:
		std::string name_;
		PhysicsSFXEventAsset* asset_ = nullptr;

	public:
		PhysicsSFXEventAsset* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, PhysicsSFXEventAsset* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysicsSFXEventAsset* asset);
	};

	class physics_vfx_event : public asset_interface
	{
	private:
		std::string name_;
		PhysicsVFXEventAsset* asset_ = nullptr;

	public:
		PhysicsVFXEventAsset* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, PhysicsVFXEventAsset* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysicsVFXEventAsset* asset);
	};

	class physics_asset : public asset_interface
	{
	private:
		std::string name_;
		PhysicsAsset* asset_ = nullptr;

	public:
		PhysicsAsset* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, PhysicsAsset* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysicsAsset* asset);
	};

	class physics_fx_shape : public asset_interface
	{
	private:
		std::string name_;
		PhysicsFXShape* asset_ = nullptr;

	public:
		PhysicsFXShape* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, PhysicsFXShape* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysicsFXShape* asset);
	};

	class physics_fx_pipeline : public asset_interface
	{
	private:
		std::string name_;
		PhysicsFXPipeline* asset_ = nullptr;

	public:
		PhysicsFXPipeline* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, PhysicsFXPipeline* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(PhysicsFXPipeline* asset);
	};
}
