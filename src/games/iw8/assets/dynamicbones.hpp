#pragma once
#include "../iw8.hpp"

namespace zonetool::iw8
{
	class dynamic_bones : public asset_interface
	{
	private:
		std::string name_;
		XAnimDynamicBones* asset_ = nullptr;
		script_string_map script_strings_;

	public:
		XAnimDynamicBones* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;
		void load_depending(zone_base* zone) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, XAnimDynamicBones* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(XAnimDynamicBones* asset);
	};

	class anim_curve : public asset_interface
	{
	private:
		std::string name_;
		XAnimCurve* asset_ = nullptr;

	public:
		XAnimCurve* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, XAnimCurve* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(XAnimCurve* asset);
	};
}
