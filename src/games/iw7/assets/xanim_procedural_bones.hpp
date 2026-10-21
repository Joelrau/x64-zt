#pragma once
#include "../iw7.hpp"

namespace zonetool::iw7
{
	class xanim_procedural_bones : public asset_interface
	{
	private:
		std::string name_;
		XAnimProceduralBones* asset_ = nullptr;
		std::vector<const char*> script_strings_;

	public:
		XAnimProceduralBones* parse(const std::string& name, zone_memory* mem);

		void init(const std::string& name, zone_memory* mem) override;
		void prepare(zone_buffer* buf, zone_memory* mem) override;

		void* pointer() override { return asset_; }
		bool referenced() override { return name_.starts_with(","); }
		std::string name() override;
		std::int32_t type() override;
		static void read(zone_reader& reader, XAnimProceduralBones* asset);
		void write(zone_base* zone, zone_buffer* buffer) override;

		static void dump(XAnimProceduralBones* asset);
	};
}
